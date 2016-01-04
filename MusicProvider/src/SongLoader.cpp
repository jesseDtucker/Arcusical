#define PARSER_VERSOBE 0

#if PARSER_VERSOBE
#include <sstream>
#endif

#include "SongLoader.hpp"

#include <algorithm>
#include <cctype>
#include <codecvt>
#include <functional>
#include <ppltasks.h>
#include <random>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "Album.hpp"
#include "Arc_Assert.hpp"
#include "IAlbumToSongMapper.hpp"
#include "IFileReader.hpp"
#include "IFile.hpp"
#include "IFolder.hpp"
#include "MPEG4_Parser.hpp"
#include "ParallelProcessor.hpp"
#include "Song.hpp"
#include "Storage.hpp"
#include "UUIDGenerator.hpp"

using namespace Arcusical;
using namespace Arcusical::Model;
using namespace Arcusical::MusicProvider;
using namespace boost::uuids;
using namespace FileSystem;
using namespace std;
using namespace Util;

Song LoadSong(const IFile& file);

static Song LoadMpeg4Song(const IFile& file);
static Song DefaultSongLoad(const IFile& file, AudioFormat encoding = AudioFormat::UNKNOWN,
                            ContainerType container = ContainerType::UNKNOWN);
static Song LoadMP3(const IFile& file);
static Song LoadWav(const IFile& file);
static Song LoadFlac(const IFile& file);

static Util::UUIDGenerator s_idGenerator;
static const unsigned long long MIN_LENGTH = 5;  // minimum length of a song for it to be considered a song

static const unordered_map<MPEG4::Encoding, AudioFormat> MPEG4_TO_MODEL_MAPPING = {
    {MPEG4::Encoding::AAC, AudioFormat::AAC}, {MPEG4::Encoding::ALAC, AudioFormat::ALAC},
    {MPEG4::Encoding::UNKNOWN, AudioFormat::UNKNOWN}};

static const unordered_map<wstring, function<Song(const IFile&)>> FILE_EX_TO_LOADER = {
    {L"m4a", LoadMpeg4Song}, {L"mp3", LoadMP3}, {L"wav", LoadWav}, {L"flac", LoadFlac} };

Song LoadSong(const IFile& file) {
  Song result;
  auto fileExtension = file.GetExtension();

  auto fileName = file.GetFullName();

  auto loadFunc = FILE_EX_TO_LOADER.find(fileExtension);
  if (loadFunc != end(FILE_EX_TO_LOADER)) {
    result = loadFunc->second(file);
  } else {
    result = DefaultSongLoad(file);
  }

  if (result.GetTitle().length() == 0) {
    auto fileName = file.GetName();
    replace(begin(fileName), end(fileName), '_', ' ');
    result.SetTitle(fileName);
  }

  return result;
}

Song LoadMpeg4Song(const IFile& file) {
  MPEG4::MPEG4_Parser mpegParser;

  auto fileReader = Storage::GetReader(&file);
  auto mpegSong = mpegParser.ReadAndParseFromStream(*fileReader);

  auto result = Song();

  // setup converter
  wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;

  result.SetArtist(converter.from_bytes(mpegSong->GetArtist()));
  result.SetId(s_idGenerator());  // generate a random id, the odds of collision are slim to none
  result.SetLength(mpegSong->GetLength());
  result.SetTitle(converter.from_bytes(mpegSong->GetTitle()));
  result.SetAlbumName(converter.from_bytes(mpegSong->GetAlbum()));

  auto trackNum = mpegSong->GetTrackNumber();
  auto diskNum = mpegSong->GetDiskNumber();
  result.SetTrackNumber({trackNum.first, trackNum.second});
  result.SetDiskNumber({diskNum.first, diskNum.second});

  AudioFormat songFormat = AudioFormat::UNKNOWN;
  auto encodingItr = MPEG4_TO_MODEL_MAPPING.find(mpegSong->GetEncoding());
  ARC_ASSERT(encodingItr != end(MPEG4_TO_MODEL_MAPPING));
  if (encodingItr != end(MPEG4_TO_MODEL_MAPPING)) {
    songFormat = encodingItr->second;
  }

  SongFile songFile;
  songFile.filePath = file.GetFullPath();
  songFile.bitRate = mpegSong->GetSampleSize() * mpegSong->GetSampleRate();
  songFile.channelCount = mpegSong->GetNumberOfChannels();
  songFile.sampleSize = mpegSong->GetNumberOfChannels();
  songFile.format = songFormat;
  songFile.container = ContainerType::MP4;
  result.AddFile(songFile);

  ARC_ASSERT(songFile.bitRate != 0);
  ARC_ASSERT(songFile.channelCount != 0);
  ARC_ASSERT(songFile.sampleSize != 0);

#if PARSER_VERSOBE

  stringstream stream;
  mpegSong->GetTree()->PrintTree(stream);

  while (!stream.eof()) {
    string output;
    getline(stream, output);
    output.append("\n");
    OutputDebugStringA(output.c_str());
  }
#endif

  return result;
}

