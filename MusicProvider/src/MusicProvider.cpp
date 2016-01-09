#define NOMINMAX

#include <algorithm>
#include <cctype>
#include <codecvt>
#include <future>
#include <memory>
#include <vector>
#include "boost/algorithm/string/predicate.hpp"
#include "boost/functional/hash.hpp"
#include "boost/optional.hpp"

#include "Album.hpp"
#include "AlbumLoader.hpp"
#include "Arc_Assert.hpp"
#include "CheckedCasts.hpp"
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

static const std::chrono::milliseconds TIME_TO_SEARCH{1500};
static const size_t MIN_SONGS_TO_LOAD_AT_ONCE = 20;
static const size_t BASE_SONGS_TO_LOAD_AT_ONCE = 50;
static const size_t MAX_SONGS_TO_LOAD_AT_ONCE = 500;
static const std::chrono::milliseconds SONG_LOAD_TARGET_TIME{4000};
static const float MAX_RATE_OF_CHANGE_TO_SONG_LOAD_RATE =
    0.2f;  // the max percentage that the song load rate can change per iteration

SongCollectionChanges CreateSongCollectionDelta(const boost::optional<SongMergeResult>& mergeResults,
                                                SongCollectionLockedPtr&& songs) {
  SongCollectionChanges result;

  if (mergeResults) {
    transform(begin(mergeResults->newSongs), end(mergeResults->newSongs),
              inserter(result.NewSongs, end(result.NewSongs)), [&songs](const Song& song) -> pair<uuid, const Song*> {
                return {song.GetId(), &songs->at(song.GetId())};
              });
    transform(begin(mergeResults->modifiedSongs), end(mergeResults->modifiedSongs),
              inserter(result.ModifiedSongs, end(result.NewSongs)),
              [&songs](const Song& song) -> pair<uuid, const Song*> {
                return {song.GetId(), &songs->at(song.GetId())};
              });
  }

  result.AllSongs = std::move(songs);

  return std::move(result);
}

AlbumCollectionChanges CreateAlbumCollectionDelta(const boost::optional<AlbumMergeResult>& mergeResults,
                                                  AlbumCollectionLockedPtr&& albums) {
  AlbumCollectionChanges result;

  if (mergeResults) {
    transform(begin(mergeResults->newAlbums), end(mergeResults->newAlbums),
              inserter(result.NewAlbums, end(result.NewAlbums)),
              [&albums](const Album& album) -> pair<uuid, const Album*> {
                return {album.GetId(), &albums->at(album.GetId())};
              });
    transform(begin(mergeResults->modifiedAlbums), end(mergeResults->modifiedAlbums),
              inserter(result.ModifiedAlbums, end(result.NewAlbums)),
              [&albums](const Album& album) -> pair<uuid, const Album*> {
                return {album.GetId(), &albums->at(album.GetId())};
              });
  }

  result.AllAlbums = std::move(albums);

  return std::move(result);
}

template <typename CB>
void PublishSongs(const CB& cb, SongCollectionLockedPtr&& songs, LoadProgress progress, mutex& lock,
                  const boost::optional<SongMergeResult>& mergeResults = boost::none) {
  auto changes = CreateSongCollectionDelta(mergeResults, std::move(songs));
  changes.Progress = progress;

  unique_lock<mutex> callbackLock(lock);
  cb(changes);
}

template <typename CB>
void PublishAlbums(const CB& cb, AlbumCollectionLockedPtr&& albums, LoadProgress progress, mutex& lock,
                   const boost::optional<AlbumMergeResult>& mergeResults = boost::none) {
  auto changes = CreateAlbumCollectionDelta(mergeResults, std::move(albums));
  changes.Progress = progress;

  unique_lock<mutex> callbackLock(lock);
  cb(changes);
}

