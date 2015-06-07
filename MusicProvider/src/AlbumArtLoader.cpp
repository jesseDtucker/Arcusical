#include <string>
#include <unordered_map>
#include <utility>

#include "AlbumArtLoader.hpp"
#include "IFile.hpp"
#include "IFileReader.hpp"
#include "IFolder.hpp"
#include "ImageTypes.hpp"
#include "LocalMusicCache.hpp"
#include "MPEG4_Parser.hpp"
#include "MusicTypes.hpp"
#include "Storage.hpp"

using std::begin;
using std::end;
using std::random_device;
using std::tuple;
using std::unordered_map;
using std::vector;
using std::wstring;

using namespace Arcusical::MusicProvider;
using namespace Arcusical::Model;
using namespace Arcusical::MPEG4;
using namespace FileSystem;
using namespace Util;

static const int MIN_VERIFY_BATCH_SIZE = 20;
static const int MIN_EMBEDED_LOAD_BATCH_SIZE = 5;
static const int MAX_EMBEDED_LOAD_BATCH_SIZE = 20;
static const std::chrono::milliseconds EMBEDED_LOAD_INTERVAL{ 500 }; // do not wait more than this amount of time before loading more
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

typedef boost::uuids::uuid SongId;

static vector<tuple<AlbumArtLoader::AlbumId, vector<SongId>>> GetSongIdsForAlbums(const vector<AlbumArtLoader::AlbumId>& ids, const AlbumCollection& albums);
static unordered_map<AlbumArtLoader::AlbumId, wstring, boost::hash<AlbumArtLoader::AlbumId>> BuildAlbumNameLookup(const vector<AlbumArtLoader::AlbumId>& ids, const AlbumCollection& albums);
static vector<tuple<AlbumArtLoader::AlbumId, vector<tuple<wstring, ContainerType>>>> GetPathsForSongIds(const vector<tuple<AlbumArtLoader::AlbumId, vector<SongId>>>& albumInfo, const SongCollection& songs);
static vector<tuple<wstring, ContainerType>> GetFilePaths(const Song& song);

static wstring LoadAlbumImage(const vector<tuple<wstring, ContainerType>>& songPaths, const wstring& albumName);
static boost::optional<wstring> LoadAlbumImage(const tuple<wstring, ContainerType>& songPathj, const wstring& albumName);
static boost::optional<tuple<vector<unsigned char>, ImageType>> GetThumbnail(const IFile& file, const ContainerType& container);
static boost::optional<tuple<vector<unsigned char>, ImageType>> LoadThumbnailFromMpeg4(const IFile& file);
static boost::optional<tuple<vector<unsigned char>, ImageType>> LoadThumbnailFromSystem(const IFile& file);
static boost::optional<wstring> SaveImageFile(vector<unsigned char>& imgData, const wstring& albumName, ImageType imageType);

Arcusical::MusicProvider::AlbumArtLoader::AlbumArtLoader(LocalMusicStore::LocalMusicCache* cache)
	: m_embededLoader([this](const vector<AlbumArtLoader::AlbumId>& albumIds)
							{ return this->EmbededAlbumLoad(albumIds); },
					  true, MIN_EMBEDED_LOAD_BATCH_SIZE, MAX_EMBEDED_LOAD_BATCH_SIZE, EMBEDED_LOAD_INTERVAL)
	, m_cache(cache)
	, m_rand(random_device()())
{
	std::async([this]()
	{
		RecordAlbumArt();
	});

	std::async([this]()
	{
		VerifyAlbums();
	});
}

