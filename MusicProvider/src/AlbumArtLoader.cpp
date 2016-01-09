#include "boost/algorithm/string/predicate.hpp"
#include <functional>
#include <numeric>
#include <random>
#include <string>
#include <unordered_map>
#include <utility>

#include "AlbumArtLoader.hpp"
#include "DefaultAlbumArt.hpp"
#include "FilterProcessor.hpp"
#include "IFile.hpp"
#include "IFileReader.hpp"
#include "IFolder.hpp"
#include "ImageTypes.hpp"
#include "LocalMusicCache.hpp"
#include "MPEG4_Parser.hpp"
#include "MusicTypes.hpp"
#include "Storage.hpp"

using std::accumulate;
using std::any_of;
using std::begin;
using std::copy_if;
using std::default_random_engine;
using std::end;
using std::function;
using std::max_element;
using std::pair;
using std::random_device;
using std::shared_ptr;
using std::tuple;
using std::unordered_map;
using std::vector;
using std::wstring;

using namespace Arcusical::MusicProvider;
using namespace Arcusical::Model;
using namespace Arcusical::MPEG4;
using namespace FileSystem;
using namespace std::placeholders;
using namespace Util;

static const int MIN_EMBEDED_LOAD_BATCH_SIZE = 5;
static const int MAX_EMBEDED_LOAD_BATCH_SIZE = 20;
static const int MIN_DELAYED_LOAD_BATCH_SIZE = 5;
static const int MAX_DELAYED_LOAD_BATCH_SIZE = 10;
static const std::chrono::milliseconds EMBEDED_LOAD_INTERVAL{
    500};  // do not wait more than this amount of time before loading more

typedef boost::uuids::uuid SongId;

static vector<tuple<AlbumArtLoader::AlbumId, vector<SongId>>> GetSongIdsForAlbums(
    const vector<AlbumArtLoader::AlbumId>& ids, const AlbumCollection& albums);
static unordered_map<AlbumArtLoader::AlbumId, wstring, boost::hash<AlbumArtLoader::AlbumId>> BuildAlbumNameLookup(
    const vector<AlbumArtLoader::AlbumId>& ids, const AlbumCollection& albums);
static vector<tuple<AlbumArtLoader::AlbumId, vector<tuple<wstring, ContainerType>>>> GetPathsForSongIds(
    const vector<tuple<AlbumArtLoader::AlbumId, vector<SongId>>>& albumInfo, const SongCollection& songs);
static vector<tuple<wstring, ContainerType>> GetFilePaths(const Song& song);

static wstring LoadAlbumImage(const vector<tuple<wstring, ContainerType>>& songPaths, const wstring& albumName);
static boost::optional<wstring> LoadAlbumImage(const tuple<wstring, ContainerType>& songPathj,
                                               const wstring& albumName);
static boost::optional<tuple<vector<unsigned char>, ImageType>> GetThumbnail(const IFile& file,
                                                                             const ContainerType& container);
static boost::optional<tuple<vector<unsigned char>, ImageType>> LoadThumbnailFromMpeg4(const IFile& file);
static boost::optional<tuple<vector<unsigned char>, ImageType>> LoadThumbnailFromSystem(const IFile& file);
static wstring SaveImageFile(const std::wstring& existingArtPath);
static boost::optional<wstring> SaveImageFile(vector<unsigned char>& imgData, const wstring& albumName,
                                              ImageType imageType);
static boost::optional<wstring> SaveImageFile(vector<unsigned char>& imgData, const wstring& albumName,
                                              const wstring& imageExtension);

static vector<wstring> GetImagePaths(vector<FilePtr> files);
static wstring FindArt(const wstring& albumName, const vector<tuple<wstring, ContainerType>>& albumSongInfo,
                       const vector<wstring>& imagePaths);
static wstring FindArt(const vector<wstring>& imagePaths, const wstring& songPath, const wstring& albumName);
static wstring VoteOnResult(const vector<pair<wstring, wstring>>& selections,
                            const unordered_map<wstring, size_t>& pathWeights);

static size_t FindLastPathSeperator(const wstring& path);
static unordered_map<wstring, size_t> BuildCounts(const vector<wstring>& strings);

