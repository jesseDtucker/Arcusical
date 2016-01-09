#include "LocalMusicCache.hpp"
#include "boost\uuid\string_generator.hpp"
#include "boost\uuid\uuid_io.hpp"
#include <codecvt>

#include "Album.hpp"
#include "Arc_Assert.hpp"
#include "CachedAlbum.pb.h"
#include "CachedAlbumList.pb.h"
#include "CachedSong.pb.h"
#include "CachedSongList.pb.h"
#include "IAlbumToSongMapper.hpp"
#include "IFile.hpp"
#include "IFolder.hpp"
#include "LockHelper.hpp"
#include "LockedPtrHelper.hpp"
#include "Song.hpp"
#include "Storage.hpp"
using namespace std;
using namespace FileSystem;
using namespace Arcusical;
using namespace Arcusical::LocalMusicStore;
using namespace Arcusical::Model;
using namespace Util;

const wstring LocalMusicCache::ALBUM_CACHE_FILE = L"albums.cache";
const wstring LocalMusicCache::SONG_CACHE_FILE = L"songs.cache";

void FillInCachedAlbumFromModel(CachedAlbum& cachedAlbum, Album& modelAlbum);
void FillInCachedSongFromModel(CachedSong& cachedSong, const Song& modelSong);
void SerializeGuid(string* rawBytes, const boost::uuids::uuid guid);
void DeserializeGuid(const string* rawBytes, boost::uuids::uuid& guid);
void FillInModelAlbumFromCached(Album& modelAlbum, const CachedAlbum& cachedAlbum);
void FillInModelSongFromCached(Song& modelSong, const CachedSong& cachedSong);

static const unordered_map<Encoding, AudioFormat> CACHE_ENCODING_TO_MODEL = {{UNKNOWN_ENCODING, AudioFormat::UNKNOWN},
                                                                             {AAC, AudioFormat::AAC},
                                                                             {ALAC, AudioFormat::ALAC},
                                                                             {MP3, AudioFormat::MP3},
                                                                             {FLAC, AudioFormat::FLAC},
                                                                             {WAV, AudioFormat::WAV}};

static const unordered_map<AudioFormat, Encoding> MODEL_ENCODING_TO_CACHE = {{AudioFormat::UNKNOWN, UNKNOWN_ENCODING},
                                                                             {AudioFormat::AAC, AAC},
                                                                             {AudioFormat::ALAC, ALAC},
                                                                             {AudioFormat::MP3, MP3},
                                                                             {AudioFormat::FLAC, FLAC},
                                                                             {AudioFormat::WAV, WAV}};

static const unordered_map<Container, ContainerType> CACHE_CONTAINER_TO_MODEL = {
    {UNKNOWN_CONTAINER, ContainerType::UNKNOWN},
    {MPEG4_CONTAINER, ContainerType::MP4},
    {MP3_CONTAINER, ContainerType::MP3},
    {FLAC_CONTAINER, ContainerType::FLAC},
    {WAV_CONTAINER, ContainerType::WAV}};

static const unordered_map<ContainerType, Container> MODEL_CONTAINER_TO_CACHE = {
    {ContainerType::UNKNOWN, UNKNOWN_CONTAINER},
    {ContainerType::MP4, MPEG4_CONTAINER},
    {ContainerType::MP3, MP3_CONTAINER},
    {ContainerType::FLAC, FLAC_CONTAINER},
    {ContainerType::WAV, WAV_CONTAINER}};

LocalMusicCache::LocalMusicCache() {
  m_songLoadFuture = async(std::launch::async, [this]() { this->LoadSongs(); });
  m_albumLoadFuture = async(std::launch::async, [this]() { this->LoadAlbums(); });
}

SongCollectionLockedPtr LocalMusicCache::GetLocalSongs() const {
  {
    unique_lock<mutex> lock(m_songsLoadingLock);
    if (!m_areSongsLoaded) {
      // if we haven't loaded, then wait until we have
      m_songLoading.wait(lock);
    }
  }

  return CreateReadLockedPointer(&m_songsEditLock, &m_localSongs);
}

AlbumCollectionLockedPtr LocalMusicCache::GetLocalAlbums() const {
  {
    unique_lock<mutex> lock(m_albumsLoadingLock);
    if (!m_areAlbumsLoaded) {
      // if we haven't loaded, then wait until we have
      m_albumsLoading.wait(lock);
    }
  }

  return CreateReadLockedPointer(&m_albumsEditLock, &m_localAlbums);
}

