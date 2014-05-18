#include "pch.h"

#include <memory>
#include <future>

#include "Album.hpp"
#include "IFile.hpp"
#include "LocalMusicCache.hpp"
#include "MusicFinder.hpp"
#include "MusicProvider.hpp"
#include "Song.hpp"
#include "SongIdMapper.hpp"
#include "Storage.hpp"

const std::string Arcusical::MusicProvider::IMusicProvider::ServiceName("MusicProvider");

namespace Arcusical
{
namespace MusicProvider
{
	MusicProvider::MusicProvider()
		: m_songCallbackSet()
	{
		SongIdMapper::GetSongsCall getLocalSongs = [this]()
		{ 
			return this->m_musicCache->GetLocalSongs().get();
		};
		SongIdMapper::GetSongsCall getRemoteSongs = [this]()
		{ 
			return std::weak_ptr<std::unordered_map<boost::uuids::uuid, std::shared_ptr<Arcusical::Model::Song>>>(m_hackRemoteList);
		};

		m_songMapper = std::make_shared<SongIdMapper>(getLocalSongs, getRemoteSongs);
		m_musicFinder = std::make_shared<LocalMusicStore::MusicFinder>();
		m_musicCache = std::make_shared<LocalMusicStore::LocalMusicCache>(std::shared_ptr<Arcusical::Model::IAlbumToSongMapper>(m_songMapper));
	}

	MusicProviderSubscription MusicProvider::SubscribeSongs(SongsChangedCallback callback)
	{
		std::unique_lock<std::mutex> callbackLock(m_songCallbackLock);

		std::function<void()> unsubscribeCallback = [callback, this](){ this->Unsubscribe(callback); };
		auto subscription = std::make_shared<Util::Subscription>(unsubscribeCallback);
		m_songCallbackSet.insert(callback);

		{
			std::unique_lock<std::mutex> loadingLock(m_isLoadingLock);

			if (!m_hasSongLoadingBegun)
			{
				m_hasSongLoadingBegun = true;
				std::async([this](){ LoadSongs(); });
			}
			else
			{
				// TODO::JT this could be problematic for the UI thread...
				callback(m_musicCache->GetLocalSongs().get(), m_hackRemoteList);
			}
		}

		return subscription;
	}

	MusicProviderSubscription MusicProvider::SubscribeAlbums(AlbumsChangedCallback callback)
	{
		std::unique_lock<std::mutex> callbackLock(m_albumCallbackLock);

		std::function<void()> unsubscribeCallback = [callback, this]() { this->Unsubscribe(callback); };
		auto subscription = std::make_shared<Util::Subscription>(unsubscribeCallback);
		m_albumCallbackSet.insert(callback);

		{
			std::unique_lock<std::mutex> loadingLock(m_isLoadingLock);

			if (!m_hasAlbumLoadingBegun)
			{
				m_hasAlbumLoadingBegun = true;
				std::async([this](){ this->LoadAlbums(); });
			}
			else
			{
				// TODO::JT this could be problematic for the UI thread...
				callback(m_musicCache->GetLocalAlbums().get());
			}
		}

		return subscription;
	}

	void MusicProvider::Unsubscribe(SongsChangedCallback callback)
	{
		std::unique_lock<std::mutex> callbackLock(m_songCallbackLock);
		m_songCallbackSet.erase(callback);
	}

	void MusicProvider::Unsubscribe(AlbumsChangedCallback callback)
	{
		std::unique_lock<std::mutex> callbackLock(m_albumCallbackLock);
		m_albumCallbackSet.erase(callback);
	}

	void MusicProvider::LoadSongs()
	{
		// Load songs is divided into these steps
		// 1) Load from cache - Publish
		// 2) Search Disk
		// 3) Merge search results with cache - Publish
		// 4) Check remote - Publish // TODO::JT will do this later

		// kick off concurrent loads from both the cache and from disk
		auto localSongsFuture = m_musicCache->GetLocalSongs();
		auto musicFinderFuture = m_musicFinder->FindSongs();

		auto cachedSongs = localSongsFuture.get();
		Publish(cachedSongs, m_hackRemoteList);

		auto musicFinderResults = musicFinderFuture.get();
		auto songs = cachedSongs.lock();
		if (MergeSongCollections(*songs, musicFinderResults) == true)
		{
			// then some songs were merged, we need to republish!
			Publish(cachedSongs, m_hackRemoteList);
		}
	}