static bool CheckAlbum(const AlbumArtLoader::IdBoolPair&);
struct CheckAlbumPath {
  typedef const AlbumArtLoader::IdPathPair& argument_type;

  bool operator()(argument_type pair) const { return pair.second.size() > 0; }
};
static AlbumArtLoader::AlbumId StripId(const AlbumArtLoader::IdBoolPair&);
static AlbumArtLoader::AlbumId StripIdFromPath(const AlbumArtLoader::IdPathPair&);

Arcusical::MusicProvider::AlbumArtLoader::AlbumArtLoader(LocalMusicStore::LocalMusicCache* cache)
    : m_verifier(),
      m_verifyFilter(&CheckAlbum),
      m_verifyIdStripper(&StripId),
      m_embededLoader(MIN_EMBEDED_LOAD_BATCH_SIZE, MAX_EMBEDED_LOAD_BATCH_SIZE, EMBEDED_LOAD_INTERVAL),
      m_loadedFilter(CheckAlbumPath()),
      m_loadedIdStripper(&StripIdFromPath),
      m_delayedLoader(MIN_DELAYED_LOAD_BATCH_SIZE, MAX_DELAYED_LOAD_BATCH_SIZE, EMBEDED_LOAD_INTERVAL),
      m_delayedLoadedFilter(CheckAlbumPath()),
      m_recorder(),
      m_cache(cache) {
  m_recorder.SetBatchProcessor(std::bind(&AlbumArtLoader::RecordAlbumArt, this, _1));
  m_verifier.SetBatchProcessor(std::bind(&AlbumArtLoader::VerifyAlbums, this, _1));
  m_embededLoader.SetBatchProcessor(std::bind(&AlbumArtLoader::EmbededAlbumLoad, this, _1));
  m_delayedLoader.SetBatchProcessor(std::bind(&AlbumArtLoader::DelayedAlbumLoad, this, _1));

  m_verifier.Connect(&m_verifyFilter);
  m_verifyFilter.ConnectRejectBuffer(&m_verifyIdStripper);
  m_verifyIdStripper.ConnectBuffer(&m_embededLoader);
  m_embededLoader.Connect(&m_loadedFilter);
  m_loadedFilter.ConnectMatchingBuffer(&m_recorder);
  m_loadedFilter.ConnectRejectBuffer(&m_loadedIdStripper);
  m_loadedIdStripper.ConnectBuffer(&m_delayedLoader);
  m_delayedLoader.Connect(&m_delayedLoadedFilter);
  m_delayedLoadedFilter.ConnectMatchingBuffer(&m_recorder);

  m_verifier.Start();
  m_embededLoader.Start();

  m_imagePathFuture = std::async(std::launch::async, [this]() {
    return FileSystem::Storage::MusicFolder().FindFilesWithExtensions({L".bmp", L".jpg", L".jpeg", L".png", L".tiff"});
  });
}

Arcusical::MusicProvider::AlbumArtLoader::~AlbumArtLoader() {
  m_verifier.AwaitCompletion();
  m_embededLoader.AwaitCompletion();
  m_delayedLoader.AwaitCompletion();
  m_imagePathFuture.wait();
}

//
// Verification
//

std::vector<AlbumArtLoader::IdBoolPair> AlbumArtLoader::VerifyAlbums(
    const std::vector<AlbumArtLoader::AlbumId>& albumIds) {
  std::vector<AlbumArtLoader::IdBoolPair> results;
  results.reserve(albumIds.size());
  vector<tuple<AlbumArtLoader::AlbumId, wstring>> imageFilePaths;

  {
    auto albums = m_cache->GetLocalAlbums();
    for (auto& id : albumIds) {
      auto album = albums->find(id);
      if (album != end(*albums)) {
        auto filePath = album->second.GetImageFilePath();
        imageFilePaths.push_back(std::make_tuple(id, filePath));
      }
    }
  }

  for (auto& idPathPair : imageFilePaths) {
    const auto& id = std::get<0>(idPathPair);
    const auto& path = std::get<1>(idPathPair);
    bool isVerified = !isDefaultAlbumArt(path) || FileSystem::Storage::FileExists(path);
    results.push_back(std::make_pair(id, isVerified));
  }

  return results;
}