void Arcusical::LocalMusicStore::LocalMusicCache::GetLocalSongsMutable(
    std::function<void(Model::SongCollection* songCollection)> callback) {
  {
    unique_lock<mutex> lock(m_songsLoadingLock);
    if (!m_areSongsLoaded) {
      // if we haven't loaded, then wait until we have
      m_songLoading.wait(lock);
    }
  }

  WriteLock songLock(&m_songsEditLock);
  callback(&m_localSongs);
}

void Arcusical::LocalMusicStore::LocalMusicCache::GetLocalAlbumsMutable(
    std::function<void(Model::AlbumCollection* songCollection)> callback) {
  {
    unique_lock<mutex> lock(m_albumsLoadingLock);
    if (!m_areAlbumsLoaded) {
      // if we haven't loaded, then wait until we have
      m_albumsLoading.wait(lock);
    }
  }

  WriteLock albumLock(&m_albumsEditLock);
  callback(&m_localAlbums);
}

void LocalMusicCache::ClearCache() {
  {
    WriteLock songLock(&m_songsEditLock);
    m_localSongs.clear();
  }
  {
    WriteLock albumLock(&m_albumsEditLock);
    m_localAlbums.clear();
  }
}

void LocalMusicCache::AddToCache(const vector<Album>& albums) {
  WriteLock albumGuard(&m_albumsEditLock);

  for (auto& album : albums) {
    m_localAlbums.erase(
        album.GetId());  // insert does nothing if the key exists, but I want it to replace no matter what
    m_localAlbums.insert({album.GetId(), album});
  }
}

void LocalMusicCache::AddToCache(const vector<Song>& songs) {
  WriteLock songGuard(&m_songsEditLock);

  for (auto& song : songs) {
    m_localSongs[song.GetId()] = song;
  }
}

void LocalMusicCache::LoadAlbums() {
  decltype(m_localAlbums) localAlbums;

  if (Storage::ApplicationFolder().ContainsFile(ALBUM_CACHE_FILE)) {
    vector<unsigned char> buffer;
    auto albumCacheFile = Storage::ApplicationFolder().GetFile(ALBUM_CACHE_FILE);
    albumCacheFile->ReadFromFile(buffer);

    CachedAlbumList cachedAlbums;
    cachedAlbums.ParseFromArray(buffer.data(), buffer.size());

    for (int i = 0; i < cachedAlbums.albums_size(); ++i) {
      auto album = Album(this);
      FillInModelAlbumFromCached(album, cachedAlbums.albums(i));
      localAlbums.insert({album.GetId(), album});
    }
  }

  WriteLock albumLock(&m_albumsEditLock);
  m_localAlbums = std::move(localAlbums);

  unique_lock<mutex> lock(m_albumsLoadingLock);
  m_areAlbumsLoaded = true;
  m_albumsLoading.notify_all();
}

void LocalMusicCache::LoadSongs() {
  decltype(m_localSongs) localSongs;

  if (Storage::ApplicationFolder().ContainsFile(SONG_CACHE_FILE)) {
    vector<unsigned char> buffer;
    auto songCacheFile = Storage::ApplicationFolder().GetFile(SONG_CACHE_FILE);
    songCacheFile->ReadFromFile(buffer);

    CachedSongList cachedSongs;
    cachedSongs.ParseFromArray(buffer.data(), buffer.size());

    for (int i = 0; i < cachedSongs.songs_size(); ++i) {
      auto song = Song();
      FillInModelSongFromCached(song, cachedSongs.songs(i));
      localSongs.insert({song.GetId(), song});
    }
  }

  WriteLock songGuard(&m_songsEditLock);
  m_localSongs = std::move(localSongs);

  unique_lock<mutex> lock(m_songsLoadingLock);
  m_areSongsLoaded = true;
  m_songLoading.notify_all();
}

void LocalMusicCache::SaveAlbums() const {
  CachedAlbumList cachedAlbums;

  decltype(m_localAlbums) localAlbums;

  {
    WriteLock albumGuard(&m_albumsEditLock);
    localAlbums = m_localAlbums;
  }

  for (auto& album : localAlbums) {
    auto cachedAlbum = cachedAlbums.add_albums();
    FillInCachedAlbumFromModel(*cachedAlbum, album.second);
  }

  // ensure we actually have contents to write to file
  if (cachedAlbums.ByteSize() > 0) {
    vector<unsigned char> buffer(cachedAlbums.ByteSize());
    auto result = cachedAlbums.SerializeToArray(buffer.data(), buffer.size());
    ARC_ASSERT_MSG(result, "Failed to serialize album list!");

    auto albumCacheFile = Storage::ApplicationFolder().CreateNewFile(LocalMusicCache::ALBUM_CACHE_FILE);
    albumCacheFile->WriteToFile(buffer);
  }
}