Song DefaultSongLoad(const IFile& file, AudioFormat encoding, ContainerType container) {
#ifdef __cplusplus_winrt
  using namespace concurrency;

  auto result = Song();
  auto musicProperties = create_task(file.GetRawHandle()->Properties->GetMusicPropertiesAsync()).get();

  result.SetArtist(wstring(musicProperties->Artist->Data()));
  result.SetId(s_idGenerator());  // generate a random id, the odds of collision are slim to none
  result.SetLength(musicProperties->Duration.Duration /
                   10000000);  // Duration is provided is 100 ns units, but we need it in seconds
  result.SetTitle(wstring(musicProperties->Title->Data()));
  result.SetAlbumName(wstring(musicProperties->Album->Data()));
  result.SetTrackNumber({musicProperties->TrackNumber, musicProperties->TrackNumber});  // TODO::JT need to get the max
                                                                                        // tracks in album set
                                                                                        // correctly... fixup album?

  SongFile songFile;
  songFile.filePath = file.GetFullPath();
  songFile.bitRate = musicProperties->Bitrate;
  songFile.channelCount = 0;  // no idea how to determine this without parsing...
  songFile.sampleSize = 0;    // no idea how to determine this without parsing...
  songFile.format = encoding;
  songFile.container = container;
  result.AddFile(songFile);

  return result;
#elif
#error "Unsupported platform!"
#endif
}

Song LoadMP3(const IFile& file) { return DefaultSongLoad(file, AudioFormat::MP3, ContainerType::MP3); }

Song LoadWav(const IFile& file) { return DefaultSongLoad(file, AudioFormat::WAV, ContainerType::WAV); }

Song LoadFlac(const IFile& file) { return DefaultSongLoad(file, AudioFormat::FLAC, ContainerType::FLAC); }

vector<IFile*> GetNewFiles(const SongCollection& existingSongs, const vector<shared_ptr<IFile>>& files) {
  unordered_set<wstring> existingSongFiles;
  for (auto& song : existingSongs) {
    for (const auto& songFile : song.second.GetFiles()) {
      auto& files = songFile.second;
      transform(begin(files), end(files), inserter(existingSongFiles, begin(existingSongFiles)),
                [](const SongFile& file) { return file.filePath; });
    }
  }

  vector<IFile*> filesRaw;
  filesRaw.resize(files.size());
  auto endItr =
      transform(begin(files), end(files), begin(filesRaw), [](const shared_ptr<IFile>& file) { return file.get(); });
  ARC_ASSERT(endItr == end(filesRaw));

  vector<IFile*> result;
  result.resize(files.size());
  auto lastItr = copy_if(begin(filesRaw), end(filesRaw), begin(result), [&existingSongFiles](const IFile* file) {
    return existingSongFiles.find(file->GetFullPath()) == end(existingSongFiles);
  });
  result.resize(lastItr - begin(result));
  return result;
}