void Arcusical::MusicProvider::AlbumArtLoader::RecordAlbumArt()
{
	while (m_embededLoader.OutputBuffer()->ResultsPending())
	{
		auto loadResults = m_embededLoader.OutputBuffer()->GetAtLeast(1);
		
		for (auto& res : loadResults)
		{
			FixupAlbumArt(res);
		};

		m_cache->GetLocalAlbumsMutable([&loadResults](Model::AlbumCollection* albums)
		{
			for (auto& result : loadResults)
			{
				auto& id = std::get<0>(result);
				auto& path = std::get<1>(result);
				auto albumItr = albums->find(id);

				ARC_ASSERT(albumItr != end(*albums));
				if (albumItr != end(*albums))
				{
					albumItr->second.SetImageFilePath(path);
				}
			}
		});

		m_cache->SaveAlbums();

		vector<AlbumArtLoader::AlbumId> albumIdsLoaded;
		albumIdsLoaded.reserve(loadResults.size());
		transform(begin(loadResults), end(loadResults), back_inserter(albumIdsLoaded), [](AlbumLoadResult res)
		{
			return std::get<0>(res);
		});

		OnArtLoaded(albumIdsLoaded);
	}
}

void Arcusical::MusicProvider::AlbumArtLoader::FixupAlbumArt(AlbumLoadResult& result)
{
	auto& path = std::get<1>(result);
	if (path.size() == 0)
	{
		// then we pull one out of the bag
		// images are selected using a Tetris algorithm
		// the images are selected until none are available then
		// the bag is refilled and selection continues. This
		// guarantees a mostly uniform selection regardless of 
		// probability and the ensuing chaos of occasionally unlikely odds
		if (m_defaultArtBag.size() == 0) // make sure to refill the bag when it gets empty
		{
			m_defaultArtBag = DEFAULT_IMAGES;
			shuffle(begin(m_defaultArtBag), end(m_defaultArtBag), m_rand);
		}

		path = m_defaultArtBag.back();
		m_defaultArtBag.pop_back();
	}
}

InputBuffer<AlbumArtLoader::AlbumId>* AlbumArtLoader::AlbumsNeedingArt()
{
	return m_embededLoader.InputBuffer();
}

InputBuffer<AlbumArtLoader::AlbumId>* AlbumArtLoader::AlbumsToVerify()
{
	return &m_albumsToVerify;
}

void AlbumArtLoader::NotifyLoadingComplete()
{
	m_albumsToVerify.Complete();
}

void AlbumArtLoader::VerifyAlbums()
{
	while (m_albumsToVerify.ResultsPending())
	{
		auto albumIds = m_albumsToVerify.GetAtLeast(MIN_VERIFY_BATCH_SIZE);
		vector<tuple<AlbumArtLoader::AlbumId, wstring>> filePaths;

		{
			auto albums = m_cache->GetLocalAlbums();
			for (auto& id : albumIds)
			{
				auto album = albums->find(id);
				if (album != end(*albums))
				{
					auto filePath = album->second.GetImageFilePath();
					filePaths.push_back(std::make_tuple(id, filePath));
				}
			}
		}

		std::vector<AlbumArtLoader::AlbumId> missingIds;
		for (auto& idPathPair : filePaths)
		{
			if (!FileSystem::Storage::FileExists(std::get<1>(idPathPair)))
			{
				// verification failed, we'll try to load this again
				missingIds.push_back(std::get<0>(idPathPair));
			}
		}

		m_embededLoader.InputBuffer()->Append(std::move(missingIds));
	}
	// done verifying albums
	m_embededLoader.InputBuffer()->Complete();
}

