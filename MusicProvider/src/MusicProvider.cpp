#include <algorithm>
#include <cctype>
#include <codecvt>
#include <memory>
#include <future>
#include <random>
#include <vector>

#include "Arc_Assert.hpp"
#include "Album.hpp"
#include "IFile.hpp"
#include "LocalMusicCache.hpp"
#include "MusicFinder.hpp"
#include "MusicProvider.hpp"
#include "Song.hpp"
#include "SongLoader.hpp"
#include "Storage.hpp"

#undef max
#undef min

using namespace std;
using namespace Arcusical::LocalMusicStore;
using namespace Arcusical::Model;
using namespace Arcusical::MusicProvider;
using namespace Util;

static void FixupAlbumName(Song& song, const vector<Song>& newSongs, const SongCollection& existingSongs);

MusicProvider::MusicProvider(LocalMusicCache* cache)
	: m_songCallbackSet()
	, m_musicCache(cache)
	, m_musicFinder()
	, m_songSelector(m_musicCache)
{ }

Subscription MusicProvider::SubscribeSongs(SongsChangedCallback callback)
{
	unique_lock<mutex> callbackLock(m_songCallbackLock);

	function<void()> unsubscribeCallback = [callback, this](){ this->Unsubscribe(callback); };
	Subscription subscription{ unsubscribeCallback };
	m_songCallbackSet.insert(callback);

	{
		unique_lock<mutex> loadingLock(m_isLoadingLock);

		if (!m_hasSongLoadingBegun)
		{
			m_hasSongLoadingBegun = true;
			async([this](){ LoadSongs(); });
		}
		else
		{
			callback(*m_musicCache->GetLocalSongs());
		}
	}

	return subscription;
}

Subscription MusicProvider::SubscribeAlbums(AlbumsChangedCallback callback)
{
	unique_lock<mutex> callbackLock(m_albumCallbackLock);

	function<void()> unsubscribeCallback = [callback, this]() { this->Unsubscribe(callback); };
	Subscription subscription{ unsubscribeCallback };
	m_albumCallbackSet.insert(callback);

	{
		unique_lock<mutex> loadingLock(m_isLoadingLock);

		if (!m_hasAlbumLoadingBegun)
		{
			m_hasAlbumLoadingBegun = true;
			async([this](){ this->LoadAlbums(); });
		}
		else
		{
			callback(*m_musicCache->GetLocalAlbums());
		}
	}

	return subscription;
}

SongSelector* MusicProvider::GetSongSelector()
{
	return &m_songSelector;
}

void MusicProvider::Unsubscribe(SongsChangedCallback callback)
{
	unique_lock<mutex> callbackLock(m_songCallbackLock);
	m_songCallbackSet.erase(callback);
}

void MusicProvider::Unsubscribe(AlbumsChangedCallback callback)
{
	unique_lock<mutex> callbackLock(m_albumCallbackLock);
	m_albumCallbackSet.erase(callback);
}

void MusicProvider::LoadSongs()
{
	// Load songs is divided into these steps
	// 1) Load from cache - Publish
	// 2) Search Disk
	// 3) Merge search results with cache - Publish
	// 4) Check remote - Publish // TODO::JT will do this later

	// kick off a concurrent load from disk
	auto musicFinderFuture = m_musicFinder.FindSongs();

	// and publish what we have in the cache
	PublishSongs();

	// intentionally taking a copy of the cache here because we don't want to hold the lock
	// the merge can take a very long time, and would block any access to the cache until we
	// are done. Taking a local copy allows the lock to release right away.
	auto songs = *m_musicCache->GetLocalSongs();
	auto musicFinderResults = musicFinderFuture.get();
	if (MergeSongCollections(songs, musicFinderResults) == true)
	{
		// then some songs were merged, we need to republish!
		PublishSongs();
	}
}

void MusicProvider::LoadAlbums()
{
	// public what we have in the cache
	PublishAlbums();

	// now subscribe to the music search service
	function<void(SongCollection&)> songsCallback = [this](SongCollection& localSongs)
	{
		auto albums = *m_musicCache->GetLocalAlbums();
		if (MergeAlbumCollections(albums, localSongs))
		{
			PublishAlbums();
		}
	};

	m_albumSubscription = this->SubscribeSongs(songsCallback);
}