vector<Song> LoadSongs(vector<IFile*> files) {
  SYSTEM_INFO sysInfo;
  GetSystemInfo(&sysInfo);

  ParallelProcessor<IFile*, Song> parellelProcessor([](const IFile* file) { return LoadSong(*file); }, sysInfo.dwNumberOfProcessors / 2);
  parellelProcessor.Append(std::move(files));
  parellelProcessor.Start();
  parellelProcessor.Complete();

  return parellelProcessor.ResultsBuffer().GetAll();
}

struct ExistingSongPair {
  const Song* original;
  const Song* newSong;
};

struct DivideSongsResults {
  vector<const Song*> newSongs;
  vector<ExistingSongPair> newFormatsOfExisting;
};

// first is the new songs, second is the modified songs
DivideSongsResults DivideSongs(const SongCollection& existingSongs, const vector<Song>& loadedSongs) {
  DivideSongsResults result;

  for (auto& song : loadedSongs) {
    auto searchItr =
        find_if(begin(existingSongs), end(existingSongs),
                [&song](const SongCollection::value_type& songPair) { return song.IsSameSong(songPair.second); });

    if (searchItr == end(existingSongs)) {
      // did not find a matching song
      result.newSongs.push_back(&song);
    } else {
      result.newFormatsOfExisting.push_back({&searchItr->second, &song});
    }
  }

  return result;
}

// join the songs into one
Song CombineSongs(const Song* original, const vector<const Song*>& songs) {
  // merge all other songs into the original
  Song result = *original;
  for (auto nextSong : songs) {
    for (auto& files : nextSong->GetFiles()) {
      for (auto& file : files.second) {
        result.AddFile(file);
      }
    }
  }

  return result;
}
Song CombineSongs(const vector<const Song*>& songs) {
  ARC_ASSERT(songs.size() > 0);
  vector<const Song*> input;
  input.reserve(songs.size());
  copy(begin(songs) + 1, end(songs), back_inserter(input));
  return CombineSongs(songs[0], input);
}

vector<Song> FlattenNewSongs(const vector<const Song*>& newSongPtrs) {
  // start by grouping the songs that are the same
  unordered_set<const Song*> alreadyGrouped;
  vector<vector<const Song*>> groups;

  for (auto baseSong : newSongPtrs) {
    // ignore those that have already been grouped
    if (alreadyGrouped.find(baseSong) == end(alreadyGrouped)) {
      vector<const Song*> group;

      group.push_back(baseSong);
      alreadyGrouped.insert(baseSong);

      // now select all that are the same and not this one and group them
      for (auto song : newSongPtrs) {
        if (song != baseSong &&                                  // is not current song
            alreadyGrouped.find(song) == end(alreadyGrouped) &&  // is not already in a group
            song->IsSameSong(*baseSong))                         // is refereeing to the same song as the base
        {
          // then group them!
          group.push_back(song);
          alreadyGrouped.insert(song);
        }
      }

      groups.push_back(group);
    }
  }

  vector<Song> newSongs;
  newSongs.resize(groups.size());

  auto endItr = transform(begin(groups), end(groups), begin(newSongs),
                          [](const vector<const Song*> group) { return CombineSongs(group); });
  ARC_ASSERT(endItr == end(newSongs));

  return move(newSongs);
}

vector<Song> FlattenModifiedSongs(const vector<ExistingSongPair>& modifiedSongs) {
  // first group the songs such that they are all together
  typedef unordered_map<const Song*, vector<const Song*>> SongMap;
  SongMap songMap;
  for (auto& pair : modifiedSongs) {
    auto itr = songMap.find(pair.original);
    if (itr == end(songMap)) {
      songMap[pair.original] = {pair.newSong};
    } else {
      songMap[pair.original].push_back(pair.newSong);
    }
  }

  vector<Song> results;
  results.reserve(songMap.size());

  transform(begin(songMap), end(songMap), back_inserter(results),
            [](const SongMap::value_type& pair) { return CombineSongs(pair.first, pair.second); });

  return results;
}