//
// Loading
//

vector<AlbumArtLoader::IdPathPair> AlbumArtLoader::EmbededAlbumLoad(const vector<AlbumArtLoader::AlbumId>& albumIds) {
  vector<tuple<AlbumArtLoader::AlbumId, vector<SongId>>> songIdsForAlbums;
  vector<tuple<AlbumArtLoader::AlbumId, vector<tuple<wstring, ContainerType>>>> songPathsForAlbums;
  vector<AlbumArtLoader::IdPathPair> albumsWithArt;
  unordered_map<AlbumArtLoader::AlbumId, wstring, boost::hash<AlbumArtLoader::AlbumId>> albumNameLookup;

  albumsWithArt.reserve(albumIds.size());

  {
    auto albums = m_cache->GetLocalAlbums();
    songIdsForAlbums = GetSongIdsForAlbums(albumIds, *albums);
    albumNameLookup = BuildAlbumNameLookup(albumIds, *albums);
  }
  {
    auto songs = m_cache->GetLocalSongs();
    songPathsForAlbums = GetPathsForSongIds(songIdsForAlbums, *songs);
  }

  transform(begin(songPathsForAlbums), end(songPathsForAlbums), back_inserter(albumsWithArt),
            [&albumNameLookup](const tuple<AlbumArtLoader::AlbumId, vector<tuple<wstring, ContainerType>>>& songPaths) {
              auto id = std::get<0>(songPaths);
              auto nameItr = albumNameLookup.find(id);
              if (nameItr != end(albumNameLookup)) {
                auto path = LoadAlbumImage(std::get<1>(songPaths), nameItr->second);
                return make_pair(id, path);
              } else {
                return make_pair(id, wstring());
              }
            });

  return albumsWithArt;
}

//
// Recording
//

std::vector<int> AlbumArtLoader::RecordAlbumArt(const std::vector<AlbumArtLoader::IdPathPair>& albumArt) {
  m_cache->GetLocalAlbumsMutable([&albumArt](Model::AlbumCollection* albums) {
    for (auto& result : albumArt) {
      auto& id = std::get<0>(result);
      auto& path = std::get<1>(result);
      auto albumItr = albums->find(id);

      if (albumItr != end(*albums) && path.size() > 0) {
        albumItr->second.SetImageFilePath(path);
      }
    }
  });

  m_cache->SaveAlbums();

  // Notify any listeners that album art has been loaded and is ready.
  vector<AlbumArtLoader::AlbumId> albumIdsLoaded;
  albumIdsLoaded.reserve(albumArt.size());
  transform(begin(albumArt), end(albumArt), back_inserter(albumIdsLoaded),
            [](IdPathPair res) { return std::get<0>(res); });

  OnArtLoaded(albumIdsLoaded);
  return std::vector<int>(albumArt.size());
}

//
// Delayed Loading
//