bool MusicProvider::MergeSongCollections(SongCollection& existingSongs, vector<shared_ptr<FileSystem::IFile>>& locatedFiles)
{
	bool haveFilesBeenAdded = false;

	set<wstring> existingSongFiles;
	vector<Song> newSongs;

	for (auto& song : existingSongs)
	{
		for (const auto& songFile : song.second.GetFiles())
		{
			existingSongFiles.insert(songFile.second.filePath);
		}
	}

	for (auto& file : locatedFiles)
	{
		if (existingSongFiles.find(file->GetFullPath()) == existingSongFiles.end())
		{
			// then we found a song in the file system that wasn't in our cache
			auto song = LoadSong(*file);
			haveFilesBeenAdded = true;
				
			// now we need to see if this song is simply another copy of an existing song or if it is
			// an entirely new song. If it is existing then we can simply add the file to the existing song
			// if not we need to add the song to the cache and the list of existing songs
			typedef SongCollection::value_type SongIdPair;
			auto matchingSongItr = find_if(existingSongs.begin(), existingSongs.end(), [&song](const SongIdPair& pair)
			{
				return song.IsSameSong(pair.second);
			});

			if (matchingSongItr != existingSongs.end())
			{
				// we found an existing song
				// lets check if we already have a song with this encoding
				// However, this should normally never happen, nonetheless the case should be handled
				// If there is a duplicate we will ignore the second song
					
				AddNewSongToExisting(song, matchingSongItr->second, file->GetFullPath());
			}
			else
			{
				// we must also check the new songs to avoid a duplicate, but only if it hasn't already been found
				auto matchingNewSongItr = find_if(newSongs.begin(), newSongs.end(), [&song](const Song otherSong)
				{
					return otherSong.IsSameSong(song);
				});

				if (matchingNewSongItr != newSongs.end())
				{
					// then an existing new song matches
					AddNewSongToExisting(song, *matchingNewSongItr, file->GetFullPath());
				}
				else
				{
					// we found a new song
					newSongs.push_back(song);
				}
			}
		}
	}

	FixupSongs(newSongs, existingSongs);

	if (haveFilesBeenAdded)
	{
		m_musicCache->AddToCache(newSongs);
		m_musicCache->SaveSongs();
	}
		
	return haveFilesBeenAdded;
}

void MusicProvider::FixupAlbums(vector<Album>& newAlbums)
{
	for (auto& album : newAlbums)
	{
		if (album.GetImageFilePath().size() == 0)
		{
			FixupAlbumImage(album);
		}
	}
}

void MusicProvider::FixupAlbumImage(Album& album)
{
	static vector<wstring> DEFAULT_IMAGES = 
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

	// images are selected using a Tetris algorithm
	// the images are selected until none are available then
	// the bag is refilled and selection continues. This
	// guarantees a uniform selection
	if (m_defaultAlbumImgBag.size() == 0)
	{
		m_defaultAlbumImgBag = DEFAULT_IMAGES;
		random_device rd;
		shuffle(begin(m_defaultAlbumImgBag), end(m_defaultAlbumImgBag), default_random_engine(rd()));
	}

	album.SetImageFilePath(m_defaultAlbumImgBag.back());
	m_defaultAlbumImgBag.pop_back();
}

