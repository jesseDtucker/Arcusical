#include <algorithm>
#include <random>

#include "LocalMusicCache.hpp"
#include "SongSelector.hpp"
using namespace std;

namespace Arcusical {
namespace MusicProvider {
SongSelector::SongSelector(LocalMusicStore::LocalMusicCache* cache) : m_cache(cache) {}

vector<Model::Song> SongSelector::GetRandomSongs(int count, function<bool(const Model::Song&)> filter) {
  auto songs = m_cache->GetLocalSongs();
  vector<Model::Song> randomizedSongs;
  vector<Model::Song> results;
  for (auto& song : *songs) {
    randomizedSongs.push_back(song.second);
  }

  random_device rd;
  shuffle(begin(randomizedSongs), end(randomizedSongs), default_random_engine(rd()));

  results.resize(count);
  auto newEnd =
      copy_if(begin(randomizedSongs), end(randomizedSongs), begin(results), [&count, &filter](const Model::Song& song) {
        auto result = count > 0 && filter(song);
        if (result) {
          --count;
        }
        return result;
      });
  results.resize(distance(begin(results), newEnd));

  return results;
}

vector<Model::Song> SongSelector::GetFromSameAlbum(const Model::Song& song, function<bool(const Model::Song&)> filter) {
  vector<Model::Song> result;
  const Model::Album* album = nullptr;

  // scope needed because albums is a locked resource, free the lock as soon as possible
  {
    auto albums = m_cache->GetLocalAlbums();

    for (auto& albumIdPair : *albums) {
      if (albumIdPair.second.GetTitle() == song.GetAlbumName()) {
        album = &albumIdPair.second;
        break;
      }
    }
  }

  if (album != nullptr) {
    for (auto& songs : *album->GetSongs()) {
      if (filter(songs)) {
        result.push_back(songs);
      }
    }
  }

  return result;
}

vector<Model::Song> SongSelector::GetFromSameArtist(const Model::Song& targetSong,
                                                    function<bool(const Model::Song&)> filter) {
  auto songs = m_cache->GetLocalSongs();
  vector<Model::Song> result;

  for (auto& song : *songs) {
    if (song.second.GetArtist() == targetSong.GetArtist()) {
      if (filter(song.second)) {
        result.push_back(song.second);
      }
    }
  }

  return result;
}
}
}