std::vector<AlbumArtLoader::IdPathPair> AlbumArtLoader::DelayedAlbumLoad(
    const std::vector<AlbumArtLoader::AlbumId>& batch) {
  // first try to load from the songs again. The load may have failed before because not all the songs
  // in an album had art and the ones that were processed initially did not contain art. I've sometimes noticed
  // only the first few songs in an album have art, but I do not define an order for loading so these may not be
  // loaded initially.
  auto embededLoadResults = EmbededAlbumLoad(batch);
  decltype(embededLoadResults) missingResults;
  decltype(embededLoadResults) foundArt;
  missingResults.reserve(embededLoadResults.size());
  foundArt.reserve(embededLoadResults.size());

  copy_if(begin(embededLoadResults), end(embededLoadResults), back_inserter(foundArt), CheckAlbumPath());
  copy_if(begin(embededLoadResults), end(embededLoadResults), back_inserter(missingResults),
          std::not1(CheckAlbumPath()));
  std::vector<AlbumArtLoader::AlbumId> idsToSearchFor;
  idsToSearchFor.reserve(missingResults.size());
  transform(begin(missingResults), end(missingResults), back_inserter(idsToSearchFor), &StripIdFromPath);

  vector<tuple<AlbumArtLoader::AlbumId, vector<SongId>>> songIdsForAlbums;
  vector<tuple<AlbumArtLoader::AlbumId, vector<tuple<wstring, ContainerType>>>> songPathsForAlbums;
  unordered_map<AlbumArtLoader::AlbumId, wstring, boost::hash<AlbumArtLoader::AlbumId>> albumNameLookup;

  {
    auto albums = m_cache->GetLocalAlbums();
    songIdsForAlbums = GetSongIdsForAlbums(idsToSearchFor, *albums);
    albumNameLookup = BuildAlbumNameLookup(idsToSearchFor, *albums);
  }
  {
    auto songs = m_cache->GetLocalSongs();
    songPathsForAlbums = GetPathsForSongIds(songIdsForAlbums, *songs);
  }

  vector<AlbumArtLoader::IdPathPair> artPaths;
  artPaths.reserve(songPathsForAlbums.size());
  transform(begin(songPathsForAlbums), end(songPathsForAlbums), back_inserter(artPaths),
            [this, &albumNameLookup](
                const tuple<AlbumArtLoader::AlbumId, vector<tuple<wstring, ContainerType>>>& songPathsForAlbum) {
              auto id = std::get<0>(songPathsForAlbum);
              auto albumSongInfo = std::get<1>(songPathsForAlbum);
              auto albumName = albumNameLookup.at(id);
              auto imagePath = FindArt(albumName, albumSongInfo, m_imagePaths);
              return make_pair(id, imagePath);
            });

  // now make sure to add in the ones that were loaded from embedded
  artPaths.reserve(artPaths.size() + foundArt.size());
  std::move(begin(foundArt), end(foundArt), back_inserter(artPaths));

  return artPaths;
}

//
// Helpers
//

vector<tuple<AlbumArtLoader::AlbumId, vector<SongId>>> GetSongIdsForAlbums(const vector<AlbumArtLoader::AlbumId>& ids,
                                                                           const AlbumCollection& albums) {
  vector<tuple<AlbumArtLoader::AlbumId, vector<SongId>>> songIdsForAlbums;
  songIdsForAlbums.reserve(albums.size());

  for (auto& id : ids) {
    auto albumItr = albums.find(id);
    if (albumItr != end(albums)) {
      auto songIds = albumItr->second.GetSongIds();
      songIdsForAlbums.push_back(std::make_tuple(id, vector<SongId>{begin(songIds), end(songIds)}));
    }
  }

  return songIdsForAlbums;
}

unordered_map<AlbumArtLoader::AlbumId, wstring, boost::hash<AlbumArtLoader::AlbumId>> BuildAlbumNameLookup(
    const vector<AlbumArtLoader::AlbumId>& ids, const AlbumCollection& albums) {
  unordered_map<AlbumArtLoader::AlbumId, wstring, boost::hash<AlbumArtLoader::AlbumId>> lookup;

  for (auto& id : ids) {
    auto albumItr = albums.find(id);
    if (albumItr != end(albums)) {
      lookup[id] = albumItr->second.GetTitle();
    }
  }

  return lookup;
}

vector<tuple<AlbumArtLoader::AlbumId, vector<tuple<wstring, ContainerType>>>> GetPathsForSongIds(
    const vector<tuple<AlbumArtLoader::AlbumId, vector<SongId>>>& albumInfo, const SongCollection& songs) {
  vector<tuple<AlbumArtLoader::AlbumId, vector<tuple<wstring, ContainerType>>>> songPathsForAlbums;
  songPathsForAlbums.reserve(albumInfo.size());
  transform(begin(albumInfo), end(albumInfo), back_inserter(songPathsForAlbums),
            [&songs](const tuple<AlbumArtLoader::AlbumId, vector<SongId>>& info) {
              auto& ids = std::get<1>(info);
              vector<tuple<wstring, ContainerType>> paths;
              paths.reserve(ids.size());
              for (auto& id : ids) {
                std::wstring songPath = L"";
                const auto& songItr = songs.find(id);
                // we have to check if the song still exists. It's possible it was deleted while we were doing
                // other work.
                if (songItr != end(songs)) {
                  auto songPaths = GetFilePaths(songItr->second);
                  std::move(begin(songPaths), end(songPaths), back_inserter(paths));
                }
              }

              return make_tuple(std::get<0>(info), paths);
            });

  return songPathsForAlbums;
}

