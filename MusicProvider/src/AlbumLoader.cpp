#include "AlbumLoader.hpp"

#include "boost/algorithm/string/predicate.hpp"
#include "boost/functional/hash.hpp"
#include "boost/uuid/uuid.hpp"
#include <algorithm>
#include <string>
#include <unordered_map>

#include "Arc_Assert.hpp"
#include "MusicTypes.hpp"
#include "UUIDGenerator.hpp"

using std::locale;
using std::pair;
using std::unordered_map;
using boost::uuids::uuid;
using std::vector;
using std::wstring;

using namespace Arcusical;
using namespace Arcusical::Model;
using namespace Arcusical::MusicProvider;

// used to make album comparison case insensitive while in the lookup map
struct iequal_to {
  bool operator()(const wstring& a, const wstring& b) const { return boost::algorithm::iequals(a, b, locale()); }
};

struct ihash {
  size_t operator()(wstring const& a) const {
    size_t seed = 0;
    locale locale;

    for (auto& c : a) {
      boost::hash_combine(seed, std::toupper(c, locale));
    }

    return seed;
  }
};

typedef unordered_map<wstring, vector<const Song*>, ihash, iequal_to> SongLookupByAlbumName;
typedef unordered_map<wstring, const Album*, ihash, iequal_to> AlbumLookupByAlbumName;

static Album CreateAlbum(const wstring& name, const vector<const Song*>& songs, const IAlbumToSongMapper* mapper);
static vector<const Song*> SongsWithoutAlbums(const AlbumCollection& existingAlbums, const SongCollection& songs);
static SongLookupByAlbumName GroupSongsByAlbumTitle(vector<const Song*> songs);
static AlbumLookupByAlbumName CreateAlbumLookup(const AlbumCollection& albums);
static vector<Album> CreateNewAlbums(AlbumLookupByAlbumName albums, SongLookupByAlbumName songGroups,
                                     const IAlbumToSongMapper* mapper);
static vector<Album> CreateModifiedAlbums(AlbumLookupByAlbumName albums, SongLookupByAlbumName songGroups);

static vector<pair<uuid, const Album*>> DetermineMissingSongs(const AlbumCollection& existingAlbums,
                                                              const SongCollection& songs);
static vector<pair<const Album*, vector<uuid>>> GroupDeletedAlbums(
    const vector<pair<uuid, const Album*>>& missingSongs);
static vector<Album> GetDeletedAlbums(const vector<pair<const Album*, vector<uuid>>>& groups);
static vector<Album> GetPartiallyDeletedAlbums(const vector<pair<const Album*, vector<uuid>>>& groups);

static wstring SelectMostCommonAlbumArtist(const vector<const Song*>& songs);

static Util::UUIDGenerator s_idGenerator;

AlbumMergeResult MusicProvider::MergeAlbums(const AlbumCollection& existingAlbums, const SongCollection& songs,
                                            const Model::IAlbumToSongMapper* mapper) {
  AlbumMergeResult result;

  auto newSongs = SongsWithoutAlbums(existingAlbums, songs);
  auto groups = GroupSongsByAlbumTitle(newSongs);
  auto albumLookup = CreateAlbumLookup(existingAlbums);
  result.newAlbums = CreateNewAlbums(albumLookup, groups, mapper);
  result.modifiedAlbums = CreateModifiedAlbums(albumLookup, groups);

  ARC_ASSERT(result.newAlbums.size() + result.modifiedAlbums.size() == groups.size());

  return result;
}

AlbumMergeResult MusicProvider::FindDeletedAlbums(const AlbumCollection& existingAlbums, const SongCollection& songs) {
  AlbumMergeResult result;

  auto missingSongs = DetermineMissingSongs(existingAlbums, songs);
  auto groups = GroupDeletedAlbums(missingSongs);
  result.deletedAlbums = GetDeletedAlbums(groups);
  result.modifiedAlbums = GetPartiallyDeletedAlbums(groups);

  ARC_ASSERT(result.deletedAlbums.size() + result.modifiedAlbums.size() == groups.size());

  return result;
}