	void MusicProvider::LoadAlbums()
	{
		auto cachedAlbums = m_musicCache->GetLocalAlbums().get();
		Publish(cachedAlbums);

		// now subscribe to the music search service
		std::function<void(SongListPtr, SongListPtr)> songsCallback = [this, cachedAlbums](SongListPtr localSongs, SongListPtr remoteSongs)
		{
			auto albums = cachedAlbums.lock();
			if (MergeAlbumCollections(*albums, localSongs))
			{
				Publish(cachedAlbums);
			}
		};

		m_albumSubscription = this->SubscribeSongs(songsCallback);
	}

	bool MusicProvider::MergeSongCollections(const std::unordered_map<boost::uuids::uuid, std::shared_ptr<Model::Song>>& existingSongs, std::vector<std::shared_ptr<FileSystem::IFile>>& locatedFiles)
	{
		bool haveFilesBeenAdded = false;

		std::set<std::wstring> existingSongFiles;
		std::vector<std::shared_ptr<Model::Song>> newSongs;

		for (auto& song : existingSongs)
		{
			for (const auto& songFile : song.second->GetFiles())
			{
				existingSongFiles.insert(songFile.second.filePath);
			}
		}

		for (auto& file : locatedFiles)
		{
			if (existingSongFiles.find(file->GetFullPath()) == existingSongFiles.end())
			{
				// then we found a song in the file system that wasn't in our cache
				auto song = m_songLoader.LoadSong(file);
				haveFilesBeenAdded = true;
				
				// now we need to see if this song is simply another copy of an existing song or if it is
				// an entirely new song. If it is existing then we can simply add the file to the existing song
				// if not we need to add the song to the cache and the list of existing songs
				typedef std::unordered_map<boost::uuids::uuid, std::shared_ptr<Model::Song>>::value_type SongIdPair;
				auto matchingSongItr = std::find_if(existingSongs.begin(), existingSongs.end(), [&song](const SongIdPair& pair)
				{
					return song->IsSameSong(*(pair.second));
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
					auto matchingNewSongItr = std::find_if(newSongs.begin(), newSongs.end(), [&song](const std::shared_ptr<Model::Song> otherSong)
					{
						return otherSong->IsSameSong(*song);
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

		if (haveFilesBeenAdded)
		{
			m_musicCache->AddToCache(newSongs);
			m_musicCache->SaveCache();
		}
		
		return haveFilesBeenAdded;
	}

	bool MusicProvider::MergeAlbumCollections(const std::unordered_map<boost::uuids::uuid, std::shared_ptr<Model::Album>>& existingAlbums, SongListPtr songs)
	{
		using namespace boost::uuids;

		// map a title to a possible list of uuids, we need a list as there could be multiple albums with the same name
		std::unordered_map<std::wstring, std::vector<uuid>> albumLookup;
		std::unordered_map<boost::uuids::uuid, std::shared_ptr<Model::Album>> newAlbums;
		auto allSongs = songs.lock();
		bool newContentAdded = false;
		
		// first build up a map of song names to their uuids
		for(auto& albumPair : existingAlbums)
		{
			albumLookup[albumPair.second->GetTitle()].push_back(albumPair.first);
		}

		// now for each of the located files we need to look up their album and see if they are in there
		for (auto& songPair : *allSongs)
		{
			auto& song = songPair.second;
			auto albumName = song->GetAlbumName();
			
			if (albumName.length() == 0)
			{
				albumName = L"Unknown Album";
			}

			auto albumLookupItr = albumLookup.find(albumName);

			if (albumLookupItr != std::end(albumLookup))
			{
				ARC_ASSERT(albumLookupItr->second.size() > 0);

				// we now have a list of albums ids, start by assuming it is the first one and if any matches are found in artist assume it is actually that one
				// for finding the album: it may be in the existing albums or the new albums lists
				auto albumItr = existingAlbums.find(albumLookupItr->second[0]);
				if (albumItr == std::end(existingAlbums))
				{
					albumItr = newAlbums.find(albumLookupItr->second[0]);
					ARC_ASSERT(albumItr != std::end(newAlbums));
				}

				auto album = albumItr->second;
				
				for (auto& possibleAlbumId : albumLookupItr->second)
				{
					auto possibleAlbumItr = existingAlbums.find(possibleAlbumId);
					if (possibleAlbumItr == std::end(existingAlbums))
					{
						possibleAlbumItr = newAlbums.find(possibleAlbumId);
						ARC_ASSERT(possibleAlbumItr != std::end(newAlbums));
					}

					auto& possibleAlbum = possibleAlbumItr->second;
					if (possibleAlbum->GetArtist() == song->GetArtist())
					{
						// we found an album that has a matching artist
						// given that the artist and album name are the same we will assume they are the same album
						album = possibleAlbum;
						break;
					}
				}

				if (!newContentAdded)
				{
					// if we haven't added anything new yet check if we are adding something new now
					auto& albumIds = album->GetMutableSongIds();
					newContentAdded = albumIds.find(song->GetId()) == std::end(albumIds);
				}

				// id's is a set so inserting duplicates is a non issue
				album->GetMutableSongIds().insert(song->GetId());
			}
			else
			{
				// we don't have this album yet, so we need to create a new one
				auto newAlbum = m_songLoader.CreateAlbum(albumName, song, m_songMapper);
				newContentAdded = true;
				newAlbums[newAlbum->GetId()] = newAlbum;
				albumLookup[newAlbum->GetTitle()].push_back(newAlbum->GetId());
			}
		}

		if (newContentAdded)
		{
			// need to convert the map into a vector
			std::vector<std::shared_ptr<Model::Album>> newAlbumsVec;

			for (auto& albumPair : newAlbums)
			{
				newAlbumsVec.push_back(albumPair.second);
			}

			m_musicCache->AddToCache(newAlbumsVec);
			m_musicCache->SaveCache();
		}

		return newContentAdded;
	}

	void MusicProvider::AddNewSongToExisting(std::shared_ptr<Model::Song> newSong, std::shared_ptr<Model::Song> existingSong, std::wstring fullPath)
	{
		auto& newSongFormats = newSong->GetAvailableFormats();
		ARC_ASSERT_MSG(newSongFormats.size() == 1, "This should not be possible. We only loaded one song, it must have a format!");
		if (newSongFormats.size() > 0)
		{
			auto newSongFormat = newSongFormats[0];
			auto& existingFile = existingSong;
			auto& existingFormats = existingFile->GetAvailableFormats();

			// ensure the format isn't already in the model...
			auto itr = std::find(std::begin(existingFormats), std::end(existingFormats), newSongFormat);
			ARC_ASSERT_MSG(itr == std::end(existingFormats), "Loaded a 2 different songs that are the same song and same format! Check your data and logic!");
			if (itr == std::end(existingFormats))
			{
				// good, the value wasn't already there, this is to be expected
				auto files = newSong->GetFiles();
				ARC_ASSERT(files.size() == 1);

				auto songFile = files.begin()->second;
				existingFile->AddFile(newSongFormat, songFile);
			}
		}
	}

	void MusicProvider::Publish(SongListPtr localSongs, SongListPtr remoteSongs)
	{
		std::unique_lock<std::mutex> callbackLock(m_songCallbackLock);
		for (auto& callback : m_songCallbackSet)
		{
			callback(localSongs, remoteSongs);
		}
	}

	void MusicProvider::Publish(AlbumListPtr albums)
	{
		std::unique_lock<std::mutex> callbackLock(m_albumCallbackLock);
		for (auto& callback : m_albumCallbackSet)
		{
			callback(albums);
		}
	}
}
}