wstring LoadAlbumImage(const vector<tuple<wstring, ContainerType>>& songPaths, const wstring& albumName) {
  wstring path;
  for (auto& songPath : songPaths) {
    auto imagePath = LoadAlbumImage(songPath, albumName);
    if (imagePath != boost::none) {
      path = *imagePath;
      break;
    }
  }

  return path;
}

boost::optional<wstring> LoadAlbumImage(const tuple<wstring, ContainerType>& songPath, const std::wstring& albumName) {
  boost::optional<wstring> imagePath = boost::none;

  auto& path = std::get<0>(songPath);
  auto& container = std::get<1>(songPath);
  auto songFile = FileSystem::Storage::LoadFileFromPath(path);
  ARC_ASSERT(songFile != nullptr);
  if (songFile != nullptr) {
    auto thumbnail = GetThumbnail(*songFile, container);
    if (thumbnail != boost::none) {
      imagePath = SaveImageFile(std::get<0>(*thumbnail), albumName, std::get<1>(*thumbnail));
    }
  }

  return imagePath;
}

vector<wstring> GetImagePaths(vector<FilePtr> imageFiles) {
  vector<wstring> imagePaths;
  imagePaths.reserve(imageFiles.size());
  transform(begin(imageFiles), end(imageFiles), back_inserter(imagePaths),
            [](const FilePtr& file) { return file->GetFullPath(); });

  return imagePaths;
}

wstring GetOnlyPathToFile(const wstring& fullPath) {
  auto pathEnd = FindLastPathSeperator(fullPath);
  if (pathEnd != 0) {
    // we want to try and keep the separator on the end, hence the +1
    return wstring(begin(fullPath), next(begin(fullPath), pathEnd + 1));
  } else {
    return fullPath;
  }
}

wstring FindArt(const wstring& albumName, const vector<tuple<wstring, ContainerType>>& albumSongInfo,
                const vector<wstring>& imagePaths) {
  if (imagePaths.size() == 0) {
    return L"";
  }
  // this is gonna get messy...
  // will find art for a song by simply:
  // 1) determining the number of identical characters in the song path and art path
  // 2) determine the average number of characters in common
  // 3) filter out any image paths that are less than or equal to the average
  // 4) select the max length of what remains, if more than one select all max
  // 5a) select any with the album name in their title, if at least 1 go to 6
  // 5b) select any with the word 'cover' in their title, if at least 1 go to 6
  // 5c) select any with the words 'art', 'album', 'front' or 'back', if at least 1 go to 6
  // 5d) select them all
  // 6) randomly select one of the available options produced by step 5s selection
  //
  // The above steps will get the art for a single song. The kicker is we have no guarantee that all the songs
  // are in the same folder and will give the same result. As such the above procedure will be repeated for each
  // song. Yes this will be slow and has a fair bit of time complexity. However, this process should run in the
  // background
  // and only for a handful of songs so I'm not too worried about this operation being a tad pricey. To improve the
  // efficiency
  // songs in the same folder will be grouped together as they should all produce the same result anyways.
  //
  // Once each song has some art selected aggregate them and select the most common song file. If a tie choose randomly

  // first break the albumSongInfo down into only paths
  vector<wstring> songPaths;
  songPaths.reserve(albumSongInfo.size());

  transform(begin(albumSongInfo), end(albumSongInfo), back_inserter(songPaths),
            [](const tuple<wstring, ContainerType>& info) {
              auto fullPath = std::get<0>(info);
              return GetOnlyPathToFile(fullPath);
            });

  // now we only really care about searching for the unique folders, no point doing the work twice.
  // However, because of the voting mechanism the number of songs in each location must be maintained
  // otherwise a single song in another directory may be enough to outvote many other songs located
  // in the album directory. While this is unlikely I would rather the system was fairly comprehensive
  // in its search capabilities and behaved in a 'manner of least surprise' for a typical end user.

  auto pathWeights = BuildCounts(songPaths);
  decltype(songPaths) uniqueSongPaths;
  uniqueSongPaths.reserve(pathWeights.size());
  transform(begin(pathWeights), end(pathWeights), back_inserter(uniqueSongPaths),
            [](const unordered_map<wstring, size_t>::value_type& pair) { return pair.first; });

  vector<pair<wstring, wstring>> selections;
  selections.reserve(uniqueSongPaths.size());
  transform(begin(uniqueSongPaths), end(uniqueSongPaths), back_inserter(selections),
            [&imagePaths, &albumName](const wstring& songPath) {
              auto imagePath = FindArt(imagePaths, songPath, albumName);
              return make_pair(songPath, imagePath);
            });

  auto imgPath = VoteOnResult(selections, pathWeights);
  return SaveImageFile(imgPath);
}

