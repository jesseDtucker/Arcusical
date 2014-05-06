#include "pch.h"

#include <future>

#include "MusicProvider.hpp"

#include "Storage.hpp"

#include "IFile.hpp"
#include "SongIdMapper.hpp"
#include "LocalMusicCache.hpp"
#include "MusicFinder.hpp"
#include "Song.hpp"

const std::string Arcusical::MusicProvider::IMusicProvider::ServiceName("MusicProvider");

namespace Arcusical
{
namespace MusicProvider
{
	MusicProvider::MusicProvider()
		: m_callbackSet()
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

	MusicProviderSubscription MusicProvider::Subscribe(MusicChangedCallback callback)
	{
		std::function<void()> unsubscribeCallback = [callback, this](){ this->Unsubscribe(callback); };
		auto subscription = std::make_shared<Util::Subscription>(unsubscribeCallback);
		m_callbackSet.insert(callback);

		{
			std::unique_lock<std::mutex> loadingLock(m_isLoadingLock);

			if (!m_hasLoadingBegun)
			{
				m_hasLoadingBegun = true;
				std::async([this](){ LoadSongs(); });
			}
		}

		std::async([this, callback]()
		{
			{
				std::unique_lock<std::mutex> loadingLock(m_isLoadingLock);

				if (m_isLoading)
				{
					return;
				}
			}

			auto localSongs = m_musicCache->GetLocalSongs().get();
			callback(localSongs, SongListPtr(m_hackRemoteList));
		});

		return subscription;
	}

	void MusicProvider::Unsubscribe(MusicChangedCallback callback)
	{
		m_callbackSet.erase(callback);
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
		if (MergeSongCollections(*cachedSongs.lock(), musicFinderResults) == true)
		{
			// then some songs were merged, we need to republish!
			Publish(cachedSongs, m_hackRemoteList);
		}
	}

	void MusicProvider::LoadAlbums()
	{
		
	}

	bool MusicProvider::MergeSongCollections(std::unordered_map<boost::uuids::uuid, std::shared_ptr<Model::Song>>& existingSongs, std::vector<std::shared_ptr<FileSystem::IFile>>& locatedFiles)
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
		for (auto& callback : m_callbackSet)
		{
			callback(localSongs, remoteSongs);
		}
	}
}
}