template <typename CB>
void PublishAlbums(const CB& cb, AlbumCollectionLockedPtr&& albums, LoadProgress progress, mutex& lock,
                   const vector<boost::uuids::uuid>& modifiedAlbumIds) {
  AlbumCollectionChanges changes;
  changes.ModifiedAlbums.reserve(modifiedAlbumIds.size());

  for (auto& id : modifiedAlbumIds) {
    const Album* albumPtr = nullptr;
    auto albumItr = albums->find(id);
    ARC_ASSERT(albumItr != end(*albums));
    if (albumItr != end(*albums)) {
      albumPtr = &albumItr->second;
    }
    changes.ModifiedAlbums[id] = albumPtr;
  };

  changes.AllAlbums = std::move(albums);
  changes.Progress = progress;

  unique_lock<mutex> callbackLock(lock);
  cb(changes);
}

MusicProvider::MusicProvider(LocalMusicCache* cache)
    : m_songCallbacks(),
      m_musicCache(cache),
      m_songSelector(m_musicCache),
      m_songLoadProgress(LoadProgress::CACHE_LOAD_PENDING),
      m_albumLoadProgress(LoadProgress::CACHE_LOAD_PENDING),
      m_artLoader(cache) {
  ARC_ASSERT(cache != nullptr);
  m_artLoadSubscription = m_artLoader.OnArtLoaded +=
      [this](const std::vector<boost::uuids::uuid>& ids) { OnArtLoaded(ids); };
}

Subscription MusicProvider::SubscribeSongs(SongsChangedCallback callback) {
  Subscription subscription;
  {
    unique_lock<mutex> callbackLock(m_songCallbackLock);
    subscription = m_songCallbacks += callback;
  }

  {
    unique_lock<mutex> loadingLock(m_isLoadingLock);

    if (!m_hasSongLoadingBegun) {
      m_hasSongLoadingBegun = true;
      m_songsLoadingFuture = async(std::launch::async, [this]() { LoadSongs(); });
    } else {
      auto songs = m_musicCache->GetLocalSongs();
      PublishSongs(callback, std::move(songs), m_songLoadProgress, m_songCallbackLock);
    }
  }

  return subscription;
}

Subscription MusicProvider::SubscribeAlbums(AlbumsChangedCallback callback) {
  Subscription subscription;
  {
    unique_lock<mutex> callbackLock(m_albumCallbackLock);
    subscription = m_albumCallbacks += callback;
  }

  {
    unique_lock<mutex> loadingLock(m_isLoadingLock);

    if (!m_hasAlbumLoadingBegun) {
      m_hasAlbumLoadingBegun = true;
      m_albumsLoadingFuture = async(std::launch::async, [this]() { this->LoadAlbums(); });
    } else {
      auto albums = m_musicCache->GetLocalAlbums();
      PublishAlbums(callback, std::move(albums), m_albumLoadProgress, m_albumCallbackLock);
    }
  }

  return subscription;
}

vector<Song> MusicProvider::GetSongsFromFiles(const vector<shared_ptr<FileSystem::IFile>>& files) {
  vector<wstring> temp;
  transform(begin(files), end(files), back_inserter(temp), [](auto file) { return file->GetFullPath(); });

  vector<Song> requestedSongs;
  // First get the songs we have already loaded.
  {
    auto allSongs = m_musicCache->GetLocalSongs();
    vector<pair<uuid, Song>> songsRequestedWithID;
    copy_if(begin(*allSongs), end(*allSongs), back_inserter(songsRequestedWithID), [&files](const auto& song) {
      auto songFiles = song.second.GetSongFiles();
      return any_of(begin(songFiles), end(songFiles), [&files](const auto& songFile) {
        return any_of(begin(files), end(files),
                      [&songFile](const auto& file) { return songFile.filePath == file->GetFullPath(); });
      });
    });
    requestedSongs.reserve(songsRequestedWithID.size());
    transform(begin(songsRequestedWithID), end(songsRequestedWithID), back_inserter(requestedSongs),
              [](auto& songWithID) { return songWithID.second; });
  }
  // Now figure out what files have not yet been loaded
  std::vector<std::shared_ptr<FileSystem::IFile>> filesToLoad;
  copy_if(begin(files), end(files), back_inserter(filesToLoad), [&requestedSongs](const auto& file) {
    return none_of(begin(requestedSongs), end(requestedSongs), [&file](const auto& song) {
      auto songFiles = song.GetSongFiles();
      return any_of(begin(songFiles), end(songFiles),
                    [&file](const auto& songFile) { return file->GetFullPath() == songFile.filePath; });
    });
  });

  if (filesToLoad.size() > 0) {
    shared_ptr<SongMergeResult> mergedSongs;
    {
      auto allSongs = m_musicCache->GetLocalSongs();
      mergedSongs = make_shared<SongMergeResult>(MergeSongs(*allSongs, filesToLoad));
    }
    m_musicCache->AddToCache(mergedSongs->newSongs);
    m_musicCache->AddToCache(mergedSongs->modifiedSongs);
    m_musicCache->SaveSongs();
    auto allSongs = m_musicCache->GetLocalSongs();
    PublishSongs(m_songCallbacks, std::move(allSongs), LoadProgress::DISK_LOAD_IN_PROGRESS, m_songCallbackLock,
                 *mergedSongs);

    requestedSongs.insert(end(requestedSongs), begin(mergedSongs->newSongs), end(mergedSongs->newSongs));
  }

  sort(begin(requestedSongs), end(requestedSongs));
  return requestedSongs;
}

