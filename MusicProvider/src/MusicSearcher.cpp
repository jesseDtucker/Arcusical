#include "MusicSearcher.hpp"

#include "boost/algorithm/string/predicate.hpp"
#include <algorithm>
#include <numeric>

#include "Arc_Assert.hpp"
#include "LocalMusicCache.hpp"

// the number of characters before searches in the middle of names will be considered
static const int MIN_FOR_MIDDLE_SEARCH = 3;

using namespace std;
using namespace Arcusical::LocalMusicStore;
using namespace Arcusical::Model;
using namespace Arcusical::MusicProvider;

// just a little helper to reduce code bloat
#define CHECK_CANCEL                            \
  \
if(result.CancellationToken->IsCanceled()) \
{ \
    return;                                     \
  \
}

SearchResult::SearchResult(Util::CancellationTokenRef token) : CancellationToken(token) {}

bool SearchResult::operator==(const SearchResult& rhs) const {
  return this->Songs == rhs.Songs && this->Albums == rhs.Albums;
}

bool SearchResult::operator!=(const SearchResult& rhs) const { return !(*this == rhs); }

MusicSearcher::MusicSearcher(LocalMusicCache* musicCache) {
  ARC_ASSERT(musicCache != nullptr);
  m_musicCache = musicCache;
}

void FindSongs(const wstring& searchTerm, SearchResult& result, LocalMusicCache& cache) {
  std::vector<const Song*> songsThatStartWith;
  std::vector<const Song*> songsThatContain;

  auto termLength = searchTerm.size();

  ARC_ASSERT_MSG(termLength > 0, "No point searching for an empty string!");

  // search song titles
  auto songs = cache.GetLocalSongs();
  for (auto& songPair : *songs) {
    CHECK_CANCEL;

    auto& song = songPair.second;
    auto& title = song.GetTitle();

    if (boost::istarts_with(title, searchTerm)) {
      songsThatStartWith.push_back(&song);
    } else if (termLength >= MIN_FOR_MIDDLE_SEARCH && boost::icontains(title, searchTerm)) {
      songsThatContain.push_back(&song);
    }
  }

  // now that we have the pointers insert the data into the results object
  // we copy the song data over
  result.Songs.resize(songsThatStartWith.size() + songsThatContain.size());
  auto next = begin(result.Songs);
  for (auto& songList : {&songsThatStartWith, &songsThatContain}) {
    CHECK_CANCEL;

    next = transform(begin(*songList), end(*songList), next, [](const Song* song) { return *song; });
  }
  ARC_ASSERT(next == end(result.Songs));
}

void FindAlbums(const wstring& searchTerm, SearchResult& result, LocalMusicCache& cache) {
  std::vector<const Album*> albumsThatStartWith;
  std::vector<const Album*> albumsThatContain;
  std::vector<const Album*> startsWithAlbumArtist;
  std::vector<const Album*> containsAlbumArtist;

  auto termLength = searchTerm.size();

  auto albums = cache.GetLocalAlbums();
  for (auto& albumPair : *albums) {
    CHECK_CANCEL;

    auto& album = albumPair.second;
    auto& title = album.GetTitle();
    auto& artist = album.GetArtist();

    if (boost::istarts_with(title, searchTerm)) {
      albumsThatStartWith.push_back(&album);
    } else if (termLength >= MIN_FOR_MIDDLE_SEARCH && boost::icontains(title, searchTerm)) {
      albumsThatContain.push_back(&album);
    } else if (boost::istarts_with(artist, searchTerm)) {
      startsWithAlbumArtist.push_back(&album);
    } else if (termLength >= MIN_FOR_MIDDLE_SEARCH && boost::icontains(artist, searchTerm)) {
      containsAlbumArtist.push_back(&album);
    }
  }

  auto foundAlbums = {albumsThatContain, albumsThatStartWith, startsWithAlbumArtist, containsAlbumArtist};
  auto albumCount = accumulate(begin(foundAlbums), end(foundAlbums), size_t(0),
                               [](size_t acc, decltype(albumsThatContain) albums) { return acc + albums.size(); });

  // no default constructor available nor do I plan to add one, create a bunch of invalid albums
  result.Albums.resize(albumCount, {nullptr});
  auto next = begin(result.Albums);
  for (auto& albumList : foundAlbums) {
    CHECK_CANCEL;

    next = transform(begin(albumList), end(albumList), next, [](const Album* album) { return *album; });
  }
  ARC_ASSERT(next == end(result.Albums));
}

SearchResult MusicSearcher::Find(wstring searchTerm, Util::CancellationTokenRef cancellationToken) {
  SearchResult result{cancellationToken};

  FindSongs(searchTerm, result, *m_musicCache);
  FindAlbums(searchTerm, result, *m_musicCache);

  return result;
}