// TODO::JT this is ugly, please clean it up
void FixupAlbumName(Song& song, const vector<Song>& newSongs, const SongCollection& existingSongs) {
  // then we haven't determined the name of this song.
  // first try to find matching songs in the existing songs
  auto existingMatchItr = find_if(begin(existingSongs), end(existingSongs),
                                  [&song](SongCollection::value_type other) { return other.second.IsSameSong(song); });
  if (existingMatchItr != end(existingSongs)) {
    // one of the existing songs appears to be the same song!
    song.SetAlbumName(existingMatchItr->second.GetAlbumName());
  }

  if (song.GetAlbumName().size() == 0) {
    // then we have not yet located the song, try again with the new songs
    auto newMatchItr =
        find_if(begin(newSongs), end(newSongs), [&song](const Song& other) { return other.IsSameSong(song); });
    if (newMatchItr != end(newSongs)) {
      song.SetAlbumName(newMatchItr->GetAlbumName());
    }
  }

  if (song.GetAlbumName().size() == 0) {
    // if we did not find any matching songs then we will instead try to locate
    // the album name based on the folder structure

    // firstly we'll take each file and determine a common base directory
    wstring basePath = L"";
    for (auto& audioFileList : song.GetFiles()) {
      for (auto& audioFile : audioFileList.second) {
        auto path = audioFile.filePath;
        if (basePath.size() == 0) {
          basePath = path;
        } else {
          auto length = min(basePath.size(), path.size());
          for (unsigned int i = 0; i < length; ++i) {
            if (basePath[i] != path[i]) {
              // cut the string at this point
              basePath = basePath.substr(0, i);
              break;
            }
          }
        }
      }
    }

    // now assume the common base folder is the album name
    wstring albumName = L"";
    set<wstring> invalidAlbumNames = {L"mp3", L"wav", L"alac", L"flac"};
    const ctype<wchar_t>& f = use_facet<ctype<wchar_t>>(locale());
    auto isValidAlbumName = false;
    do {
      auto cutPoint = basePath.find_last_of('\\');
      if (FileSystem::Storage::IsFolder(basePath)) {
        albumName = basePath.substr(cutPoint + 1, basePath.size() - cutPoint);
        wstring albumNameLower = albumName;
        f.tolower(&albumNameLower[0], &albumNameLower[0] + albumNameLower.size());
        isValidAlbumName = invalidAlbumNames.find(albumNameLower) == end(invalidAlbumNames);
      }
      basePath = basePath.substr(0, cutPoint);
    } while ((!isValidAlbumName || albumName.size() == 0) &&
             basePath.find_last_of('\\') != basePath.find_first_of('\\'));

    song.SetAlbumName(albumName);
  }

  ARC_ASSERT_MSG(song.GetAlbumName().size() != 0, "Failed to fixup album name for song!");
}

void FixupSongs(vector<Song>& newSongs, const SongCollection& existingSongs) {
  for (auto& song : newSongs) {
    if (song.GetAlbumName().size() == 0) {
      FixupAlbumName(song, newSongs, existingSongs);
    }
  }
}

// only keep songs that meet the critera of what we wish to present
// currently this rejects songs less that 5 seconds in length
vector<Song> FilterSongs(const vector<Song>& songs) {
  vector<Song> result;
  result.reserve(songs.size());

  copy_if(begin(songs), end(songs), back_inserter(result),
          [](const Song& song) { return song.GetLength() > MIN_LENGTH; });

  return result;
}

SongMergeResult MusicProvider::MergeSongs(const SongCollection& existingSongs, const vector<shared_ptr<IFile>>& files) {
  SongMergeResult result;

  auto newFiles = GetNewFiles(existingSongs, files);
  auto loadedSongs = LoadSongs(newFiles);
  auto songs = FilterSongs(loadedSongs);
  auto dividedSongs = DivideSongs(
      existingSongs, songs);  // sort the new songs into completely new songs and songs that match existing songs
  result.newSongs = FlattenNewSongs(dividedSongs.newSongs);
  result.modifiedSongs = FlattenModifiedSongs(dividedSongs.newFormatsOfExisting);

  FixupSongs(result.newSongs, existingSongs);

  return result;
}