vector<AlbumArtLoader::AlbumLoadResult> AlbumArtLoader::EmbededAlbumLoad(const vector<AlbumArtLoader::AlbumId>& albumIds)
{
	vector<tuple<AlbumArtLoader::AlbumId, vector<SongId>>> songIdsForAlbums;
	vector<tuple<AlbumArtLoader::AlbumId, vector<tuple<wstring, ContainerType>>>> songPathsForAlbums;
	vector<AlbumArtLoader::AlbumLoadResult> albumsWithArt;
	unordered_map<AlbumArtLoader::AlbumId, wstring, boost::hash<AlbumArtLoader::AlbumId>> albumNameLookup;

	albumsWithArt.reserve(albumIds.size());

	{
		auto albums = m_cache->GetLocalAlbums();
		songIdsForAlbums = GetSongIdsForAlbums(albumIds, *albums);
		albumNameLookup = BuildAlbumNameLookup(albumIds, *albums);
	}
	{
		auto songs = m_cache->GetLocalSongs();
		songPathsForAlbums = GetPathsForSongIds(songIdsForAlbums, *songs);
	}

	transform(begin(songPathsForAlbums), end(songPathsForAlbums), back_inserter(albumsWithArt), [&albumNameLookup]
		(const tuple<AlbumArtLoader::AlbumId, vector<tuple<wstring, ContainerType>>>& songPaths)
	{
		auto id = std::get<0>(songPaths);
		auto nameItr = albumNameLookup.find(id);
		ARC_ASSERT(nameItr != end(albumNameLookup) && nameItr->second.size() > 0);
		if (nameItr != end(albumNameLookup))
		{
			auto path = LoadAlbumImage(std::get<1>(songPaths), nameItr->second);
			return std::make_tuple(id, path);
		}
		else
		{
			return std::make_tuple(id, wstring());
		}
	});

	return albumsWithArt;
}

vector<tuple<AlbumArtLoader::AlbumId, vector<SongId>>> GetSongIdsForAlbums(const vector<AlbumArtLoader::AlbumId>& ids, const AlbumCollection& albums)
{
	vector<tuple<AlbumArtLoader::AlbumId, vector<SongId>>> songIdsForAlbums;
	songIdsForAlbums.reserve(albums.size());

	for (auto& id : ids)
	{
		auto albumItr = albums.find(id);
		ARC_ASSERT(albumItr != end(albums)); // we should never be loading a non existent album
		if (albumItr != end(albums))
		{
			auto songIds = albumItr->second.GetSongIds();
			songIdsForAlbums.push_back(std::make_tuple(id, vector < SongId > { begin(songIds), end(songIds) }));
		}
	}

	return songIdsForAlbums;
}

unordered_map<AlbumArtLoader::AlbumId, wstring, boost::hash<AlbumArtLoader::AlbumId>> BuildAlbumNameLookup(const vector<AlbumArtLoader::AlbumId>& ids, const AlbumCollection& albums)
{
	unordered_map<AlbumArtLoader::AlbumId, wstring, boost::hash<AlbumArtLoader::AlbumId>> lookup;

	for (auto& id : ids)
	{
		auto albumItr = albums.find(id);
		ARC_ASSERT(albumItr != end(albums))
		if (albumItr != end(albums))
		{
			lookup[id] = albumItr->second.GetTitle();
		}
	}

	return lookup;
}

vector<tuple<AlbumArtLoader::AlbumId, vector<tuple<wstring, ContainerType>>>> GetPathsForSongIds(const vector<tuple<AlbumArtLoader::AlbumId, vector<SongId>>>& albumInfo, const SongCollection& songs)
{
	vector<tuple<AlbumArtLoader::AlbumId, vector<tuple<wstring, ContainerType>>>> songPathsForAlbums;
	songPathsForAlbums.reserve(albumInfo.size());
	transform(begin(albumInfo), end(albumInfo), back_inserter(songPathsForAlbums), [&songs](const tuple<AlbumArtLoader::AlbumId, vector<SongId>>& info)
	{
		auto& ids = std::get<1>(info);
		vector<tuple<wstring, ContainerType>> paths;
		paths.reserve(ids.size());
		for (auto& id : ids)
		{
			std::wstring songPath = L"";
			const auto& songItr = songs.find(id);
			ARC_ASSERT(songItr != end(songs));
			if (songItr != end(songs))
			{
				auto songPaths = GetFilePaths(songItr->second);
				std::move(begin(songPaths), end(songPaths), back_inserter(paths));
			}
		}

		return make_tuple(std::get<0>(info), paths);
	});

	return songPathsForAlbums;
}