wstring VoteOnResult(const vector<pair<wstring, wstring>>& selections,
                     const unordered_map<wstring, size_t>& pathWeights) {
  // now convert the selections to votes taking into account the weights of each path
  vector<pair<size_t, wstring>> votes;
  votes.reserve(selections.size());
  transform(begin(selections), end(selections), back_inserter(votes),
            [&pathWeights](const pair<wstring, wstring>& selection) {
              ARC_ASSERT(end(pathWeights) != pathWeights.find(selection.first));
              auto weight = pathWeights.at(selection.first);
              return make_pair(weight, selection.second);
            });

  // finally it is possible that an image was selected twice by two different paths, its votes should be merged together
  unordered_map<wstring, size_t> uniqueVotes;
  for (auto& vote : votes) {
    if (vote.second.size() > 0)  // don't vote on empty paths
    {
      auto itr = uniqueVotes.find(vote.second);
      if (itr == end(uniqueVotes)) {
        uniqueVotes[vote.second] = vote.first;
      } else {
        uniqueVotes[vote.second] += vote.first;
      }
    }
  }

  if (uniqueVotes.size() > 0) {
    auto maxItr = max_element(begin(uniqueVotes), end(uniqueVotes),
                              [](const unordered_map<wstring, size_t>::value_type& a,
                                 const unordered_map<wstring, size_t>::value_type& b) { return a.second < b.second; });

    return maxItr->first;
  } else {
    // there are no options
    return L"";
  }
}

size_t NumberOfCommonAncestors(const wstring& a, const wstring& b) {
  // requiring that a is the shorter string
  if (a.size() > b.size()) {
    return NumberOfCommonAncestors(b, a);
  }

  auto aItr = begin(a);
  auto bItr = begin(b);

  size_t commonCount = 0;
  while (aItr != end(a) && *aItr == *bItr) {
    ARC_ASSERT(bItr != end(b));  // a should be shorter, so we can't run over b. assert just in case

    // basically we only care about the depth matching, not the actual length of the path that is common
    if (*aItr == '\\' || *aItr == '/') {
      ++commonCount;
    }

    ++aItr;
    ++bItr;
  }

  return commonCount;
}

wstring SelectCandidate(const wstring& albumName, const vector<wstring>& fullImagePaths) {
  // this is a series of selection passes
  // first pass is for any that have the album name is the title of the file
  // second pass is for any with the word 'cover' or 'art' 'album' in the title
  // third pass is for any with the word 'front' or 'back' in the title
  // final pass selects them all

  // given that we only care about the titles at this point lets first strip the rest of the path out
  ARC_ASSERT(fullImagePaths.size() > 0);
  vector<pair<wstring, const wstring*>> imagePaths;
  imagePaths.reserve(fullImagePaths.size());
  transform(begin(fullImagePaths), end(fullImagePaths), back_inserter(imagePaths), [](const wstring& fullPath) {
    auto nameStart = FindLastPathSeperator(fullPath);
    auto name = wstring(std::next(begin(fullPath), nameStart + 1), end(fullPath));
    return make_pair(name, &fullPath);
  });

  vector<vector<wstring>> namesToCheck = {{albumName},        {L"front"}, {L"cover", L"albumart"},
                                          {L"album", L"art"}, {L"inlay"}, {L"back", L"rear"}};

  vector<pair<wstring, const wstring*>> possiblePaths;
  for (auto& keywords : namesToCheck) {
    copy_if(begin(imagePaths), end(imagePaths), back_inserter(possiblePaths),
            [&keywords](const pair<wstring, const wstring*>& title) {
              return any_of(begin(keywords), end(keywords),
                            [&title](const wstring& keyword) { return boost::icontains(title.first, keyword); });
            });

    if (possiblePaths.size() > 0) {
      break;
    }
  }

  const wstring* selection = nullptr;
  static random_device rd;
  static default_random_engine rand(rd());

  auto r = rand();

  if (possiblePaths.size() == 0) {
    // no possible paths, just pick any path at random
    auto offset = r % imagePaths.size();
    selection = imagePaths[offset].second;
  } else {
    auto offset = r % possiblePaths.size();
    selection = possiblePaths[offset].second;
  }

  ARC_ASSERT(selection != nullptr);
  ARC_ASSERT(selection->size() != 0);
  ARC_ASSERT(FileSystem::Storage::FileExists(*selection));

  return *selection;
}