SongSelector* MusicProvider::GetSongSelector() { return &m_songSelector; }

void MusicProvider::LoadSongs() {
  auto songFilesWB = FileSystem::Storage::MusicFolder().FindFilesWithExtensions({L".m4a", L".wav", L".mp3", L".flac"});

  // and publish what we have in the cache
  {
    auto songs = m_musicCache->GetLocalSongs();
    m_songLoadProgress = LoadProgress::CACHE_LOAD_COMPLETE;
    PublishSongs(m_songCallbacks, std::move(songs), m_songLoadProgress, m_songCallbackLock);
  }

  m_songLoadProgress = LoadProgress::DISK_LOAD_IN_PROGRESS;
  auto songFiles = ProcessSongFiles(*songFilesWB);
  SongMergeResult deletedSongs;

  {
    auto songs = m_musicCache->GetLocalSongs();
    deletedSongs = FindDeletedSongs(*songs, songFiles);
  }
  if (deletedSongs.modifiedSongs.size() > 0 || deletedSongs.deletedSongs.size() > 0) {
    m_musicCache->AddToCache(deletedSongs.modifiedSongs);
    m_musicCache->RemoveFromCache(deletedSongs.deletedSongs);
    m_musicCache->SaveSongs();
  }
  {
    auto songs = m_musicCache->GetLocalSongs();
    m_songLoadProgress = LoadProgress::DISK_LOAD_COMPLETE;
    PublishSongs(m_songCallbacks, std::move(songs), m_songLoadProgress, m_songCallbackLock, deletedSongs);
  }
}

