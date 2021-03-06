#pragma once

#include <future>
#include <vector>

#include "CancellationToken.hpp"

namespace Arcusical {
namespace LocalMusicStore {
class LocalMusicCache;
}
namespace Model {
class Song;
class Album;
}
}

namespace Arcusical {
namespace MusicProvider {
struct SearchResult {
  SearchResult(Util::CancellationTokenRef token);
  bool operator==(const SearchResult& rhs) const;
  bool operator!=(const SearchResult& rhs) const;

  std::vector<Model::Song> Songs;
  std::vector<Model::Album> Albums;
  Util::CancellationTokenRef CancellationToken = nullptr;
};

class MusicSearcher final {
 public:
  MusicSearcher(LocalMusicStore::LocalMusicCache* musicCache);
  SearchResult Find(std::wstring searchTerm, Util::CancellationTokenRef cancellationToken);

 private:
  LocalMusicStore::LocalMusicCache* m_musicCache;
};
} /* Music Provider */
} /* Arcusical */