Album CreateAlbum(const wstring& name, const vector<const Song*>& songs, const IAlbumToSongMapper* mapper) {
  using namespace boost::uuids;

  ARC_ASSERT(songs.size() > 0);

  auto newAlbum = Album(mapper);

  vector<uuid> songIds;
  transform(begin(songs), end(songs), back_inserter(songIds), [](const Model::Song* song) { return song->GetId(); });

  newAlbum.SetId(s_idGenerator());
  newAlbum.SetArtist(SelectMostCommonAlbumArtist(songs));
  newAlbum.GetMutableSongIds().insert(begin(songIds), end(songIds));
  newAlbum.SetTitle(name);

  return newAlbum;
}

vector<const Song*> SongsWithoutAlbums(const AlbumCollection& existingAlbums, const SongCollection& songs) {
  vector<uuid> allNewSongIds;    // all of the ids for all songs
  vector<uuid> allAlbumSongIds;  // all song ids that are referenced by an album

  allNewSongIds.reserve(songs.size());
  allAlbumSongIds.reserve(songs.size());

  for (auto& song : songs) {
    allNewSongIds.push_back(song.first);
  }
  for (auto& album : existingAlbums) {
    auto& ids = album.second.GetSongIds();
    allAlbumSongIds.insert(end(allAlbumSongIds), begin(ids), end(ids));
  }

  sort(begin(allNewSongIds), end(allNewSongIds));
  sort(begin(allAlbumSongIds), end(allAlbumSongIds));

  // now take the difference of the two sets, whatever is left are songs that are not referenced in an album
  vector<uuid> songIdsWithoutAlbums;

  set_difference(begin(allNewSongIds), end(allNewSongIds), begin(allAlbumSongIds), end(allAlbumSongIds),
                 back_inserter(songIdsWithoutAlbums));

  vector<const Song*> results;
  results.reserve(songIdsWithoutAlbums.size());

  transform(begin(songIdsWithoutAlbums), end(songIdsWithoutAlbums), back_inserter(results),
            [&songs](const uuid& id) { return &songs.at(id); });

  return results;
}

SongLookupByAlbumName GroupSongsByAlbumTitle(vector<const Song*> songs) {
  SongLookupByAlbumName result;

  for (auto song : songs) {
    result[song->GetAlbumName()].push_back(song);
  }

  return result;
}

AlbumLookupByAlbumName CreateAlbumLookup(const AlbumCollection& albums) {
  AlbumLookupByAlbumName result;
  result.reserve(albums.size());

  transform(begin(albums), end(albums), inserter(result, end(result)),
            [](const AlbumCollection::value_type& album) -> pair<wstring, const Album*> {
              return {album.second.GetTitle(), &album.second};
            });

  return result;
}

vector<Album> CreateNewAlbums(AlbumLookupByAlbumName albums, SongLookupByAlbumName songGroups,
                              const IAlbumToSongMapper* mapper) {
  // any groups in songs that are not in albums are new albums
  vector<Album> result;

  for (auto& songGroup : songGroups) {
    // make sure this isn't an existing album
    if (albums.find(songGroup.first) == end(albums)) {
      result.push_back(CreateAlbum(songGroup.first, songGroup.second, mapper));
    }
  }

  return result;
}

vector<Album> CreateModifiedAlbums(AlbumLookupByAlbumName albums, SongLookupByAlbumName songGroups) {
  vector<Album> result;

  for (auto& songGroup : songGroups) {
    // make sure this isn't an existing album
    auto albumItr = albums.find(songGroup.first);
    if (albumItr != end(albums)) {
      Album existingAlbum = *albumItr->second;
      vector<uuid> songIds;

      transform(begin(songGroup.second), end(songGroup.second), back_inserter(songIds),
                [](const Song* song) { return song->GetId(); });

      existingAlbum.GetMutableSongIds().insert(begin(songIds), end(songIds));
      result.push_back(existingAlbum);
    }
  }

  return result;
}

