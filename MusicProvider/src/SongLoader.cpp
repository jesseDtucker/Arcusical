#include "pch.h"

#define PARSER_VERSOBE 0

#if PARSER_VERSOBE
#include <sstream>
#include <debugapi.h>
#endif

#include <algorithm>
#include <cctype>
#include <codecvt>
#include <ppltasks.h>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "Album.hpp"
#include "Arc_Assert.hpp"
#include "IFileReader.hpp"
#include "IFile.hpp"
#include "IFolder.hpp"
#include "ImageTypes.hpp"
#include "Song.hpp"
#include "SongIdMapper.hpp"
#include "SongLoader.hpp"
#include "Storage.hpp"

namespace Arcusical
{
namespace MusicProvider
{
	static Model::Song LoadMpeg4Song(FileSystem::IFile& file);
	static Model::Song DefaultSongLoad(FileSystem::IFile& file, Model::AudioFormat encoding = Model::AudioFormat::UNKNOWN, Model::ContainerType container = Model::ContainerType::UNKNOWN);
	static Model::Song LoadMP3(FileSystem::IFile& file);
	static Model::Song LoadWav(FileSystem::IFile& file);

	static std::pair<bool, std::wstring> DefaultLoadAlbumImage(FileSystem::IFile& file, const std::wstring& albumName);
	static std::pair<bool, std::wstring> LoadAlbumImageFromMpeg4(FileSystem::IFile& file, const std::wstring& albumName);
	static std::pair<bool, std::wstring> AttemptToFindAlbumArt(FileSystem::IFile& file, const std::wstring& albumName);
	static std::wstring SaveImageFile(std::vector<unsigned char>& imgData, const std::wstring& albumName, MPEG4::ImageType imageType);

	static boost::uuids::random_generator s_idGenerator;

	static const std::unordered_map<MPEG4::Encoding, Model::AudioFormat> MPEG4_TO_MODEL_MAPPING =
	{
		{ MPEG4::Encoding::AAC, Model::AudioFormat::AAC },
		{ MPEG4::Encoding::ALAC, Model::AudioFormat::ALAC },
		{ MPEG4::Encoding::UNKNOWN, Model::AudioFormat::UNKNOWN }
	};

	static const std::unordered_map<std::wstring, std::function<Model::Song(FileSystem::IFile&)>> FILE_EX_TO_LOADER =
	{
		{ L"m4a", LoadMpeg4Song },
		{ L"mp3", LoadMP3 },
		{ L"wav", LoadWav }
	};

	static const std::unordered_map<Model::ContainerType, std::function<std::pair<bool, std::wstring>(FileSystem::IFile& file, const std::wstring& albumName)>> FILE_EX_TO_IMAGE_LOADER =
	{
		{ Model::ContainerType::MP4, LoadAlbumImageFromMpeg4 }
	};

	static const std::unordered_set<std::wstring> POSSIBLE_IMAGE_FILE_EXTENSIONS = { L"png", L"bmp", L"jpg", L"jpeg" };

	Model::Song LoadSong(FileSystem::IFile& file)
	{
		Model::Song result;
		auto fileExtension = file.GetExtension();

		auto fileName = file.GetFullName();

		auto loadFunc = FILE_EX_TO_LOADER.find(fileExtension);
		if (loadFunc != std::end(FILE_EX_TO_LOADER))
		{
			result = loadFunc->second(file);
		}
		else
		{
			result = DefaultSongLoad(file);
		}

		return result;
	}

	Model::Album CreateAlbum(std::wstring& name, Model::Song& song, std::shared_ptr<SongIdMapper>& mapper)
	{
		auto newAlbum = Model::Album(mapper);

		newAlbum.SetId(s_idGenerator());
		newAlbum.SetArtist(song.GetArtist());
		newAlbum.GetMutableSongIds().insert(song.GetId());
		newAlbum.SetTitle(name);

		newAlbum.SetImageFilePath(LoadAlbumImage(song, name, true));

		return newAlbum;
	}