vector<tuple<wstring, ContainerType>> GetFilePaths(const Song& song)
{
	vector<tuple<wstring, ContainerType>> paths;
	auto& filesWithFormats = song.GetFiles();
	paths.reserve(filesWithFormats.size());
	for (auto& format : filesWithFormats)
	{
		for (auto& songFile : format.second)
		{
			paths.push_back(std::make_tuple(songFile.filePath, songFile.container));
		}
	}

	return paths;
}

wstring LoadAlbumImage(const vector<tuple<wstring, ContainerType>>& songPaths, const wstring& albumName)
{
	wstring path;
	for (auto& songPath : songPaths)
	{
		auto imagePath = LoadAlbumImage(songPath, albumName);
		if (imagePath != boost::none)
		{
			path = *imagePath;
			break;
		}
	}

	return path;
}

boost::optional<wstring> LoadAlbumImage(const tuple<wstring, ContainerType>& songPath, const std::wstring& albumName)
{
	boost::optional<wstring> imagePath = boost::none;

	auto& path = std::get<0>(songPath);
	auto& container = std::get<1>(songPath);
	auto songFile = FileSystem::Storage::LoadFileFromPath(path);
	ARC_ASSERT(songFile != nullptr);
	if (songFile != nullptr)
	{
		auto thumbnail = GetThumbnail(*songFile, container);
		if (thumbnail != boost::none)
		{
			imagePath = SaveImageFile(std::get<0>(*thumbnail), albumName, std::get<1>(*thumbnail));
		}
	}

	return imagePath;
}

boost::optional<tuple<vector<unsigned char>, ImageType>> GetThumbnail(const IFile& file, const ContainerType& container)
{
	boost::optional<tuple<vector<unsigned char>, ImageType>> thumb = boost::none;

	switch (container)
	{
	case ContainerType::MP4:
		thumb = LoadThumbnailFromMpeg4(file);
		break;
	default:
		// fall back to system if we can't get it ourselves
		thumb = LoadThumbnailFromSystem(file);
	}

	return thumb;
}

boost::optional<tuple<vector<unsigned char>, ImageType>> LoadThumbnailFromMpeg4(const IFile& file)
{
	boost::optional<tuple<vector<unsigned char>, ImageType>> thumb = boost::none;

	MPEG4_Parser mpegParser;
	auto reader = Storage::GetReader(&file);
	auto mpegTree = mpegParser.ReadAndParseFromStream(*reader);

	auto imageData = mpegTree->GetImageData();
	if (imageData != nullptr && imageData->size() > 0)
	{
		auto imageType = mpegTree->GetImageType();
		thumb = tuple<vector<unsigned char>, ImageType>(std::move(*imageData), imageType);
	}

	return thumb;
}

boost::optional<tuple<vector<unsigned char>, ImageType>> LoadThumbnailFromSystem(const IFile& file)
{
	auto thumbnail = file.GetThumbnail(false); // hopefully the thumbnail is the album art, probably won't work well for .wav or .flac...
	if (thumbnail.size() > 0)
	{
		return make_tuple(thumbnail, ImageType::BMP);
	}
	else
	{
		return boost::none;
	}
}

boost::optional<wstring> SaveImageFile(vector<unsigned char>& imgData, const wstring& albumName, ImageType imageType)
{
	using namespace FileSystem;

	auto fileName = albumName + L"." + ImageTypeToWString.at(imageType);
	Storage::RemoveIllegalCharacters(fileName);
	replace(begin(fileName), end(fileName), '\\', '_');
	replace(begin(fileName), end(fileName), '/', '_');
	fileName = L"albumImgs\\" + fileName;

	auto imgFile = Storage::ApplicationFolder().CreateNewFile(fileName);

	if (nullptr != imgFile)
	{
		imgFile->WriteToFile(imgData);
		return imgFile->GetFullPath();
	}
	else
	{
		return boost::none;
	}
}