vector<pair<uuid, const Album*>> DetermineMissingSongs(const AlbumCollection& existingAlbums,
                                                       const SongCollection& songs) {
  vector<pair<uuid, const Album*>> allAlbumSongIds;
  vector<pair<uuid, const Album*>> allSongIds;
  allAlbumSongIds.reserve(songs.size());
  allSongIds.reserve(songs.size());

  for (auto& album : existingAlbums) {
    auto ids = album.second.GetSongIds();
    transform(begin(ids), end(ids), back_inserter(allAlbumSongIds),
              [&album](const uuid& id) -> pair<uuid, const Album*> {
                return {id, &album.second};
              });
  }

  transform(begin(songs), end(songs), back_inserter(allSongIds),
            [](const SongCollection::value_type& song) -> pair<uuid, const Album*> {
              return {song.first, nullptr};
            });

  auto sortPred = [](const pair<uuid, const Album*>& a, const pair<uuid, const Album*>& b) {
    return a.first < b.first;
  };

  sort(begin(allAlbumSongIds), end(allAlbumSongIds), sortPred);
  sort(begin(allSongIds), end(allSongIds), sortPred);

  vector<pair<uuid, const Album*>> results;
  set_difference(begin(allAlbumSongIds), end(allAlbumSongIds), begin(allSongIds), end(allSongIds),
                 back_inserter(results), sortPred);

  return results;
}

vector<pair<const Album*, vector<uuid>>> GroupDeletedAlbums(const vector<pair<uuid, const Album*>>& missingSongs) {
  unordered_map<const Album*, vector<uuid>> groups;
  groups.reserve(missingSongs.size());  // over sized, but better than re allocations

  for (auto& pair : missingSongs) {
    groups[pair.second].push_back(pair.first);
  }

  vector<pair<const Album*, vector<uuid>>> results;
  results.reserve(groups.size());
  copy(begin(groups), end(groups), back_inserter(results));

  return results;
}

vector<Album> GetDeletedAlbums(const vector<pair<const Album*, vector<uuid>>>& groups) {
  vector<pair<const Album*, vector<uuid>>> deleted;
  copy_if(begin(groups), end(groups), back_inserter(deleted), [](const pair<const Album*, vector<uuid>>& pair) {
    // if all songs have been removed the album has been deleted
    return pair.first->GetSongIds().size() == pair.second.size();
  });

  vector<Album> results;
  results.reserve(deleted.size());

  transform(begin(deleted), end(deleted), back_inserter(results), [](const pair<const Album*, vector<uuid>>& pair) {
    auto album = *pair.first;
    album.GetMutableSongIds().clear();
    return album;
  });

  return results;
}

vector<Album> GetPartiallyDeletedAlbums(const vector<pair<const Album*, vector<uuid>>>& groups) {
  vector<pair<const Album*, vector<uuid>>> modified;
  copy_if(begin(groups), end(groups), back_inserter(modified), [](const pair<const Album*, vector<uuid>>& pair) {
    // if all songs have been removed the album has been deleted
    return pair.first->GetSongIds().size() != pair.second.size();
  });

  vector<Album> results;
  results.reserve(modified.size());

  transform(begin(modified), end(modified), back_inserter(results), [](const pair<const Album*, vector<uuid>>& pair) {
    auto album = *pair.first;
    for (auto& id : pair.second) {
      album.GetMutableSongIds().erase(id);
    }
    return album;
  });

  return results;
}

wstring SelectMostCommonAlbumArtist(const vector<const Song*>& songs) {
  ARC_ASSERT(songs.size() > 0);

  typedef unordered_map<wstring, int> countMap;
  countMap counter;
  for (auto& song : songs) {
    auto& artist = song->GetArtist();
    if (counter.find(artist) == end(counter)) {
      counter[artist] = 1;
    } else {
      ++counter[artist];
    }
  }

  auto m = max_element(begin(counter), end(counter), [](const countMap::value_type& a, const countMap::value_type& b) {
    return a.second < b.second;
  });

  return m->first;
}