	Model::Song LoadMpeg4Song(FileSystem::IFile& file)
	{
		MPEG4::MPEG4_Parser mpegParser;

		auto fileReader = FileSystem::Storage::GetReader(&file);
		auto mpegSong = mpegParser.ReadAndParseFromStream(*fileReader);

		auto result = Model::Song();

		//setup converter
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

		result.SetArtist(converter.from_bytes(mpegSong->GetArtist()));
		result.SetId(s_idGenerator()); // generate a random id, the odds of collision are slim to none
		result.SetLength(mpegSong->GetLength());
		result.SetTitle(converter.from_bytes(mpegSong->GetTitle()));
		result.SetAlbumName(converter.from_bytes(mpegSong->GetAlbum()));

		Model::AudioFormat songFormat = Model::AudioFormat::UNKNOWN;
		auto encodingItr = MPEG4_TO_MODEL_MAPPING.find(mpegSong->GetEncoding());
		ARC_ASSERT(encodingItr != std::end(MPEG4_TO_MODEL_MAPPING));
		if (encodingItr != std::end(MPEG4_TO_MODEL_MAPPING))
		{
			songFormat = encodingItr->second;
		}

		Model::SongFile songFile;
		songFile.filePath = file.GetFullPath();
		songFile.bitRate = mpegSong->GetSampleSize() * mpegSong->GetSampleRate();
		songFile.channelCount = mpegSong->GetNumberOfChannels();
		songFile.sampleSize = mpegSong->GetNumberOfChannels();
		songFile.format = songFormat;
		songFile.container = Model::ContainerType::MP4;
		result.AddFile(songFile);

		ARC_ASSERT(songFile.bitRate != 0);
		ARC_ASSERT(songFile.channelCount != 0);
		ARC_ASSERT(songFile.sampleSize != 0);

#if PARSER_VERSOBE

		std::stringstream stream;
		mpegSong->GetTree()->PrintTree(stream);

		while (!stream.eof())
		{
			std::string output;
			std::getline(stream, output);
			output.append("\n");
			OutputDebugStringA(output.c_str());
		}
#endif

		return result;
	}

	Model::Song DefaultSongLoad(FileSystem::IFile& file, Model::AudioFormat encoding, Model::ContainerType container)
	{
#ifdef __cplusplus_winrt
		using namespace concurrency;

		auto result = Model::Song();
		auto musicProperties = create_task(file.GetRawHandle()->Properties->GetMusicPropertiesAsync()).get();
		
		result.SetArtist(std::wstring(musicProperties->Artist->Data()));
		result.SetId(s_idGenerator()); // generate a random id, the odds of collision are slim to none
		result.SetLength(musicProperties->Duration.Duration / 1000); // Duration is provided is ms, but we need it in seconds
		result.SetTitle(std::wstring(musicProperties->Title->Data()));

		Model::SongFile songFile;
		songFile.filePath = file.GetFullPath();
		songFile.bitRate = musicProperties->Bitrate;
		songFile.channelCount = 0; // no idea how to determine this without parsing...
		songFile.sampleSize = 0; // no idea how to determine this without parsing...
		songFile.format = encoding;
		songFile.container = container;
		result.AddFile(songFile);

		if (result.GetTitle().length() == 0)
		{
			result.SetTitle(file.GetName());
		}

		return result;
#elif
#error "Unsupported platform!"
#endif
	}

	Model::Song LoadMP3(FileSystem::IFile& file)
	{
		return DefaultSongLoad(file, Model::AudioFormat::MP3, Model::ContainerType::MP3);
	}

	Model::Song LoadWav(FileSystem::IFile& file)
	{
		return DefaultSongLoad(file, Model::AudioFormat::WAV, Model::ContainerType::WAV);
	}

	std::wstring LoadAlbumImage(Model::Song& song, const std::wstring& albumName, bool checkFileSystem)
	{
		// for each song attempt to load the image, if successful stop
		// then return the path of the new album image
		
		auto files = song.GetFiles();
		std::wstring filePath = L"";
		bool isFound = false;

		for (auto& audioFile : files)
		{
			auto imageLoadFunc = FILE_EX_TO_IMAGE_LOADER.find(audioFile.second.container);
			if (imageLoadFunc != std::end(FILE_EX_TO_IMAGE_LOADER))
			{
				auto storageFile = FileSystem::Storage::LoadFileFromPath(audioFile.second.filePath);
				auto result = imageLoadFunc->second(*storageFile, albumName);
				if (result.first)
				{
					isFound = true;
					filePath = result.second;
					break;
				}
			}
		}

		if (!isFound)
		{
			// attempt secondary loads
			// first try and load the thumbnail from all copies of the song
			for (auto& audioFile : files)
			{
				auto storageFile = FileSystem::Storage::LoadFileFromPath(audioFile.second.filePath);
				auto result = DefaultLoadAlbumImage(*storageFile, albumName);
				if (result.first)
				{
					isFound = true;
					filePath = result.second;
					break;
				}
			}
			
			if (!isFound && checkFileSystem)
			{
				// if we still don't have it check the file system for an album
				for (auto& audioFile : files)
				{
					auto storageFile = FileSystem::Storage::LoadFileFromPath(audioFile.second.filePath);
					auto result = AttemptToFindAlbumArt(*storageFile, albumName);
					if (result.first)
					{
						isFound = true;
						filePath = result.second;
						break;
					}
				}
			}
		}

		return filePath;
	}

