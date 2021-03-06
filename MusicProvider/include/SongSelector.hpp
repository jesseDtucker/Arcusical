#pragma once

#include <functional>
#include <vector>

namespace Arcusical {
namespace LocalMusicStore {
class LocalMusicCache;
};
namespace Model {
class Song;
};

namespace MusicProvider {
//////////////////////////////////////////////////////////////////////////
//
// Class that takes requests and selects a set of songs based upon those requests.
// For instance it can select random songs, songs from an album or by an artist.
//
//////////////////////////////////////////////////////////////////////////
class SongSelector final {
 public:
  SongSelector(LocalMusicStore::LocalMusicCache* cache);

  std::vector<Model::Song> GetRandomSongs(int count, std::function<bool(const Model::Song&)> = nullptr);
  std::vector<Model::Song> GetFromSameAlbum(const Model::Song& song, std::function<bool(const Model::Song&)> = nullptr);
  std::vector<Model::Song> GetFromSameArtist(const Model::Song& song,
                                             std::function<bool(const Model::Song&)> = nullptr);

 private:
  LocalMusicStore::LocalMusicCache* m_cache;
};
}
}
