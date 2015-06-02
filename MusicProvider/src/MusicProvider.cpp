#define NOMINMAX

#include <algorithm>
#include "boost/algorithm/string/predicate.hpp"
#include "boost/functional/hash.hpp"
#include <cctype>
#include <codecvt>
#include <memory>
#include <future>
#include <vector>

#include "Arc_Assert.hpp"
#include "Album.hpp"
#include "IFile.hpp"
#include "IFolder.hpp"
#include "LocalMusicCache.hpp"
#include "MusicProvider.hpp"
#include "Song.hpp"
#include "SongLoader.hpp"
#include "Storage.hpp"

using namespace Arcusical::LocalMusicStore;
using namespace Arcusical::Model;
using namespace Arcusical::MusicProvider;
using namespace boost::uuids;
using namespace std;
using namespace Util;

static const std::chrono::milliseconds TIME_TO_SEARCH{ 1500 };
static const size_t MAX_SONGS_TO_LOAD_AT_ONCE = 50;

SongCollectionChanges CreateSongCollectionDelta(const boost::optional<SongMergeResult>& mergeResults,
												SongCollectionLockedPtr&& songs)
{
	SongCollectionChanges result;

	if (mergeResults)
	{
		transform(begin(mergeResults->newSongs), end(mergeResults->newSongs),
			inserter(result.NewSongs, end(result.NewSongs)), [&songs](const Song& song) ->pair <uuid, const Song*>
		{
			return { song.GetId(), &songs->at(song.GetId()) };
		});
		transform(begin(mergeResults->modifiedSongs), end(mergeResults->modifiedSongs),
			inserter(result.ModifiedSongs, end(result.NewSongs)), [&songs](const Song& song) ->pair <uuid, const Song*>
		{
			return { song.GetId(), &songs->at(song.GetId()) };
		});
	}

	result.AllSongs = std::move(songs);

	return std::move(result);
}

AlbumCollectionChanges CreateAlbumCollectionDelta(const boost::optional<AlbumMergeResult>& mergeResults,
												 AlbumCollectionLockedPtr&& albums)
{
	AlbumCollectionChanges result;

	if (mergeResults)
	{
		transform(begin(mergeResults->newAlbums), end(mergeResults->newAlbums),
			inserter(result.NewAlbums, end(result.NewAlbums)), [&albums](const Album& album) -> pair<uuid, const Album*>
		{
			return { album.GetId(), &albums->at(album.GetId()) };
		});
		transform(begin(mergeResults->modifiedAlbums), end(mergeResults->modifiedAlbums),
			inserter(result.ModifiedAlbums, end(result.NewAlbums)), [&albums](const Album& album) -> pair<uuid, const Album*>
		{
			return { album.GetId(), &albums->at(album.GetId()) };
		});
	}

	result.AllAlbums = std::move(albums);

	return std::move(result);
}

template<typename CB>
void PublishSongs(const CB& cb, 
				  SongCollectionLockedPtr&& songs,
				  mutex& lock,
				  const boost::optional<SongMergeResult>& mergeResults = boost::none)
{
	auto changes = CreateSongCollectionDelta(mergeResults, std::move(songs));

	unique_lock<mutex> callbackLock(lock);
	cb(changes);
}

template<typename CB>
void PublishAlbums(const CB& cb,
				   AlbumCollectionLockedPtr&& albums,
				   mutex& lock,
				   const boost::optional<AlbumMergeResult>& mergeResults = boost::none)
{
	auto changes = CreateAlbumCollectionDelta(mergeResults, std::move(albums));

	unique_lock<mutex> callbackLock(lock);
	cb(changes);
}


MusicProvider::MusicProvider(LocalMusicCache* cache)
	: m_songCallbacks()
	, m_musicCache(cache)
	, m_songSelector(m_musicCache)
{ }

Subscription MusicProvider::SubscribeSongs(SongsChangedCallback callback)
{
	Subscription subscription;
	{
		unique_lock<mutex> callbackLock(m_songCallbackLock);
		subscription = m_songCallbacks += callback;
	}

	{
		unique_lock<mutex> loadingLock(m_isLoadingLock);

		if (!m_hasSongLoadingBegun)
		{
			m_hasSongLoadingBegun = true;
			async([this](){ LoadSongs(); });
		}
		else
		{
			auto songs = m_musicCache->GetLocalSongs();
			PublishSongs(callback, std::move(songs), m_songCallbackLock);
		}
	}

	return subscription;
}

Subscription MusicProvider::SubscribeAlbums(AlbumsChangedCallback callback)
{
	Subscription subscription;
	{
		unique_lock<mutex> callbackLock(m_albumCallbackLock);
		subscription = m_albumCallbacks += callback;
	}

	{
		unique_lock<mutex> loadingLock(m_isLoadingLock);

		if (!m_hasAlbumLoadingBegun)
		{
			m_hasAlbumLoadingBegun = true;
			async([this](){ this->LoadAlbums(); });
		}
		else
		{
			auto albums = m_musicCache->GetLocalAlbums();
			PublishAlbums(callback, std::move(albums), m_albumCallbackLock);
		}
	}

	return subscription;
}

