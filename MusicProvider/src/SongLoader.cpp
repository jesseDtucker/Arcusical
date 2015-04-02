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

using namespace std;
using namespace Arcusical;
using namespace Arcusical::MusicProvider;

static Model::Song LoadMpeg4Song(FileSystem::IFile& file);
static Model::Song DefaultSongLoad(FileSystem::IFile& file, Model::AudioFormat encoding = Model::AudioFormat::UNKNOWN, Model::ContainerType container = Model::ContainerType::UNKNOWN);
static Model::Song LoadMP3(FileSystem::IFile& file);
static Model::Song LoadWav(FileSystem::IFile& file);

static pair<bool, wstring> DefaultLoadAlbumImage(FileSystem::IFile& file, const wstring& albumName);
static pair<bool, wstring> LoadAlbumImageFromMpeg4(FileSystem::IFile& file, const wstring& albumName);
static pair<bool, wstring> AttemptToFindAlbumArt(FileSystem::IFile& file, const wstring& albumName);
static pair<bool, wstring> AttemptToFindAlbumArt(FileSystem::IFolder& folder, const wstring& albumName);
static wstring SaveImageFile(vector<unsigned char>& imgData, const wstring& albumName, MPEG4::ImageType imageType);

static boost::uuids::random_generator s_idGenerator;

static const unordered_map<MPEG4::Encoding, Model::AudioFormat> MPEG4_TO_MODEL_MAPPING =
{
	{ MPEG4::Encoding::AAC, Model::AudioFormat::AAC },
	{ MPEG4::Encoding::ALAC, Model::AudioFormat::ALAC },
	{ MPEG4::Encoding::UNKNOWN, Model::AudioFormat::UNKNOWN }
};

static const unordered_map<wstring, function<Model::Song(FileSystem::IFile&)>> FILE_EX_TO_LOADER =
{
	{ L"m4a", LoadMpeg4Song },
	{ L"mp3", LoadMP3 },
	{ L"wav", LoadWav }
};

static const unordered_map<Model::ContainerType, function<pair<bool, wstring>(FileSystem::IFile& file, const wstring& albumName)>> FILE_EX_TO_IMAGE_LOADER =
{
	{ Model::ContainerType::MP4, LoadAlbumImageFromMpeg4 }
};

static const unordered_set<wstring> POSSIBLE_IMAGE_FILE_EXTENSIONS = { L"png", L"bmp", L"jpg", L"jpeg" };

Model::Song MusicProvider::LoadSong(FileSystem::IFile& file)
{
	Model::Song result;
	auto fileExtension = file.GetExtension();

	auto fileName = file.GetFullName();

	auto loadFunc = FILE_EX_TO_LOADER.find(fileExtension);
	if (loadFunc != end(FILE_EX_TO_LOADER))
	{
		result = loadFunc->second(file);
	}
	else
	{
		result = DefaultSongLoad(file);
	}

	if (result.GetTitle().length() == 0)
	{
		auto fileName = file.GetName();
		replace(begin(fileName), end(fileName), '_', ' ');
		result.SetTitle(fileName);
	}

	return result;
}