	std::pair<bool, std::wstring> LoadAlbumImageFromMpeg4(FileSystem::IFile& file, const std::wstring& albumName)
	{
		using namespace FileSystem;

		std::pair<bool, std::wstring> result = { false, L"" };

		MPEG4::MPEG4_Parser mpegParser;
		auto reader = Storage::GetReader(&file);
		auto mpegTree = mpegParser.ReadAndParseFromStream(*reader);

		auto imageData = mpegTree->GetImageData();
		if (nullptr != imageData)
		{
			auto path = SaveImageFile(*imageData, albumName, mpegTree->GetImageType());
			result = { true, path };
		}

		return result;
	}

	std::pair<bool, std::wstring> DefaultLoadAlbumImage(FileSystem::IFile& file, const std::wstring& albumName)
	{
		auto thumbnail = file.GetThumbnail(false); // hopefully the thumbnail is the album art, probably won't work well for .wav or .flac...
		if (thumbnail.size() > 0)
		{
			auto path = SaveImageFile(thumbnail, albumName, MPEG4::ImageType::BMP);
			return { true, path };
		}
		else
		{
			return { false, L"" };
		}
	}

	std::wstring SaveImageFile(std::vector<unsigned char>& imgData, const std::wstring& albumName, MPEG4::ImageType imageType)
	{
		using namespace FileSystem;

		auto fileName = L"albumImgs\\" + albumName + L"." + MPEG4::ImageTypeToWString.at(imageType);
		Storage::RemoveIllegalCharacters(fileName);

		ARC_ASSERT_MSG(!Storage::ApplicationFolder().ContainsFile(fileName), "an album image with this name already exists!");
		auto imgFile = Storage::ApplicationFolder().CreateNewFile(fileName);

		imgFile->WriteToFile(imgData);
		return imgFile->GetFullPath();
	}

	std::pair<bool, std::wstring> AttemptToFindAlbumArt(FileSystem::IFile& file, const std::wstring& albumName)
	{
		using namespace concurrency;

		std::pair<bool, std::wstring> result = { false, L"" };
		std::shared_ptr<FileSystem::IFile> albumArtFile = nullptr;

		auto parentFolder = file.GetParent();
		auto files = parentFolder->GetFiles();
		const std::ctype<wchar_t>& f = std::use_facet< std::ctype<wchar_t> >(std::locale());
		decltype(files) possibleAlbumArtFiles;
		for (auto& file : files)
		{
			auto ext = file->GetExtension();
			f.tolower(&ext[0], &ext[0] + ext.size());

			auto itr = POSSIBLE_IMAGE_FILE_EXTENSIONS.find(ext);
			if (itr != std::end(POSSIBLE_IMAGE_FILE_EXTENSIONS))
			{
				possibleAlbumArtFiles.push_back(file);
			}
		}

		// now we need to select one
		if (possibleAlbumArtFiles.size() == 1)
		{
			// there is only one, lets go with that!
			albumArtFile = possibleAlbumArtFiles[0];
		}
		else if (possibleAlbumArtFiles.size() > 1)
		{
			// if any pass at least one criteria select that one
			// first criteria is select any that contain the album name
			// second criteria is any that contain any of the following: 'cover', 'art', 'album'
			// finally if none pass select a random one
			
			std::wstring albumNameLower = albumName;
			f.tolower(&albumNameLower[0], &albumNameLower[0] + albumNameLower.size());
			for (auto& file : possibleAlbumArtFiles)
			{
				auto name = file->GetFullName();
				f.tolower(&name[0], &name[0] + name.size());
				if (name.find(albumNameLower) != std::basic_string<wchar_t>::npos)
				{
					albumArtFile = file;
					break;
				}
			}

			if (nullptr == albumArtFile)
			{
				// still haven't found it, try second criteria
				const std::unordered_set<std::wstring> searchCriteria = { L"cover", L"art", L"album" };
				for (auto& file : possibleAlbumArtFiles)
				{
					auto name = file->GetFullName();
					f.tolower(&name[0], &name[0] + name.size());
					for (auto& searchTerm : searchCriteria)
					{
						if (name.find(searchTerm) != std::basic_string<wchar_t>::npos)
						{
							albumArtFile = file;
							break;
						}
					}
				}

				if (nullptr == albumArtFile)
				{
					// no dice, still haven't found a match. Pick one at random
					albumArtFile = possibleAlbumArtFiles[rand() % possibleAlbumArtFiles.size()];
				}
			}
		}

		if (nullptr != albumArtFile)
		{
			// For some reason we can't load the image in its default location, try to move to local folder first
			std::vector<unsigned char> buffer;
			albumArtFile->ReadFromFile(buffer);
			auto ext = albumArtFile->GetExtension();
			f.tolower(&ext[0], &ext[0] + ext.size());

			auto itr = MPEG4::WStringToImageType.find(ext);
			MPEG4::ImageType imageType = MPEG4::ImageType::UNKNOWN;
			if (itr != std::end(MPEG4::WStringToImageType))
			{
				imageType = itr->second;
			}

			auto path = SaveImageFile(buffer, albumName, imageType);
			result = { true, path };
		}
		
		return result;
	}

}
}