void MusicProvider::FixupSongs(vector<Song>& newSongs, SongCollection& existingSongs)
{
	for (auto& song : newSongs)
	{
		if (song.GetAlbumName().size() == 0)
		{
			FixupAlbumName(song, newSongs, existingSongs);
		}
	}
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
		for (auto& audioFile : song.GetFiles())
		{
			auto path = audioFile.second.filePath;
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

		// now assume the common base folder is the album name
		wstring albumName = L"";
		set<wstring> invalidAlbumNames = {L"mp3", L"wav", L"alac", L"flac"};
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

bool MusicProvider::MergeAlbumCollections(AlbumCollection& existingAlbums, SongCollection& songs)
{
	using namespace boost::uuids;

	// map a title to a possible list of uuids, we need a list as there could be multiple albums with the same name
	unordered_map<wstring, vector<uuid>> albumLookup;
	AlbumCollection newAlbums;
	bool newContentAdded = false;
		
	// first build up a map of song names to their uuids
	for(auto& albumPair : existingAlbums)
	{
		albumLookup[albumPair.second.GetTitle()].push_back(albumPair.first);
	}

	// now for each of the located files we need to look up their album and see if they are in there
	for (auto& songPair : songs)
	{
		auto& song = songPair.second;
		auto albumName = song.GetAlbumName();
			
		if (albumName.length() == 0)
		{
			albumName = L"Unknown Album";
		}

		auto albumLookupItr = albumLookup.find(albumName);

		if (albumLookupItr != end(albumLookup))
		{
			ARC_ASSERT(albumLookupItr->second.size() > 0);

			// we now have a list of albums ids, start by assuming it is the first one and if any matches are found in artist assume it is actually that one
			// for finding the album: it may be in the existing albums or the new albums lists
			auto albumItr = existingAlbums.find(albumLookupItr->second[0]);
			if (albumItr == end(existingAlbums))
			{
				albumItr = newAlbums.find(albumLookupItr->second[0]);
				ARC_ASSERT(albumItr != end(newAlbums));
			}

			auto* album = &(albumItr->second);
				
			for (auto& possibleAlbumId : albumLookupItr->second)
			{
				auto possibleAlbumItr = existingAlbums.find(possibleAlbumId);
				if (possibleAlbumItr == end(existingAlbums))
				{
					possibleAlbumItr = newAlbums.find(possibleAlbumId);
					ARC_ASSERT(possibleAlbumItr != end(newAlbums));
				}

				auto& possibleAlbum = possibleAlbumItr->second;
				if (possibleAlbum.GetArtist() == song.GetArtist())
				{
					// we found an album that has a matching artist
					// given that the artist and album name are the same we will assume they are the same album
					album = &possibleAlbum;
					break;
				}
			}

			if (!newContentAdded)
			{
				// if we haven't added anything new yet check if we are adding something new now
				auto& albumIds = album->GetMutableSongIds();
				newContentAdded = albumIds.find(song.GetId()) == end(albumIds);
			}

			// id's is a set so inserting duplicates is a non issue
			album->GetMutableSongIds().insert(song.GetId());
			if (album->GetImageFilePath().size() == 0)
			{
				// we have not yet managed to located the albums art
				// try again with this song
				auto path = LoadAlbumImage(song, album->GetTitle());
				album->SetImageFilePath(path);
			}
		}
		else
		{
			// we don't have this album yet, so we need to create a new one
			auto newAlbum = CreateAlbum(albumName, song, m_musicCache);
			newContentAdded = true;
			newAlbums.insert({ newAlbum.GetId(), newAlbum });
			albumLookup[newAlbum.GetTitle()].push_back(newAlbum.GetId());
		}
	}

	if (newContentAdded)
	{
		// need to convert the map into a vector
		vector<Album> newAlbumsVec;

		for (auto& albumPair : newAlbums)
		{
			newAlbumsVec.push_back(albumPair.second);
		}

		FixupAlbums(newAlbumsVec);

		m_musicCache->AddToCache(newAlbumsVec);
		m_musicCache->SaveAlbums();
	}

	return newContentAdded;
}

void MusicProvider::AddNewSongToExisting(const Song& newSong, Song& existingSong, wstring fullPath)
{
	auto& newSongFormats = newSong.GetAvailableFormats();
	ARC_ASSERT_MSG(newSongFormats.size() == 1, "This should not be possible. We only loaded one song, it must have a format!");
	if (newSongFormats.size() > 0)
	{
		auto newSongFormat = newSongFormats[0];
		auto& existingFile = existingSong;
		auto& existingFormats = existingFile.GetAvailableFormats();

		// ensure the format isn't already in the model...
		auto itr = find(begin(existingFormats), end(existingFormats), newSongFormat);
		// ARC_ASSERT_MSG(itr == end(existingFormats), "Loaded a 2 different songs that are the same song and same format! Check your data and logic!");
		if (itr == end(existingFormats))
		{
			// good, the value wasn't already there, this is to be expected
			auto files = newSong.GetFiles();
			ARC_ASSERT(files.size() == 1);

			auto songFile = files.begin()->second;
			existingFile.AddFile(songFile);
		}
	}
}

void MusicProvider::PublishSongs()
{
	auto songs = *m_musicCache->GetLocalSongs();

	unique_lock<mutex> callbackLock(m_songCallbackLock);
	for (auto& callback : m_songCallbackSet)
	{
		callback(songs);
	}
}

void MusicProvider::PublishAlbums()
{
	auto albums = *m_musicCache->GetLocalAlbums();

	unique_lock<mutex> callbackLock(m_albumCallbackLock);
	for (auto& callback : m_albumCallbackSet)
	{
		callback(albums);
	}
}

Album MusicProvider::GetAlbum(const wstring& name)
{
	Album result(m_musicCache);
	auto albums = m_musicCache->GetLocalAlbums();

	for (auto& album : *albums)
	{
		if (album.second.GetTitle() == name)
		{
			result = album.second;
			break;
		}
	}

	return result;
}