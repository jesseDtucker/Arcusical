#include "pch.h"

#define PARSER_VERSOBE 0

#if PARSER_VERSOBE
#include <sstream>
#include <debugapi.h>
#endif

#include <codecvt>
#include <string>
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
	static Model::Song LoadMP3(FileSystem::IFile& file);
	static Model::Song LoadWav(FileSystem::IFile& file);

	static std::wstring LoadAlbumImage(Model::Song& song, const std::wstring& albumName);
	static std::pair<bool, std::wstring> LoadAlbumImageFromMpeg4(FileSystem::IFile& file, const std::wstring& albumName);
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
			ARC_FAIL("Attempted to load an unsupported format!");
			// TODO::JT need to return something reasonable or throw
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

		newAlbum.SetImageFilePath(LoadAlbumImage(song, name));

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

	Model::Song LoadMP3(FileSystem::IFile& file)
	{
		auto result = Model::Song();

		result.SetArtist(L"Some Artist");
		result.SetId(s_idGenerator()); // generate a random id, the odds of collision are slim to none
		result.SetLength(100);
		result.SetTitle(L"Some MP3");

		Model::AudioFormat songFormat = Model::AudioFormat::MP3;

		Model::SongFile songFile;
		songFile.filePath = file.GetFullPath();
		songFile.bitRate = 0;
		songFile.channelCount = 0;
		songFile.sampleSize = 0;
		songFile.format = songFormat;
		songFile.container = Model::ContainerType::MP3;
		result.AddFile(songFile);

		return result;
	}

	Model::Song LoadWav(FileSystem::IFile& file)
	{
		auto result = Model::Song();

		result.SetArtist(L"Some Artist");
		result.SetId(s_idGenerator()); // generate a random id, the odds of collision are slim to none
		result.SetLength(100);
		result.SetTitle(L"Some WAV");

		Model::SongFile songFile;
		songFile.filePath = file.GetFullPath();
		songFile.bitRate = 0;
		songFile.channelCount = 0;
		songFile.sampleSize = 0;
		songFile.format = Model::AudioFormat::WAV;
		songFile.container = Model::ContainerType::WAV;
		result.AddFile(songFile);

		return result;
	}

	std::wstring LoadAlbumImage(Model::Song& song, const std::wstring& albumName)
	{
		// for each song attempt to load the image, if successful stop
		// then return the path of the new album image
		
		auto files = song.GetFiles();
		std::wstring filePath = L"";

		for (auto& audioFile : files)
		{
			auto imageLoadFunc = FILE_EX_TO_IMAGE_LOADER.find(audioFile.second.container);
			if (imageLoadFunc != std::end(FILE_EX_TO_IMAGE_LOADER))
			{
				auto storageFile = FileSystem::Storage::LoadFileFromPath(audioFile.second.filePath);
				auto result = imageLoadFunc->second(*storageFile, albumName);
				if (result.first)
				{
					filePath = result.second;
					break;
				}
			}
		}

		if (filePath.length() == 0)
		{
			filePath = L"\\Assets\\albumBackground.png";
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

}
}