vector<pair<wstring, const Song*>> DetermineMissingFiles(const SongCollection& existingSongs,
                                                         const vector<shared_ptr<IFile>>& files) {
  vector<pair<wstring, const Song*>> expectedPaths;
  vector<pair<wstring, const Song*>> actualPaths;

  expectedPaths.reserve(existingSongs.size());  // won't be enough, but should be close
  actualPaths.reserve(files.size());            // will be exactly enough

  for (auto& songPair : existingSongs) {
    auto& formats = songPair.second.GetFiles();
    for (auto& songFiles : formats) {
      transform(begin(songFiles.second), end(songFiles.second), back_inserter(expectedPaths),
                [&songPair](const SongFile & songFile)
                    ->pair<wstring, const Song*> { return {songFile.filePath, &songPair.second}; });
    }
  }

  transform(begin(files), end(files), back_inserter(actualPaths),
            [](const shared_ptr<IFile> & file)->pair<wstring, const Song*> { return {file->GetFullPath(), nullptr}; });

  auto sortPred = [](const pair<wstring, const Song*> a,
                     const pair<wstring, const Song*>& b) { return a.first < b.first; };

  sort(begin(expectedPaths), end(expectedPaths), sortPred);
  sort(begin(actualPaths), end(actualPaths), sortPred);

  vector<pair<wstring, const Song*>> results;

  set_difference(begin(expectedPaths), end(expectedPaths), begin(actualPaths), end(actualPaths), back_inserter(results),
                 sortPred);

  return results;
}

vector<pair<const Song*, vector<wstring>>> GroupDeletedSongs(vector<pair<wstring, const Song*>> missingSongs) {
  unordered_map<const Song*, vector<wstring>> groups;
  for (auto& song : missingSongs) {
    groups[song.second].push_back(song.first);
  }

  vector<pair<const Song*, vector<wstring>>> results;
  results.reserve(groups.size());
  copy(begin(groups), end(groups), back_inserter(results));

  return results;
}

vector<Song> CompletelyDeletedSongs(const vector<pair<const Song*, vector<wstring>>>& groups) {
  vector<pair<const Song*, vector<wstring>>> deleted;
  copy_if(begin(groups), end(groups), back_inserter(deleted), [](const pair<const Song*, vector<wstring>>& group) {
    // then every file this song references is gone
    return group.first->GetNumFiles() == group.second.size();
  });

  vector<Song> results;
  results.reserve(deleted.size());
  transform(begin(deleted), end(deleted), back_inserter(results),
            [](const pair<const Song*, vector<wstring>>& group) { return *group.first; });

  return results;
}

vector<Song> PartiallyDeletedSongs(const vector<pair<const Song*, vector<wstring>>>& groups) {
  vector<pair<const Song*, vector<wstring>>> modified;
  copy_if(begin(groups), end(groups), back_inserter(modified), [](const pair<const Song*, vector<wstring>>& group) {
    // the only some of the files this song referenced are gone
    return group.first->GetNumFiles() != group.second.size();
  });

  vector<Song> results;
  results.reserve(modified.size());
  transform(begin(modified), end(modified), back_inserter(results), [](const pair<const Song*, vector<wstring>>& pair) {
    auto song = *pair.first;  // take a copy
    for (auto& path : pair.second) {
      song.RemoveFile(path);
    }
    return song;
  });

  return results;
}

SongMergeResult Arcusical::MusicProvider::FindDeletedSongs(const SongCollection& existingSongs,
                                                           const vector<shared_ptr<IFile>>& files) {
  SongMergeResult result;

  auto missingFiles = DetermineMissingFiles(existingSongs, files);
  auto groups = GroupDeletedSongs(missingFiles);
  result.deletedSongs = CompletelyDeletedSongs(groups);
  result.modifiedSongs = PartiallyDeletedSongs(groups);

  return result;
}