void LocalMusicCache::SaveSongs() const {
  CachedSongList cachedSongs;

  decltype(m_localSongs) localSongs;

  {
    WriteLock songsGuard(&m_songsEditLock);
    localSongs = m_localSongs;
  }

  for (auto& song : localSongs) {
    auto cachedSong = cachedSongs.add_songs();
    FillInCachedSongFromModel(*cachedSong, song.second);
  }

  // ensure we actually have contents to write to file
  if (cachedSongs.ByteSize() > 0) {
    vector<unsigned char> buffer(cachedSongs.ByteSize());
    auto result = cachedSongs.SerializeToArray(buffer.data(), buffer.size());
    ARC_ASSERT_MSG(result, "Failed to serialize song list!");

    auto songCacheFile = Storage::ApplicationFolder().CreateNewFile(SONG_CACHE_FILE);
    songCacheFile->WriteToFile(buffer);
  }
}

void FillInCachedAlbumFromModel(CachedAlbum& cachedAlbum, Album& modelAlbum) {
  // set the id
  auto guid = cachedAlbum.mutable_id();
  auto rawBytes = guid->mutable_rawdata();
  SerializeGuid(rawBytes, modelAlbum.GetId());

  // setup converter
  typedef codecvt_utf8<wchar_t> convert_type;
  wstring_convert<convert_type, wchar_t> converter;

  // apply strings
  cachedAlbum.set_title(converter.to_bytes(modelAlbum.GetTitle()));
  cachedAlbum.set_artist(converter.to_bytes(modelAlbum.GetArtist()));
  cachedAlbum.set_imagefile(converter.to_bytes(modelAlbum.GetImageFilePath()));

  // add ids
  for (const auto& songId : modelAlbum.GetSongIds()) {
    auto songGuid = cachedAlbum.add_songlist();
    SerializeGuid(songGuid->mutable_rawdata(), songId);
  }
}

void FillInCachedSongFromModel(CachedSong& cachedSong, const Song& modelSong) {
  // set the id
  auto guid = cachedSong.mutable_id();
  auto rawBytes = guid->mutable_rawdata();
  SerializeGuid(rawBytes, modelSong.GetId());

  // setup converter
  wstring_convert<codecvt_utf8<wchar_t>, wchar_t> converter;

  // apply strings
  cachedSong.set_title(converter.to_bytes(modelSong.GetTitle()));
  cachedSong.set_artist(converter.to_bytes(modelSong.GetArtist()));
  cachedSong.set_album(converter.to_bytes(modelSong.GetAlbumName()));
  cachedSong.set_length(modelSong.GetLength());

  // set track and disk numbers
  cachedSong.set_tracknumber(modelSong.GetTrackNumber().first);
  cachedSong.set_maxtracknumber(modelSong.GetTrackNumber().second);
  cachedSong.set_disknumber(modelSong.GetDiskNumber().first);
  cachedSong.set_maxdisknumber(modelSong.GetDiskNumber().second);

  for (const auto& files : modelSong.GetFiles()) {
    for (auto& file : files.second) {
      auto songFile = cachedSong.add_files();

      auto cacheEncoding = MODEL_ENCODING_TO_CACHE.find(files.first);
      ARC_ASSERT_MSG(cacheEncoding != end(MODEL_ENCODING_TO_CACHE),
                     "Attempted to cache a format that doesn't have any cache equivelent!");
      if (cacheEncoding != end(MODEL_ENCODING_TO_CACHE)) {
        songFile->set_encoding((*cacheEncoding).second);
      } else {
        songFile->set_encoding(UNKNOWN_ENCODING);
      }

      auto cacheContainer = MODEL_CONTAINER_TO_CACHE.find(file.container);
      if (cacheContainer != end(MODEL_CONTAINER_TO_CACHE)) {
        songFile->set_container((*cacheContainer).second);
      } else {
        songFile->set_container(UNKNOWN_CONTAINER);
      }

      songFile->set_file(converter.to_bytes(file.filePath));
      songFile->set_bitrate(file.bitRate);
      songFile->set_samplesize(file.sampleSize);
      songFile->set_channelcount(file.channelCount);
    }
  }
}