vector<FileSystem::FilePtr> MusicProvider::ProcessSongFiles(Util::WorkBuffer<FileSystem::FilePtr>& songFilesWB) {
  auto numSongsToLoad = BASE_SONGS_TO_LOAD_AT_ONCE;
  chrono::milliseconds timeSpentLoading = 0ms;
  size_t numSongsLoaded = 0;
  boost::optional<future<void>> publishFuture = boost::none;
  vector<shared_ptr<FileSystem::IFile>> songFiles;

  while (songFilesWB.ResultsPending()) {
    // note: this has to be a shared_ptr despite being a unique_ptr in practice. This is because VS 2013 does
    // not support generalized lambda capture (C++14). After upgrade to VS 2015 this can be implemented with
    // either move ctors or unique_ptr.
    shared_ptr<SongMergeResult> mergedSongs;
    auto nextBatch = songFilesWB.GetAtMost(numSongsToLoad, TIME_TO_SEARCH);
    if (nextBatch.size() == 0) {
      continue;
    }
    auto loadStartTime = chrono::system_clock::now();

    {
      // scope is because GetLocalSongs returns a lockedPtr, must release as soon as we are done with it!
      auto songs = m_musicCache->GetLocalSongs();
      mergedSongs = make_shared<SongMergeResult>(MergeSongs(*songs, nextBatch));
    }

    auto loadEndTime = chrono::system_clock::now();
    auto elapsed = chrono::duration_cast<chrono::milliseconds>(loadEndTime - loadStartTime);
    timeSpentLoading += elapsed;
    numSongsLoaded += nextBatch.size();

    // load adjustment will try to keep the load time to approx the time indicated in SONG_LOAD_TARGET_TIME
    // this will help adapt the publish time to be on a steady interval instead of variable. This method is
    // not perfect as we are attempting to estimate future load times based on the prior load time.

    if (numSongsLoaded > 0 && timeSpentLoading.count() > 0) {
      auto loadTimePerSong = timeSpentLoading.count() / numSongsLoaded;
      if (loadTimePerSong > 0) {
        auto lowerRateLimit = (size_t)(numSongsToLoad * (1.0 - MAX_RATE_OF_CHANGE_TO_SONG_LOAD_RATE));
        auto upperRateLimit = (size_t)(numSongsToLoad * (1.0 + MAX_RATE_OF_CHANGE_TO_SONG_LOAD_RATE));
        auto newCount = Util::SafeIntCast<decltype(numSongsToLoad)>(SONG_LOAD_TARGET_TIME.count() / loadTimePerSong);
        numSongsToLoad = (newCount + numSongsToLoad) / 2;  // avg them out so sudden jumps are evened out a bit
        numSongsToLoad = max(lowerRateLimit, numSongsToLoad);
        numSongsToLoad = min(upperRateLimit, numSongsToLoad);
        numSongsToLoad = max(MIN_SONGS_TO_LOAD_AT_ONCE, numSongsToLoad);
        numSongsToLoad = min(MAX_SONGS_TO_LOAD_AT_ONCE, numSongsToLoad);
      }
    }

    if (mergedSongs->newSongs.size() > 0 || mergedSongs->modifiedSongs.size() > 0) {
      // we want to ensure publishes are sequential regardless of thread scheduling, after all it would be difficult
      // if results were received for a modified song before it had been received as a new song.
      if (publishFuture) {
        publishFuture->wait();
      }

      // note: cache should be modified after we are certain the publish has finished, this is so
      // thread scheduling does not result in the cache containing more songs than the publish is informing
      // about. Ie. publish is scheduled, then next batch of songs are loaded and saved to cache and then
      // previous schedule is published. If that happened the publish would not be accurate with respect to
      // the available data.
      m_musicCache->AddToCache(mergedSongs->newSongs);
      m_musicCache->AddToCache(mergedSongs->modifiedSongs);
      m_musicCache->SaveSongs();

      // doing this async so that song loading can continue while other code reads the song info found thus far.
      publishFuture = async(std::launch::async, [this, mergedSongs]() {
        auto songs = m_musicCache->GetLocalSongs();
        PublishSongs(m_songCallbacks, std::move(songs), m_songLoadProgress, m_songCallbackLock, *mergedSongs);
      });
    }
    songFiles.reserve(songFiles.size() + nextBatch.size());
    std::move(begin(nextBatch), end(nextBatch), back_inserter(songFiles));
  }

  if (publishFuture) {
    publishFuture->wait();
  }

  return songFiles;
}

