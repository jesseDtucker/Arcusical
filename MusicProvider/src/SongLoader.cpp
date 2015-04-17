#define PARSER_VERSOBE 0

#if PARSER_VERSOBE
#include <sstream>
#include <debugapi.h>
#endif

#include <algorithm>
#include "boost/algorithm/string/predicate.hpp"
#include "boost/functional/hash.hpp"
#include <cctype>
#include <codecvt>
#include <functional>
#include <ppltasks.h>
#include <random>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "Album.hpp"
#include "Arc_Assert.hpp"
#include "IAlbumToSongMapper.hpp"
#include "IFileReader.hpp"
#include "IFile.hpp"
#include "IFolder.hpp"
#include "ImageTypes.hpp"
#include "MPEG4_Parser.hpp"
#include "Song.hpp"
#include "SongLoader.hpp"
#include "Storage.hpp"

using namespace Arcusical;
using namespace Arcusical::Model;
using namespace Arcusical::MusicProvider;
using namespace boost::uuids;
using namespace FileSystem;
using namespace std;

struct ihash;
struct iequal_to;

typedef unordered_map<wstring, vector<const Song*>, ihash, iequal_to> SongLookupByAlbumName;
typedef unordered_map<wstring, const Album*, ihash, iequal_to> AlbumLookupByAlbumName;

Album CreateAlbum(const wstring& name, const vector<const Song*>& songs, const IAlbumToSongMapper* mapper);
wstring LoadAlbumImage(const vector<const Song*>& songs, const wstring& albumName, bool checkFileSystem);
Song LoadSong(const IFile& file);

static Song LoadMpeg4Song(const IFile& file);
static Song DefaultSongLoad(const IFile& file, AudioFormat encoding = AudioFormat::UNKNOWN, ContainerType container = ContainerType::UNKNOWN);
static Song LoadMP3(const IFile& file);
static Song LoadWav(const IFile& file);

static pair<bool, wstring> DefaultLoadAlbumImage(const IFile& file, const wstring& albumName);
static pair<bool, wstring> LoadAlbumImageFromMpeg4(const IFile& file, const wstring& albumName);
static pair<bool, wstring> AttemptToFindAlbumArt(const IFile& file, const wstring& albumName);
static pair<bool, wstring> AttemptToFindAlbumArt(const IFolder& folder, const wstring& albumName);
static wstring SaveImageFile(vector<unsigned char>& imgData, const wstring& albumName, MPEG4::ImageType imageType);

static boost::uuids::random_generator s_idGenerator;

static const unordered_map<MPEG4::Encoding, AudioFormat> MPEG4_TO_MODEL_MAPPING =
{
	{ MPEG4::Encoding::AAC, AudioFormat::AAC },
	{ MPEG4::Encoding::ALAC, AudioFormat::ALAC },
	{ MPEG4::Encoding::UNKNOWN, AudioFormat::UNKNOWN }
};

static const unordered_map<wstring, function<Song(const IFile&)>> FILE_EX_TO_LOADER =
{
	{ L"m4a", LoadMpeg4Song },
	{ L"mp3", LoadMP3 },
	{ L"wav", LoadWav }
};

static const unordered_map<ContainerType, function<pair<bool, wstring>(IFile& file, const wstring& albumName)>> FILE_EX_TO_IMAGE_LOADER =
{
	{ ContainerType::MP4, LoadAlbumImageFromMpeg4 }
};

static const unordered_set<wstring> POSSIBLE_IMAGE_FILE_EXTENSIONS = { L"png", L"bmp", L"jpg", L"jpeg" };