void SerializeGuid(string* rawBytes, const boost::uuids::uuid guid) { *rawBytes = boost::uuids::to_string(guid); }

void DeserializeGuid(const string* rawBytes, boost::uuids::uuid& guid) {
  guid = boost::uuids::string_generator()(*rawBytes);
}

void FillInModelAlbumFromCached(Album& modelAlbum, const CachedAlbum& cachedAlbum) {
  boost::uuids::uuid id;
  DeserializeGuid(&cachedAlbum.id().rawdata(), id);
  modelAlbum.SetId(id);

  wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;
  modelAlbum.SetArtist(converter.from_bytes(cachedAlbum.artist()));
  modelAlbum.SetTitle(converter.from_bytes(cachedAlbum.title()));
  modelAlbum.SetImageFilePath(converter.from_bytes(cachedAlbum.imagefile()));

  for (int i = 0; i < cachedAlbum.songlist_size(); ++i) {
    auto cachedSongId = cachedAlbum.songlist(i);
    boost::uuids::uuid songId;
    DeserializeGuid(&cachedSongId.rawdata(), songId);
    modelAlbum.GetMutableSongIds().insert(songId);
  }
}

void FillInModelSongFromCached(Song& modelSong, const CachedSong& cachedSong) {
  boost::uuids::uuid songId;
  DeserializeGuid(&cachedSong.id().rawdata(), songId);
  modelSong.SetId(songId);

  wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;
  modelSong.SetArtist(converter.from_bytes(cachedSong.artist()));
  modelSong.SetLength(cachedSong.length());
  modelSong.SetTitle(converter.from_bytes(cachedSong.title()));
  modelSong.SetAlbumName(converter.from_bytes(cachedSong.album()));

  modelSong.SetTrackNumber({cachedSong.tracknumber(), cachedSong.maxtracknumber()});
  modelSong.SetDiskNumber({cachedSong.disknumber(), cachedSong.maxdisknumber()});

  for (const auto& cachedFile : cachedSong.files()) {
    auto audioFormat = AudioFormat::UNKNOWN;
    auto encodingItr = CACHE_ENCODING_TO_MODEL.find(cachedFile.encoding());

    ARC_ASSERT_MSG(encodingItr != end(CACHE_ENCODING_TO_MODEL), "Found a format in cache that isn't in the model map!")
    if (encodingItr != end(CACHE_ENCODING_TO_MODEL)) {
      audioFormat = (*encodingItr).second;
    }

    auto container = ContainerType::UNKNOWN;
    auto containerItr = CACHE_CONTAINER_TO_MODEL.find(cachedFile.container());

    ARC_ASSERT_MSG(containerItr != end(CACHE_CONTAINER_TO_MODEL),
                   "Found a container in cache that isn't in model map!");
    if (containerItr != end(CACHE_CONTAINER_TO_MODEL)) {
      container = (*containerItr).second;
    }

    Model::SongFile songFile;
    songFile.format = audioFormat;
    songFile.container = container;
    songFile.filePath = converter.from_bytes(cachedFile.file());
    songFile.bitRate = cachedFile.bitrate();
    songFile.channelCount = cachedFile.channelcount();
    songFile.sampleSize = cachedFile.samplesize();
    modelSong.AddFile(songFile);
  }
}

SongCollection LocalMusicCache::GetSongsFromIds(const std::set<boost::uuids::uuid>& ids) const {
  SongCollection results;

  auto localSongs = GetLocalSongs();
  ARC_ASSERT_MSG(localSongs != nullptr, "Local songs returned a null ptr! This should never happen!");

  if (localSongs != nullptr) {
    for (auto& id : ids) {
      if (localSongs->find(id) != localSongs->end()) {
        results[id] = localSongs->at(id);
      }
    }
  }

  return results;
}

void Arcusical::LocalMusicStore::LocalMusicCache::RemoveFromCache(const std::vector<Model::Album>& albums) {
  WriteLock lock(&m_albumsEditLock);

  for (auto& album : albums) {
    m_localAlbums.erase(album.GetId());
  }
}

void Arcusical::LocalMusicStore::LocalMusicCache::RemoveFromCache(const std::vector<Model::Song>& songs) {
  WriteLock lock(&m_songsEditLock);

  for (auto& song : songs) {
    m_localSongs.erase(song.GetId());
  }
}