wstring FindArt(const vector<wstring>& imagePaths, const wstring& songPath, const wstring& albumName) {
  wstring selectedImagePath;
  ARC_ASSERT(imagePaths.size() > 0);

  // Note see the other FindArt function for details of the process, makes more sense to read all pieces in one go to
  // understand the process
  vector<pair<wstring, size_t>> imagePathsWithLengths;
  imagePathsWithLengths.reserve(imagePaths.size());
  transform(begin(imagePaths), end(imagePaths), back_inserter(imagePathsWithLengths), [&songPath](const wstring& path) {
    auto length = NumberOfCommonAncestors(path, songPath);
    return make_pair(path, length);
  });

  size_t total = accumulate(begin(imagePathsWithLengths), end(imagePathsWithLengths), size_t(0),
                            [](const size_t& c, const pair<wstring, size_t>& p) { return c + p.second; });

  // rounding matters!
  auto avg = (float)(total) / (float)(imagePathsWithLengths.size());

  vector<pair<wstring, size_t>> candidatePairs;
  copy_if(begin(imagePathsWithLengths), end(imagePathsWithLengths), back_inserter(candidatePairs),
          [&avg](const pair<wstring, size_t> pair) { return pair.second > avg; });

  if (candidatePairs.size() > 0) {
    auto max =
        max_element(begin(candidatePairs), end(candidatePairs),
                    [](const pair<wstring, size_t>& a, const pair<wstring, size_t>& b) { return a.second < b.second; });
    ARC_ASSERT(max != end(candidatePairs));

    auto last = remove_if(begin(candidatePairs), end(candidatePairs),
                          [&max](const pair<wstring, size_t>& pair) { return pair.second != max->second; });
    candidatePairs.erase(last, end(candidatePairs));
    vector<wstring> candidates;
    candidates.reserve(candidatePairs.size());
    ARC_ASSERT(candidatePairs.size() > 0);
    transform(begin(candidatePairs), end(candidatePairs), back_inserter(candidates),
              [](const pair<wstring, size_t>& pair) { return pair.first; });

    selectedImagePath = SelectCandidate(albumName, candidates);
  }

  return selectedImagePath;
}

InputBuffer<AlbumArtLoader::AlbumId>* AlbumArtLoader::AlbumsNeedingArt() { return &m_embededLoader; }

InputBuffer<AlbumArtLoader::AlbumId>* AlbumArtLoader::AlbumsToVerify() { return &m_verifier; }

void AlbumArtLoader::NotifyLoadingComplete() {
  auto imageFiles = m_imagePathFuture.get();
  m_imagePaths = GetImagePaths(imageFiles->GetAll());
  m_delayedLoader.Start();
  m_verifier.Complete();
  m_embededLoader.Complete();
}

vector<tuple<wstring, ContainerType>> GetFilePaths(const Song& song) {
  vector<tuple<wstring, ContainerType>> paths;
  auto& filesWithFormats = song.GetFiles();
  paths.reserve(filesWithFormats.size());
  for (auto& format : filesWithFormats) {
    for (auto& songFile : format.second) {
      paths.push_back(std::make_tuple(songFile.filePath, songFile.container));
    }
  }

  return paths;
}

boost::optional<tuple<vector<unsigned char>, ImageType>> GetThumbnail(const IFile& file,
                                                                      const ContainerType& container) {
  boost::optional<tuple<vector<unsigned char>, ImageType>> thumb = boost::none;

  switch (container) {
    case ContainerType::MP4:
      thumb = LoadThumbnailFromMpeg4(file);
      break;
    default:
      // fall back to system if we can't get it ourselves
      thumb = LoadThumbnailFromSystem(file);
  }

  return thumb;
}