Song LoadSong(const IFile& file)
{
	Song result;
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

// searches through the songs and picks the most common artist
wstring SelectMostCommonAlbumArtist(const vector<const Song*>& songs)
{
	ARC_ASSERT(songs.size() > 0);

	typedef unordered_map<wstring, int> countMap;
	countMap counter;
	for (auto& song : songs)
	{
		auto& artist = song->GetArtist();
		if (counter.find(artist) == end(counter))
		{
			counter[artist] = 1;
		}
		else
		{
			++counter[artist];
		}
	}

	auto m = max_element(begin(counter), end(counter), [](const countMap::value_type&a, const countMap::value_type& b)
	{
		return a.second < b.second;
	});

	return m->first;
}

Album CreateAlbum(const wstring& name, const vector<const Song*>& songs, const IAlbumToSongMapper* mapper)
{
	using namespace boost::uuids;

	ARC_ASSERT(songs.size() > 0);

	auto newAlbum = Album(mapper);

	vector<uuid> songIds;
	transform(begin(songs), end(songs), back_inserter(songIds), [](const Model::Song* song)
	{
		return song->GetId();
	});

	newAlbum.SetId(s_idGenerator());
	newAlbum.SetArtist(SelectMostCommonAlbumArtist(songs));
	newAlbum.GetMutableSongIds().insert(begin(songIds), end(songIds));
	newAlbum.SetTitle(name);

	newAlbum.SetImageFilePath(LoadAlbumImage(songs, name, true));

	return newAlbum;
}

Song LoadMpeg4Song(const IFile& file)
{
	MPEG4::MPEG4_Parser mpegParser;

	auto fileReader = Storage::GetReader(&file);
	auto mpegSong = mpegParser.ReadAndParseFromStream(*fileReader);

	auto result = Song();

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

	AudioFormat songFormat = AudioFormat::UNKNOWN;
	auto encodingItr = MPEG4_TO_MODEL_MAPPING.find(mpegSong->GetEncoding());
	ARC_ASSERT(encodingItr != end(MPEG4_TO_MODEL_MAPPING));
	if (encodingItr != end(MPEG4_TO_MODEL_MAPPING))
	{
		songFormat = encodingItr->second;
	}

	SongFile songFile;
	songFile.filePath = file.GetFullPath();
	songFile.bitRate = mpegSong->GetSampleSize() * mpegSong->GetSampleRate();
	songFile.channelCount = mpegSong->GetNumberOfChannels();
	songFile.sampleSize = mpegSong->GetNumberOfChannels();
	songFile.format = songFormat;
	songFile.container = ContainerType::MP4;
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

Song DefaultSongLoad(const IFile& file, AudioFormat encoding, ContainerType container)
{
#ifdef __cplusplus_winrt
	using namespace concurrency;

	auto result = Song();
	auto musicProperties = create_task(file.GetRawHandle()->Properties->GetMusicPropertiesAsync()).get();

	result.SetArtist(wstring(musicProperties->Artist->Data()));
	result.SetId(s_idGenerator()); // generate a random id, the odds of collision are slim to none
	result.SetLength(musicProperties->Duration.Duration / 10000000); // Duration is provided is 100 ns units, but we need it in seconds
	result.SetTitle(wstring(musicProperties->Title->Data()));
	result.SetAlbumName(wstring(musicProperties->Album->Data()));
	result.SetTrackNumber({ musicProperties->TrackNumber, musicProperties->TrackNumber }); // TODO::JT need to get the max tracks in album set correctly... fixup album?

	SongFile songFile;
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

Song LoadMP3(const IFile& file)
{
	return DefaultSongLoad(file, AudioFormat::MP3, ContainerType::MP3);
}

Song LoadWav(const IFile& file)
{
	return DefaultSongLoad(file, AudioFormat::WAV, ContainerType::WAV);
}

wstring LoadAlbumImage(const vector<const Song*>& songs, const wstring& albumName, bool checkFileSystem)
{
	// for each song attempt to load the image, if successful stop
	// then return the path of the new album image
	
	vector<SongFile> files;
	files.reserve(songs.size()); // each song may have multiple files, but this will at least be a reasonable first guess
	for (auto& song : songs)
	{
		auto& songFileFormats = song->GetFiles();
		for (auto& songFiles : songFileFormats)
		{
			copy(begin(songFiles.second), end(songFiles.second), back_inserter(files));
		}
	}

	wstring filePath = L"";
	bool isFound = false;

	for (auto& audioFile : files)
	{
		auto imageLoadFunc = FILE_EX_TO_IMAGE_LOADER.find(audioFile.container);
		if (imageLoadFunc != end(FILE_EX_TO_IMAGE_LOADER))
		{
			auto storageFile = Storage::LoadFileFromPath(audioFile.filePath);
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
			auto storageFile = Storage::LoadFileFromPath(audioFile.filePath);
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
				auto storageFile = Storage::LoadFileFromPath(audioFile.filePath);
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

pair<bool, wstring> LoadAlbumImageFromMpeg4(const IFile& file, const wstring& albumName)
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

pair<bool, wstring> DefaultLoadAlbumImage(const IFile& file, const wstring& albumName)
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

pair<bool, wstring> AttemptToFindAlbumArt(const IFile& file, const wstring& albumName)
{
	return AttemptToFindAlbumArt(*file.GetParent(), albumName);
}

// TODO::JT refactor this ugliness... it's getting hard to read
// TODO::JT searching directories should happen after the fact, not during song load
//	        Move this functionality elsewhere
pair<bool, wstring> AttemptToFindAlbumArt(const IFolder& folder, const wstring& albumName)
{
	pair<bool, wstring> result = { false, L"" };
	shared_ptr<IFile> albumArtFile = nullptr;

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

vector<IFile*> GetNewFiles(const SongCollection& existingSongs,
						   const vector<shared_ptr<IFile>>& files)
{
	unordered_set<wstring> existingSongFiles;
	for (auto& song : existingSongs)
	{
		for (const auto& songFile : song.second.GetFiles())
		{
			auto& files = songFile.second;
			transform(begin(files), end(files), inserter(existingSongFiles, begin(existingSongFiles)), [](const SongFile& file)
			{
				return file.filePath;
			});
		}
	}

	vector<IFile*> filesRaw;
	filesRaw.resize(files.size());
	auto endItr = transform(begin(files), end(files), begin(filesRaw), [](const shared_ptr<IFile>& file)
	{
		return file.get();
	});
	ARC_ASSERT(endItr == end(filesRaw));

	vector<IFile*> result;
	result.resize(files.size());
	auto lastItr = copy_if(begin(filesRaw), end(filesRaw), begin(result), [&existingSongFiles](const IFile* file)
	{
		return existingSongFiles.find(file->GetFullPath()) == end(existingSongFiles);
	});
	result.resize(lastItr - begin(result));
	return result;
}

vector<Song> LoadSongs(vector<IFile*> files)
{
	vector<Song> results;
	results.resize(files.size());
	auto endItr = transform(begin(files), end(files), begin(results), [](const IFile* file)
	{
		return LoadSong(*file);
	});
	ARC_ASSERT(endItr == end(results));
	return results;
}

struct ExistingSongPair
{
	const Song* original;
	const Song* newSong;
};

struct DivideSongsResults
{
	vector<const Song*> newSongs;
	vector<ExistingSongPair> newFormatsOfExisting;
};

// first is the new songs, second is the modified songs
DivideSongsResults DivideSongs(const SongCollection& existingSongs,
							   const vector<Song>& loadedSongs)
{
	DivideSongsResults result;

	for (auto& song : loadedSongs)
	{
		auto searchItr = find_if(begin(existingSongs), end(existingSongs), [&song](const SongCollection::value_type& songPair )
		{
			return song.IsSameSong(songPair.second);
		});

		if (searchItr == end(existingSongs))
		{
			// did not find a matching song
			result.newSongs.push_back(&song);
		}
		else
		{
			result.newFormatsOfExisting.push_back({ &searchItr->second, &song });
		}
	}

	return result;
}

// join the songs into one
Song CombineSongs(const Song* original, const vector<const Song*>& songs)
{
	// merge all other songs into the original
	Song result = *original;
	for (auto nextSong : songs)
	{
		for (auto& files : nextSong->GetFiles())
		{
			for (auto& file : files.second)
			{
				result.AddFile(file);
			}
		}
	}

	return result;
}
Song CombineSongs(const vector<const Song*>& songs)
{
	ARC_ASSERT(songs.size() > 0);
	vector<const Song*> input;
	input.reserve(songs.size());
	copy(begin(songs) + 1, end(songs), back_inserter(input));
	return CombineSongs(songs[0], input);
}

vector<Song> FlattenNewSongs(const vector<const Song*>& newSongPtrs)
{
	// start by grouping the songs that are the same
	unordered_set<const Song*> alreadyGrouped;
	vector<vector<const Song*>> groups;

	for (auto baseSong : newSongPtrs)
	{
		// ignore those that have already been grouped
		if (alreadyGrouped.find(baseSong) == end(alreadyGrouped))
		{
			vector<const Song*> group;

			group.push_back(baseSong);
			alreadyGrouped.insert(baseSong);

			// now select all that are the same and not this one and group them
			for (auto song : newSongPtrs)
			{
				if (song != baseSong && // is not current song
					alreadyGrouped.find(song) == end(alreadyGrouped) && // is not already in a group
					song->IsSameSong(*baseSong)) // is refereeing to the same song as the base
				{
					// then group them!
					group.push_back(song);
					alreadyGrouped.insert(song);
				}
			}

			groups.push_back(group);
		}
	}

	vector<Song> newSongs;
	newSongs.resize(groups.size());

	auto endItr = transform(begin(groups), end(groups), begin(newSongs), [](const vector<const Song*> group)
	{
		return CombineSongs(group);
	});
	ARC_ASSERT(endItr == end(newSongs));

	return move(newSongs);
}

vector<Song> FlattenModifiedSongs(const vector<ExistingSongPair>& modifiedSongs)
{
	// first group the songs such that they are all together
	typedef unordered_map<const Song*, vector<const Song*>> SongMap;
	SongMap songMap;
	for (auto& pair : modifiedSongs)
	{
		auto itr = songMap.find(pair.original);
		if (itr == end(songMap))
		{
			songMap[pair.original] = { pair.newSong };
		}
		else
		{
			songMap[pair.original].push_back(pair.newSong);
		}
	}

	vector<Song> results;
	results.reserve(songMap.size());

	transform(begin(songMap), end(songMap), back_inserter(results), [](const SongMap::value_type& pair)
	{
		return CombineSongs(pair.first, pair.second);
	});

	return results;
}

void FixupAlbumName(Song& song, const vector<Song>& newSongs, const SongCollection& existingSongs)
{
	// then we haven't determined the name of this song.
	// first try to find matching songs in the existing songs
	auto existingMatchItr = find_if(begin(existingSongs), end(existingSongs), [&song](SongCollection::value_type other)
	{
		return other.second.IsSameSong(song);
	});
	if (existingMatchItr != end(existingSongs))
	{
		// one of the existing songs appears to be the same song!
		song.SetAlbumName(existingMatchItr->second.GetAlbumName());
	}

	if (song.GetAlbumName().size() == 0)
	{
		// then we have not yet located the song, try again with the new songs
		auto newMatchItr = find_if(begin(newSongs), end(newSongs), [&song](const Song& other)
		{
			return other.IsSameSong(song);
		});
		if (newMatchItr != end(newSongs))
		{
			song.SetAlbumName(newMatchItr->GetAlbumName());
		}
	}

	if (song.GetAlbumName().size() == 0)
	{
		// if we did not find any matching songs then we will instead try to locate
		// the album name based on the folder structure

		// firstly we'll take each file and determine a common base directory
		wstring basePath = L"";
		for (auto& audioFileList : song.GetFiles())
		{
			for (auto& audioFile : audioFileList.second)
			{
				auto path = audioFile.filePath;
				if (basePath.size() == 0)
				{
					basePath = path;
				}
				else
				{
					auto length = min(basePath.size(), path.size());
					for (unsigned int i = 0; i < length; ++i)
					{
						if (basePath[i] != path[i])
						{
							// cut the string at this point
							basePath = basePath.substr(0, i);
							break;
						}
					}
				}
			}
		}

		// now assume the common base folder is the album name
		wstring albumName = L"";
		set<wstring> invalidAlbumNames = { L"mp3", L"wav", L"alac", L"flac" };
		const ctype<wchar_t>& f = use_facet< ctype<wchar_t> >(locale());
		auto isValidAlbumName = false;
		do
		{
			auto cutPoint = basePath.find_last_of('\\');
			if (FileSystem::Storage::IsFolder(basePath))
			{
				albumName = basePath.substr(cutPoint + 1, basePath.size() - cutPoint);
				wstring albumNameLower = albumName;
				f.tolower(&albumNameLower[0], &albumNameLower[0] + albumNameLower.size());
				isValidAlbumName = invalidAlbumNames.find(albumNameLower) == end(invalidAlbumNames);
			}
			basePath = basePath.substr(0, cutPoint);
		} while ((!isValidAlbumName || albumName.size() == 0) && basePath.find_last_of('\\') != basePath.find_first_of('\\'));

		song.SetAlbumName(albumName);
	}

	ARC_ASSERT_MSG(song.GetAlbumName().size() != 0, "Failed to fixup album name for song!");
}

void FixupSongs(vector<Song>& newSongs, const SongCollection& existingSongs)
{
	for (auto& song : newSongs)
	{
		if (song.GetAlbumName().size() == 0)
		{
			FixupAlbumName(song, newSongs, existingSongs);
		}
	}
}

SongMergeResult MusicProvider::MergeSongs(const SongCollection& existingSongs, 
									const vector<shared_ptr<IFile>>& files)
{
	SongMergeResult result;

	auto newFiles = GetNewFiles(existingSongs, files);
	auto songs = LoadSongs(newFiles);
	auto dividedSongs = DivideSongs(existingSongs, songs); // sort the new songs into completely new songs and songs that match existing songs
	result.newSongs = FlattenNewSongs(dividedSongs.newSongs);
	result.modifiedSongs = FlattenModifiedSongs(dividedSongs.newFormatsOfExisting);

	FixupSongs(result.newSongs, existingSongs);

	return result;
}

vector<const Song*> SongsWithoutAlbums(const AlbumCollection& existingAlbums, const SongCollection& songs)
{
	vector<uuid> allNewSongIds; // all of the ids for all songs
	vector<uuid> allAlbumSongIds; // all song ids that are referenced by an album

	allNewSongIds.reserve(songs.size());
	allAlbumSongIds.reserve(songs.size());

	for (auto& song : songs)
	{
		allNewSongIds.push_back(song.first);
	}
	for (auto& album : existingAlbums)
	{
		auto& ids = album.second.GetSongIds();
		allAlbumSongIds.insert(end(allAlbumSongIds), begin(ids), end(ids));
	}

	sort(begin(allNewSongIds), end(allNewSongIds));
	sort(begin(allAlbumSongIds), end(allAlbumSongIds));

	// now take the difference of the two sets, whatever is left are songs that are not referenced in an album
	vector<uuid> songIdsWithoutAlbums;

	set_difference(begin(allNewSongIds),
		end(allNewSongIds),
		begin(allAlbumSongIds),
		end(allAlbumSongIds),
		back_inserter(songIdsWithoutAlbums));

	vector<const Song*> results;
	results.reserve(songIdsWithoutAlbums.size());

	transform(begin(songIdsWithoutAlbums), end(songIdsWithoutAlbums), back_inserter(results), [&songs](const uuid& id)
	{
		return &songs.at(id);
	});

	return results;
}

// used to make album comparison case insensitive while in the lookup map
struct iequal_to
{
	bool operator()(const wstring& a, const wstring& b) const
	{
		return boost::algorithm::iequals(a, b, std::locale());
	}
};

struct ihash
{
	size_t operator()(wstring const& a) const
	{
		size_t seed = 0;
		locale locale;

		for (auto& c : a)
		{
			boost::hash_combine(seed, std::toupper(c, locale));
		}

		return seed;
	}
};

SongLookupByAlbumName GroupSongsByAlbumTitle(vector<const Song*> songs)
{
	SongLookupByAlbumName result;

	for (auto song : songs)
	{
		result[song->GetAlbumName()].push_back(song);
	}

	return result;
}

AlbumLookupByAlbumName CreateAlbumLookup(const AlbumCollection& albums)
{
	AlbumLookupByAlbumName result;
	result.reserve(albums.size());
	
	transform(begin(albums), end(albums), inserter(result, end(result)), [](const AlbumCollection::value_type& album) -> pair<wstring, const Album*>
	{
		return { album.second.GetTitle(), &album.second };
	});

	return result;
}

vector<Album> CreateNewAlbums(AlbumLookupByAlbumName albums, SongLookupByAlbumName songGroups, const IAlbumToSongMapper* mapper)
{
	// any groups in songs that are not in albums are new albums
	vector<Album> result;

	for (auto& songGroup : songGroups)
	{
		// make sure this isn't an existing album
		if (albums.find(songGroup.first) == end(albums))
		{
			result.push_back(CreateAlbum(songGroup.first, songGroup.second, mapper));
		}
	}

	return result;
}

vector<Album> CreateModifiedAlbums(AlbumLookupByAlbumName albums, SongLookupByAlbumName songGroups)
{
	vector<Album> result;

	for (auto& songGroup : songGroups)
	{
		// make sure this isn't an existing album
		auto albumItr = albums.find(songGroup.first);
		if (albumItr != end(albums))
		{
			Album existingAlbum = *albumItr->second;
			vector<uuid> songIds;

			transform(begin(songGroup.second), end(songGroup.second), back_inserter(songIds), [](const Song* song)
			{
				return song->GetId();
			});

			existingAlbum.GetMutableSongIds().insert(begin(songIds), end(songIds));
			result.push_back(existingAlbum);
		}
	}

	return result;
}

void FixupAlbumImages(vector<Album>& newAlbums)
{
	static const vector<wstring> DEFAULT_IMAGES =
	{
		L"ms-appx:///Assets/DefaultAlbumBackgrounds/default_black.png",
		L"ms-appx:///Assets/DefaultAlbumBackgrounds/default_cyan.png",
		L"ms-appx:///Assets/DefaultAlbumBackgrounds/default_dark_blue.png",
		L"ms-appx:///Assets/DefaultAlbumBackgrounds/default_dark_red.png",
		L"ms-appx:///Assets/DefaultAlbumBackgrounds/default_fuchsia.png",
		L"ms-appx:///Assets/DefaultAlbumBackgrounds/default_green.png",
		L"ms-appx:///Assets/DefaultAlbumBackgrounds/default_light_cyan.png",
		L"ms-appx:///Assets/DefaultAlbumBackgrounds/default_light_green.png",
		L"ms-appx:///Assets/DefaultAlbumBackgrounds/default_lime.png",
		L"ms-appx:///Assets/DefaultAlbumBackgrounds/default_orange.png",
		L"ms-appx:///Assets/DefaultAlbumBackgrounds/default_red.png",
		L"ms-appx:///Assets/DefaultAlbumBackgrounds/default_violet.png"
	};

	random_device rd;
	auto bag = DEFAULT_IMAGES;
	for (auto& album : newAlbums)
	{
		if (album.GetImageFilePath().size() == 0)
		{
			// then we pull one out of the bag
			// images are selected using a Tetris algorithm
			// the images are selected until none are available then
			// the bag is refilled and selection continues. This
			// guarantees a uniform selection
			if (bag.size() == 0) // make sure to refill the bag when it gets empty
			{
				bag = DEFAULT_IMAGES;
				shuffle(begin(bag), end(bag), default_random_engine(rd()));
			}

			album.SetImageFilePath(bag.back());
			bag.pop_back();
		}
	}
}

AlbumMergeResult MusicProvider::MergeAlbums(const AlbumCollection& existingAlbums, const SongCollection& songs, const Model::IAlbumToSongMapper* mapper)
{
	AlbumMergeResult result;

	auto newSongs = SongsWithoutAlbums(existingAlbums, songs);
	auto groups = GroupSongsByAlbumTitle(newSongs);
	auto albumLookup = CreateAlbumLookup(existingAlbums);
	result.newAlbums = CreateNewAlbums(albumLookup, groups, mapper);
	result.modifiedAlbums = CreateModifiedAlbums(albumLookup, groups);

	ARC_ASSERT(result.newAlbums.size() + result.modifiedAlbums.size() == groups.size());

	// only need to fix-up new ones, modified ones should be fine as is
	FixupAlbumImages(result.newAlbums);

	return result;
}

vector<pair<wstring, const Song*>> DetermineMissingFiles(const SongCollection& existingSongs, const vector<shared_ptr<IFile>>& files)
{
	vector<pair<wstring, const Song*>> expectedPaths;
	vector<pair<wstring, const Song*>> actualPaths;

	expectedPaths.reserve(existingSongs.size()); // won't be enough, but should be close
	actualPaths.reserve(files.size()); // will be exactly enough

	for (auto& songPair : existingSongs)
	{
		auto& formats = songPair.second.GetFiles();
		for (auto& songFiles : formats)
		{
			transform(begin(songFiles.second), end(songFiles.second), back_inserter(expectedPaths),
				[&songPair](const SongFile& songFile) -> pair < wstring, const Song* >
			{
				return{ songFile.filePath, &songPair.second };
			});
		}
		
	}
	
	transform(begin(files), end(files), back_inserter(actualPaths), [](const shared_ptr<IFile>& file) -> pair<wstring, const Song*>
	{
		return{ file->GetFullPath(), nullptr };
	});

	auto sortPred = [](const pair<wstring, const Song*> a, const pair<wstring, const Song*>& b)
	{
		return a.first < b.first;
	};

	sort(begin(expectedPaths), end(expectedPaths), sortPred);
	sort(begin(actualPaths), end(actualPaths), sortPred);
	
	vector<pair<wstring, const Song*>> results;

	set_difference(begin(expectedPaths), end(expectedPaths), begin(actualPaths), end(actualPaths), 
				   back_inserter(results), sortPred);

	return results;
}

vector<pair<const Song*, vector<wstring>>> GroupDeletedSongs(vector<pair<wstring, const Song*>> missingSongs)
{
	unordered_map<const Song*, vector<wstring>> groups;
	for (auto& song : missingSongs)
	{
		groups[song.second].push_back(song.first);
	}

	vector<pair<const Song*, vector<wstring>>> results;
	results.reserve(groups.size());
	copy(begin(groups), end(groups), back_inserter(results));

	return results;
}

vector<Song> CompletelyDeletedSongs(const vector<pair<const Song*, vector<wstring>>>& groups)
{
	vector<pair<const Song*, vector<wstring>>> deleted;
	copy_if(begin(groups), end(groups), back_inserter(deleted), [](const pair<const Song*, vector<wstring>>& group)
	{
		// then every file this song references is gone
		return group.first->GetNumFiles() == group.second.size();
	});

	vector<Song> results;
	results.reserve(deleted.size());
	transform(begin(deleted), end(deleted), back_inserter(results), [](const pair<const Song*, vector<wstring>>& group)
	{
		return *group.first;
	});

	return results;
}

vector<Song> PartiallyDeletedSongs(const vector<pair<const Song*, vector<wstring>>>& groups)
{
	vector<pair<const Song*, vector<wstring>>> modified;
	copy_if(begin(groups), end(groups), back_inserter(modified), [](const pair<const Song*, vector<wstring>>& group)
	{
		// the only some of the files this song referenced are gone
		return group.first->GetNumFiles() != group.second.size();
	});

	vector<Song> results;
	results.reserve(modified.size());
	transform(begin(modified), end(modified), back_inserter(results), [](const pair<const Song*, vector<wstring>>& pair)
	{
		auto song = *pair.first; // take a copy
		for (auto& path : pair.second)
		{
			song.RemoveFile(path);
		}
		return song;
	});

	return results;
}

SongMergeResult Arcusical::MusicProvider::FindDeletedSongs(const SongCollection& existingSongs, const vector<shared_ptr<IFile>>& files)
{
	SongMergeResult result;

	auto missingFiles = DetermineMissingFiles(existingSongs, files);
	auto groups = GroupDeletedSongs(missingFiles);
	result.deletedSongs = CompletelyDeletedSongs(groups);
	result.modifiedSongs = PartiallyDeletedSongs(groups);

	return result;
}

vector<pair<uuid, const Album*>> DetermineMissingSongs(const AlbumCollection& existingAlbums, const SongCollection& songs)
{
	vector<pair<uuid, const Album*>> allAlbumSongIds;
	vector<pair<uuid, const Album*>> allSongIds;
	allAlbumSongIds.reserve(songs.size());
	allSongIds.reserve(songs.size());

	for (auto& album : existingAlbums)
	{
		auto ids = album.second.GetSongIds();
		transform(begin(ids), end(ids), back_inserter(allAlbumSongIds), [&album](const uuid& id) -> pair<uuid, const Album*>
		{
			return { id, &album.second };
		});
	}

	transform(begin(songs), end(songs), back_inserter(allSongIds), [](const SongCollection::value_type& song) -> pair<uuid, const Album*>
	{
		return { song.first, nullptr };
	});

	auto sortPred = [](const pair<uuid, const Album*>& a, const pair<uuid, const Album*>& b)
	{
		return a.first < b.first;
	};

	sort(begin(allAlbumSongIds), end(allAlbumSongIds), sortPred);
	sort(begin(allSongIds), end(allSongIds), sortPred);

	vector<pair<uuid, const Album*>> results;
	set_difference(begin(allAlbumSongIds), end(allAlbumSongIds), begin(allSongIds), end(allSongIds),
				   back_inserter(results), sortPred);

	return results;
}

vector<pair<const Album*, vector<uuid>>> GroupDeletedAlbums(const vector<pair<uuid, const Album*>>& missingSongs)
{
	unordered_map<const Album*, vector<uuid>> groups;
	groups.reserve(missingSongs.size()); // over sized, but better than re allocations

	for (auto& pair : missingSongs)
	{
		groups[pair.second].push_back(pair.first);
	}

	vector<pair<const Album*, vector<uuid>>> results;
	results.reserve(groups.size());
	copy(begin(groups), end(groups), back_inserter(results));

	return results;
}

vector<Album> GetDeletedAlbums(const vector<pair<const Album*, vector<uuid>>>& groups)
{
	vector<pair<const Album*, vector<uuid>>> deleted;
	copy_if(begin(groups), end(groups), back_inserter(deleted), [](const pair<const Album*, vector<uuid>>& pair)
	{
		// if all songs have been removed the album has been deleted
		return pair.first->GetSongIds().size() == pair.second.size();
	});
	
	vector<Album> results;
	results.reserve(deleted.size());

	transform(begin(deleted), end(deleted), back_inserter(results), [](const pair<const Album*, vector<uuid>>& pair)
	{
		auto album = *pair.first;
		album.GetMutableSongIds().clear();
		return album;
	});

	return results;
}

vector<Album> GetPartiallyDeletedAlbums(const vector<pair<const Album*, vector<uuid>>>& groups)
{
	vector<pair<const Album*, vector<uuid>>> modified;
	copy_if(begin(groups), end(groups), back_inserter(modified), [](const pair<const Album*, vector<uuid>>& pair)
	{
		// if all songs have been removed the album has been deleted
		return pair.first->GetSongIds().size() != pair.second.size();
	});

	vector<Album> results;
	results.reserve(modified.size());

	transform(begin(modified), end(modified), back_inserter(results), [](const pair<const Album*, vector<uuid>>& pair)
	{
		auto album = *pair.first;
		for (auto& id : pair.second)
		{
			album.GetMutableSongIds().erase(id);
		}
		return album;
	});

	return results;
}

AlbumMergeResult MusicProvider::FindDeletedAlbums(const AlbumCollection& existingAlbums, const SongCollection& songs)
{
	AlbumMergeResult result;

	auto missingSongs = DetermineMissingSongs(existingAlbums, songs);
	auto groups = GroupDeletedAlbums(missingSongs);
	result.deletedAlbums = GetDeletedAlbums(groups);
	result.modifiedAlbums = GetPartiallyDeletedAlbums(groups);

	ARC_ASSERT(result.deletedAlbums.size() + result.modifiedAlbums.size() == groups.size());

	return result;
}
