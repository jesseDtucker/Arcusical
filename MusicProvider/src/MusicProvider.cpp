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
#include "LocalMusicCache.hpp"
#include "MusicFinder.hpp"
#include "MusicProvider.hpp"
#include "Song.hpp"
#include "SongLoader.hpp"
#include "Storage.hpp"

#undef max
#undef min

using namespace Arcusical::LocalMusicStore;
using namespace Arcusical::Model;
using namespace Arcusical::MusicProvider;
using namespace boost::uuids;
using namespace std;
using namespace Util;

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

	result.AllSongs = move(songs);

	return move(result);
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

	result.AllAlbums = move(albums);

	return move(result);
}

template<typename CB>
void PublishSongs(const CB& cb, 
				  SongCollectionLockedPtr&& songs,
				  mutex& lock,
				  const boost::optional<SongMergeResult>& mergeResults = boost::none)
{
	if (songs->size() == 0)
	{
		return;
	}

	auto changes = CreateSongCollectionDelta(mergeResults, move(songs));

	unique_lock<mutex> callbackLock(lock);
	cb(changes);
}

template<typename CB>
void PublishAlbums(const CB& cb,
				   AlbumCollectionLockedPtr&& albums,
				   mutex& lock,
				   const boost::optional<AlbumMergeResult>& mergeResults = boost::none)
{
	if (albums->size() == 0)
	{
		return;
	}

	auto changes = CreateAlbumCollectionDelta(mergeResults, move(albums));

	unique_lock<mutex> callbackLock(lock);
	cb(changes);
}


MusicProvider::MusicProvider(LocalMusicCache* cache)
	: m_songCallbacks()
	, m_musicCache(cache)
	, m_musicFinder()
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
			PublishSongs(callback, move(songs), m_songCallbackLock);
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
			PublishAlbums(callback, move(albums), m_albumCallbackLock);
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
	// kick off a concurrent search for files from disk
	auto musicFinderFuture = m_musicFinder.FindSongs();

	// and publish what we have in the cache
	{
		auto songs = m_musicCache->GetLocalSongs();
		PublishSongs(m_songCallbacks, move(songs), m_songCallbackLock);
	}

	SongMergeResult mergedSongs;
	{
		// scope is because GetLocalSongs returns a lockedPtr, must release as soon as we are done with it!
		auto songs = m_musicCache->GetLocalSongs();
		auto musicFinderResults = musicFinderFuture.get();
		mergedSongs = MergeSongs(*songs, musicFinderResults);
	}
	if (mergedSongs.newSongs.size() > 0 || mergedSongs.modifiedSongs.size() > 0)
	{
		m_musicCache->AddToCache(mergedSongs.newSongs);
		m_musicCache->AddToCache(mergedSongs.modifiedSongs);
		auto songs = m_musicCache->GetLocalSongs();
		PublishSongs(m_songCallbacks, move(songs), m_songCallbackLock, mergedSongs);
		m_musicCache->SaveSongs();
	}
	// TODO::JT check for deleted songs
}

void MusicProvider::LoadAlbums()
{
	// publish what we have in the cache
	{
		auto albums = m_musicCache->GetLocalAlbums();
		PublishAlbums(m_albumCallbacks, move(albums), m_albumCallbackLock);
	}
	
	// now subscribe to the music search service
	function<void(const SongCollectionChanges&)> songsCallback = [this](const SongCollectionChanges& songs)
	{
		AlbumMergeResult mergedAlbums;
		{
			auto albums = m_musicCache->GetLocalAlbums();
			mergedAlbums = MergeAlbums(*albums, *songs.AllSongs, m_musicCache);
		}
		if (mergedAlbums.newAlbums.size() > 0 || mergedAlbums.modifiedAlbums.size() > 0)
		{
			m_musicCache->AddToCache(mergedAlbums.modifiedAlbums);
			m_musicCache->AddToCache(mergedAlbums.newAlbums);
			auto albums = m_musicCache->GetLocalAlbums();
			PublishAlbums(m_albumCallbacks, move(albums), m_albumCallbackLock, mergedAlbums);
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