boost::optional<tuple<vector<unsigned char>, ImageType>> LoadThumbnailFromMpeg4(const IFile& file) {
  boost::optional<tuple<vector<unsigned char>, ImageType>> thumb = boost::none;

  MPEG4_Parser mpegParser;
  auto reader = Storage::GetReader(&file);
  auto mpegTree = mpegParser.ReadAndParseFromStream(*reader);

  auto imageData = mpegTree->GetImageData();
  if (imageData != nullptr && imageData->size() > 0) {
    auto imageType = mpegTree->GetImageType();
    thumb = tuple<vector<unsigned char>, ImageType>(std::move(*imageData), imageType);
  }

  return thumb;
}

boost::optional<tuple<vector<unsigned char>, ImageType>> LoadThumbnailFromSystem(const IFile& file) {
  auto thumbnail = file.GetThumbnail(
      false);  // hopefully the thumbnail is the album art, probably won't work well for .wav or .flac...
  if (thumbnail.size() > 0) {
    return make_tuple(thumbnail, ImageType::BMP);
  } else {
    return boost::none;
  }
}

boost::optional<wstring> SaveImageFile(vector<unsigned char>& imgData, const wstring& albumName, ImageType imageType) {
  auto ext = ImageTypeToWString.at(imageType);
  return SaveImageFile(imgData, albumName, ext);
}

boost::optional<wstring> SaveImageFile(vector<unsigned char>& imgData, const wstring& albumName,
                                       const wstring& imageExtension) {
  using namespace FileSystem;

  auto fileName = albumName + L"." + imageExtension;
  Storage::RemoveIllegalCharacters(fileName);
  replace(begin(fileName), end(fileName), '\\', '_');
  replace(begin(fileName), end(fileName), '/', '_');
  fileName = L"albumImgs\\" + fileName;

  auto imgFile = Storage::ApplicationFolder().CreateNewFile(fileName);

  if (nullptr != imgFile) {
    imgFile->WriteToFile(imgData);
    return imgFile->GetFullPath();
  } else {
    return boost::none;
  }
}

wstring SaveImageFile(const std::wstring& existingArtPath) {
  if (existingArtPath.size() == 0) {
    return existingArtPath;
  }

  // For some reason we can't load the image in its default location, move to local folder first
  auto hash = std::hash<wstring>()(existingArtPath);
  auto albumArtFile = FileSystem::Storage::LoadFileFromPath(existingArtPath);
  vector<unsigned char> buffer;
  albumArtFile->ReadFromFile(buffer);
  auto ext = albumArtFile->GetExtension();

  // including the hash of the path incase there is a collision with file names
  // for instance 2 separate images both names cover.png
  auto newFileName = albumArtFile->GetName() + L"_" + std::to_wstring(hash);
  auto path = SaveImageFile(buffer, newFileName, ext);
  ARC_ASSERT(path);
  ARC_ASSERT(path->size() > 0);

  return *path;
}

size_t FindLastPathSeperator(const wstring& path) {
  auto nameStart = path.find_last_of('/');
  if (nameStart == wstring::npos) {
    nameStart = path.find_last_of('\\');
  }
  if (nameStart == wstring::npos) {
    nameStart = 0;
  }
  return nameStart;
}

unordered_map<wstring, size_t> BuildCounts(const vector<wstring>& strings) {
  unordered_map<wstring, size_t> counts;

  for (auto& str : strings) {
    auto itr = counts.find(str);
    if (itr == end(counts)) {
      counts[str] = 1;
    } else {
      ++counts[str];
    }
  }

  return counts;
}

// New Stuff

// functional
bool CheckAlbum(const AlbumArtLoader::IdBoolPair& pair) { return pair.second; }

AlbumArtLoader::AlbumId StripId(const AlbumArtLoader::IdBoolPair& pair) { return pair.first; }

AlbumArtLoader::AlbumId StripIdFromPath(const AlbumArtLoader::IdPathPair& pair) { return pair.first; }