Model::Album MusicProvider::CreateAlbum(wstring& name, Model::Song& song, shared_ptr<SongIdMapper>& mapper)
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
	wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;

	result.SetArtist(converter.from_bytes(mpegSong->GetArtist()));
	result.SetId(s_idGenerator()); // generate a random id, the odds of collision are slim to none
	result.SetLength(mpegSong->GetLength());
	result.SetTitle(converter.from_bytes(mpegSong->GetTitle()));
	result.SetAlbumName(converter.from_bytes(mpegSong->GetAlbum()));

	auto trackNum = mpegSong->GetTrackNumber();
	auto diskNum = mpegSong->GetDiskNumber();
	result.SetTrackNumber({ trackNum.first, trackNum.second });
	result.SetDiskNumber({ diskNum.first, diskNum.second });

	Model::AudioFormat songFormat = Model::AudioFormat::UNKNOWN;
	auto encodingItr = MPEG4_TO_MODEL_MAPPING.find(mpegSong->GetEncoding());
	ARC_ASSERT(encodingItr != end(MPEG4_TO_MODEL_MAPPING));
	if (encodingItr != end(MPEG4_TO_MODEL_MAPPING))
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

	stringstream stream;
	mpegSong->GetTree()->PrintTree(stream);

	while (!stream.eof())
	{
		string output;
		getline(stream, output);
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
		
	result.SetArtist(wstring(musicProperties->Artist->Data()));
	result.SetId(s_idGenerator()); // generate a random id, the odds of collision are slim to none
	result.SetLength(musicProperties->Duration.Duration / 1000); // Duration is provided is ms, but we need it in seconds
	result.SetTitle(wstring(musicProperties->Title->Data()));
	result.SetAlbumName(wstring(musicProperties->Album->Data()));
	result.SetTrackNumber({ musicProperties->TrackNumber, musicProperties->TrackNumber }); // TODO::JT need to get the max tracks in album set correctly... fixup album?

	Model::SongFile songFile;
	songFile.filePath = file.GetFullPath();
	songFile.bitRate = musicProperties->Bitrate;
	songFile.channelCount = 0; // no idea how to determine this without parsing...
	songFile.sampleSize = 0; // no idea how to determine this without parsing...
	songFile.format = encoding;
	songFile.container = container;
	result.AddFile(songFile);

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

wstring MusicProvider::LoadAlbumImage(Model::Song& song, const wstring& albumName, bool checkFileSystem)
{
	// for each song attempt to load the image, if successful stop
	// then return the path of the new album image
		
	auto files = song.GetFiles();
	wstring filePath = L"";
	bool isFound = false;

	for (auto& audioFile : files)
	{
		auto imageLoadFunc = FILE_EX_TO_IMAGE_LOADER.find(audioFile.second.container);
		if (imageLoadFunc != end(FILE_EX_TO_IMAGE_LOADER))
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

pair<bool, wstring> LoadAlbumImageFromMpeg4(FileSystem::IFile& file, const wstring& albumName)
{
	using namespace FileSystem;

	pair<bool, wstring> result = { false, L"" };

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

pair<bool, wstring> DefaultLoadAlbumImage(FileSystem::IFile& file, const wstring& albumName)
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

wstring SaveImageFile(vector<unsigned char>& imgData, const wstring& albumName, MPEG4::ImageType imageType)
{
	using namespace FileSystem;

	auto fileName = albumName + L"." + MPEG4::ImageTypeToWString.at(imageType);
	Storage::RemoveIllegalCharacters(fileName);
	replace(begin(fileName), end(fileName), '\\', '_');
	replace(begin(fileName), end(fileName), '/', '_');
	fileName = L"albumImgs\\" + fileName;

	ARC_ASSERT_MSG(!Storage::ApplicationFolder().ContainsFile(fileName), "an album image with this name already exists!");
	auto imgFile = Storage::ApplicationFolder().CreateNewFile(fileName);

	if (nullptr != imgFile)
	{
		imgFile->WriteToFile(imgData);
		return imgFile->GetFullPath();
	}
	else
	{
		return L"";
	}
}

pair<bool, wstring> AttemptToFindAlbumArt(FileSystem::IFile& file, const wstring& albumName)
{
	return AttemptToFindAlbumArt(*file.GetParent(), albumName);
}

// TODO::JT refactor this ugliness... it's getting hard to read
// TODO::JT searching directories should happen after the fact, not during song load
//	        Move this functionality elsewhere
pair<bool, wstring> AttemptToFindAlbumArt(FileSystem::IFolder& folder, const wstring& albumName)
{
	pair<bool, wstring> result = { false, L"" };
	shared_ptr<FileSystem::IFile> albumArtFile = nullptr;

	auto files = folder.GetFiles();
	const ctype<wchar_t>& f = use_facet< ctype<wchar_t> >(locale());
	decltype(files) possibleAlbumArtFiles;
	for (auto& file : files)
	{
		auto ext = file->GetExtension();
		f.tolower(&ext[0], &ext[0] + ext.size());

		auto itr = POSSIBLE_IMAGE_FILE_EXTENSIONS.find(ext);
		if (itr != end(POSSIBLE_IMAGE_FILE_EXTENSIONS))
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

		wstring albumNameLower = albumName;
		f.tolower(&albumNameLower[0], &albumNameLower[0] + albumNameLower.size());
		for (auto& file : possibleAlbumArtFiles)
		{
			auto name = file->GetFullName();
			f.tolower(&name[0], &name[0] + name.size());
			if (name.find(albumNameLower) != basic_string<wchar_t>::npos)
			{
				albumArtFile = file;
				break;
			}
		}

		if (nullptr == albumArtFile)
		{
			// still haven't found it, try second criteria
			const vector<wstring> searchCriteria = { L"album_front", L"album," L"cover", L"art"};
			for (auto& file : possibleAlbumArtFiles)
			{
				auto name = file->GetFullName();
				f.tolower(&name[0], &name[0] + name.size());
				for (auto& searchTerm : searchCriteria)
				{
					if (name.find(searchTerm) != basic_string<wchar_t>::npos)
					{
						albumArtFile = file;
						break;
					}
				}

				if (albumArtFile != nullptr)
				{
					break;
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
		vector<unsigned char> buffer;
		albumArtFile->ReadFromFile(buffer);
		auto ext = albumArtFile->GetExtension();
		f.tolower(&ext[0], &ext[0] + ext.size());

		auto itr = MPEG4::WStringToImageType.find(ext);
		MPEG4::ImageType imageType = MPEG4::ImageType::UNKNOWN;
		if (itr != end(MPEG4::WStringToImageType))
		{
			imageType = itr->second;
		}

		auto path = SaveImageFile(buffer, albumName, imageType);
		result = { true, path };
	}
	else
	{
		// we couldn't find it, try stepping into any sub-folders that have artwork or cover in the name
		// TODO::JT this is badly broken....
		auto folders = folder.GetSubfolders();
		for (auto& folder : folders)
		{
			auto folderName = folder->GetName();
			f.tolower(&folderName[0], &folderName[0] + folderName.size());
			if (folderName.find(L"artwork") != wstring::npos
				|| folderName.find(L"art") != wstring::npos
				|| folderName.find(L"cover") != wstring::npos)
			{
				// TODO::JT so broken, can recurse infinitely... need to not back up while going down.
				//result = AttemptToFindAlbumArt(*folder, albumName);
				if (result.first)
				{
					break;
				}
			}
		}

		if (!result.first)
		{
			// try again on the parent folder if the parent folder is not the root music folder
			auto parent = folder.GetParent();
			if (parent != nullptr)
			{
				result = AttemptToFindAlbumArt(*parent, albumName);
			}
		}
	}

	return result;
}