SongSelector* MusicProvider::GetSongSelector()
{
	return &m_songSelector;
}

void MusicProvider::LoadSongs()
{
	auto songFilesWB = FileSystem::Storage::MusicFolder().FindFilesWithExtensions({ L".m4a", L".wav", L".mp3" });

	// and publish what we have in the cache
	{
		auto songs = m_musicCache->GetLocalSongs();
		PublishSongs(m_songCallbacks, std::move(songs), m_songCallbackLock);
	}

	SongMergeResult mergedSongs;
	SongMergeResult deletedSongs;
	decltype(songFilesWB->GetAll()) songFiles;

	while (songFilesWB->ResultsPending())
	{
		auto nextBatch = songFilesWB->WaitAndGet(TIME_TO_SEARCH, MAX_SONGS_TO_LOAD_AT_ONCE);

		{
			// scope is because GetLocalSongs returns a lockedPtr, must release as soon as we are done with it!
			auto songs = m_musicCache->GetLocalSongs();
			mergedSongs = MergeSongs(*songs, nextBatch);
		}
		if (mergedSongs.newSongs.size() > 0 || mergedSongs.modifiedSongs.size() > 0)
		{
			m_musicCache->AddToCache(mergedSongs.newSongs);
			m_musicCache->AddToCache(mergedSongs.modifiedSongs);
			auto songs = m_musicCache->GetLocalSongs();
			PublishSongs(m_songCallbacks, std::move(songs), m_songCallbackLock, mergedSongs);
			m_musicCache->SaveSongs();
		}
		songFiles.reserve(songFiles.size() + nextBatch.size());
		std::move(begin(nextBatch), end(nextBatch), back_inserter(songFiles));
	}
	{
		auto songs = m_musicCache->GetLocalSongs();
		deletedSongs = FindDeletedSongs(*songs, songFiles);
	}
	if (deletedSongs.modifiedSongs.size() > 0 || deletedSongs.deletedSongs.size() > 0)
	{
		m_musicCache->AddToCache(deletedSongs.modifiedSongs);
		m_musicCache->RemoveFromCache(deletedSongs.deletedSongs);
		{
			auto songs = m_musicCache->GetLocalSongs();
			PublishSongs(m_songCallbacks, std::move(songs), m_songCallbackLock, deletedSongs);
		}
		m_musicCache->SaveSongs();
	}
}

void MusicProvider::LoadAlbums()
{
	// publish what we have in the cache
	{
		auto albums = m_musicCache->GetLocalAlbums();
		PublishAlbums(m_albumCallbacks, std::move(albums), m_albumCallbackLock);
	}
	
	// now subscribe to the music search service
	function<void(const SongCollectionChanges&)> songsCallback = [this](const SongCollectionChanges& songs)
	{
		AlbumMergeResult mergedAlbums;
		AlbumMergeResult deletedAlbums;

		{
			auto albums = m_musicCache->GetLocalAlbums();
			mergedAlbums = MergeAlbums(*albums, *songs.AllSongs, m_musicCache);
		}
		if (mergedAlbums.newAlbums.size() > 0 || mergedAlbums.modifiedAlbums.size() > 0)
		{
			m_musicCache->AddToCache(mergedAlbums.modifiedAlbums);
			m_musicCache->AddToCache(mergedAlbums.newAlbums);
			{
				auto albums = m_musicCache->GetLocalAlbums();
				PublishAlbums(m_albumCallbacks, std::move(albums), m_albumCallbackLock, mergedAlbums);
			}
			m_musicCache->SaveAlbums();
		}
		{
			auto albums = m_musicCache->GetLocalAlbums();
			deletedAlbums = FindDeletedAlbums(*albums, *songs.AllSongs);
		}
		if (deletedAlbums.modifiedAlbums.size() > 0 || deletedAlbums.deletedAlbums.size() > 0)
		{
			m_musicCache->AddToCache(deletedAlbums.modifiedAlbums);
			m_musicCache->RemoveFromCache(deletedAlbums.deletedAlbums);
			{
				auto albums = m_musicCache->GetLocalAlbums();
				PublishAlbums(m_albumCallbacks, std::move(albums), m_albumCallbackLock, deletedAlbums);
			}
			
			m_musicCache->SaveAlbums();
		}
	};

	m_songSubscription = this->SubscribeSongs(songsCallback);
}

boost::optional<Album> MusicProvider::GetAlbum(const wstring& name)
{
	auto albums = m_musicCache->GetLocalAlbums();

	auto albumItr = find_if(begin(*albums), end(*albums), [&name](const pair<uuid, Album>& albumPair)
	{
		return boost::iequals(albumPair.second.GetTitle(), name);
	});

	if (albumItr == end(*albums))
	{
		return boost::none;
	}
	else
	{
		return albumItr->second;
	}
}