void MusicProvider::LoadAlbums() {
  // publish what we have in the cache
  {
    auto albums = m_musicCache->GetLocalAlbums();
    m_albumLoadProgress = LoadProgress::CACHE_LOAD_COMPLETE;
    PublishAlbums(m_albumCallbacks, std::move(albums), m_albumLoadProgress, m_albumCallbackLock);
  }
  // now kick off the album art verification
  {
    auto albums = m_musicCache->GetLocalAlbums();
    vector<boost::uuids::uuid> allAlbumIds;
    allAlbumIds.reserve(albums->size());
    transform(begin(*albums), end(*albums), back_inserter(allAlbumIds),
              [](const AlbumCollection::value_type& pair) { return pair.first; });
    m_artLoader.AlbumsToVerify()->Append(std::move(allAlbumIds));
  }

  // now subscribe to the music search service
  function<void(const SongCollectionChanges&)> songsCallback = [this](const SongCollectionChanges& songs) {
    AlbumMergeResult mergedAlbums;
    AlbumMergeResult deletedAlbums;

    m_albumLoadProgress = LoadProgress::DISK_LOAD_IN_PROGRESS;

    // first determine new albums and modified albums
    {
      auto albums = m_musicCache->GetLocalAlbums();
      mergedAlbums = MergeAlbums(*albums, *songs.AllSongs, m_musicCache);
    }
    if (mergedAlbums.newAlbums.size() > 0 || mergedAlbums.modifiedAlbums.size() > 0) {
      m_musicCache->AddToCache(mergedAlbums.modifiedAlbums);
      m_musicCache->AddToCache(mergedAlbums.newAlbums);
      {
        auto albums = m_musicCache->GetLocalAlbums();
        PublishAlbums(m_albumCallbacks, std::move(albums), m_albumLoadProgress, m_albumCallbackLock, mergedAlbums);
      }
      m_musicCache->SaveAlbums();
    }

    // second, delete albums that no longer exist
    {
      auto albums = m_musicCache->GetLocalAlbums();
      deletedAlbums = FindDeletedAlbums(*albums, *songs.AllSongs);
    }
    if (deletedAlbums.modifiedAlbums.size() > 0 || deletedAlbums.deletedAlbums.size() > 0) {
      m_musicCache->AddToCache(deletedAlbums.modifiedAlbums);
      m_musicCache->RemoveFromCache(deletedAlbums.deletedAlbums);
      {
        auto albums = m_musicCache->GetLocalAlbums();
        PublishAlbums(m_albumCallbacks, std::move(albums), m_albumLoadProgress, m_albumCallbackLock, deletedAlbums);
      }

      m_musicCache->SaveAlbums();
    }

    // third, kick off a load of the album art
    LoadAlbumArt(mergedAlbums);
    // and notify the art loader that all albums and songs should be loaded now
    // so it can start its delayed load processing.
    if (songs.Progress == LoadProgress::DISK_LOAD_COMPLETE) {
      m_artLoader.NotifyLoadingComplete();
    }
  };

  m_songSubscription = this->SubscribeSongs(songsCallback);
}

void Arcusical::MusicProvider::MusicProvider::LoadAlbumArt(const AlbumMergeResult& albumChanges) {
  vector<boost::uuids::uuid> newAlbumIds;
  vector<boost::uuids::uuid> modifiedAlbumIds;
  newAlbumIds.reserve(albumChanges.newAlbums.size());
  modifiedAlbumIds.reserve(albumChanges.modifiedAlbums.size());

  auto idExtractor = [](const Album& album) { return album.GetId(); };

  transform(begin(albumChanges.newAlbums), end(albumChanges.newAlbums), back_inserter(newAlbumIds), idExtractor);
  transform(begin(albumChanges.modifiedAlbums), end(albumChanges.modifiedAlbums), back_inserter(modifiedAlbumIds),
            idExtractor);

  m_artLoader.AlbumsNeedingArt()->Append(std::move(newAlbumIds));
  m_artLoader.AlbumsToVerify()->Append(std::move(modifiedAlbumIds));
}

void Arcusical::MusicProvider::MusicProvider::OnArtLoaded(const std::vector<boost::uuids::uuid> albumIdsLoaded) {
  auto albums = m_musicCache->GetLocalAlbums();
  PublishAlbums(m_albumCallbacks, std::move(albums), m_albumLoadProgress, m_albumCallbackLock, albumIdsLoaded);
}

boost::optional<Album> MusicProvider::GetAlbum(const wstring& name) {
  auto albums = m_musicCache->GetLocalAlbums();

  auto albumItr = find_if(begin(*albums), end(*albums), [&name](const pair<uuid, Album>& albumPair) {
    return boost::iequals(albumPair.second.GetTitle(), name);
  });

  if (albumItr == end(*albums)) {
    return boost::none;
  } else {
    return albumItr->second;
  }
}