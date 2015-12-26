#include <algorithm>
#include "boost/algorithm/string/predicate.hpp"
#include "boost/exception/all.hpp"
#include <limits>
#include <numeric>

#include "Arc_Assert.hpp"
#include "Exceptions.hpp"
#include "IFileReader.hpp"
#include "IFile.hpp"
#include "Song.hpp"
#include "Storage.hpp"

#undef max
#undef min

using namespace std;
using namespace Util;

namespace Arcusical {
namespace Model {
Song::Song() : m_Length(0), m_TrackNumber({0, 0}), m_DiskNumber({0, 0}) {}

bool Song::operator==(const Song& rhs) const { return this->GetId() == rhs.GetId(); }

bool Song::operator!=(const Song& rhs) const { return !(*this == rhs); }

bool Song::IsSameSong(const Song& otherSong) const {
  // they are considered equal if and only if they have the same title and artist

  return boost::iequals(this->GetTitle(), otherSong.GetTitle()) &&
         boost::iequals(this->GetArtist(), otherSong.GetArtist());
}

bool Song::operator<(const Song& other) const {
  // ordering is defined by a series of precedents
  // disk number takes priority
  // followed by track number
  // followed by lexicographical order of the title
  //
  // zeros are considered 'no info' and so are considered to be after all other elements

  const auto MAX_INT = numeric_limits<int>::max();

  auto thisDiskNum = m_DiskNumber.first;
  auto thisTrackNum = m_TrackNumber.first;

  auto otherDiskNum = other.m_DiskNumber.first;
  auto otherTrackNum = other.m_TrackNumber.first;

  // treat zero as Int.max, we want those that have info to come first, others come second
  thisDiskNum = thisDiskNum != 0 ? thisDiskNum : MAX_INT;
  thisTrackNum = thisTrackNum != 0 ? thisTrackNum : MAX_INT;

  otherDiskNum = otherDiskNum != 0 ? otherDiskNum : MAX_INT;
  otherTrackNum = otherTrackNum != 0 ? otherTrackNum : MAX_INT;

  if (thisDiskNum != MAX_INT || otherDiskNum != MAX_INT) {
    // they one has a disk number, if they don't match compare those instead of the track number
    if (thisDiskNum != otherDiskNum) {
      return thisDiskNum < otherDiskNum;
    }
  }

  if (thisTrackNum != MAX_INT || otherTrackNum != MAX_INT) {
    // one of them has a track number, compare that
    if (thisTrackNum != otherTrackNum) {
      return thisTrackNum < otherTrackNum;
    }
  }

  return m_Title < other.m_Title;
}

bool Song::operator>(const Song& other) const { return !(*this <= other); }

bool Song::operator>=(const Song& other) const { return (*this == other) || (*this > other); }

bool Song::operator<=(const Song& other) const { return (*this == other) || (*this < other); }

void Song::AddFile(const SongFile& songFile) {
#if _DEBUG
  auto& filesOfThisFormat = m_Files[songFile.format];
  auto endItr = find_if(begin(filesOfThisFormat), end(filesOfThisFormat),
                        [&songFile](const SongFile& file) { return file.filePath == songFile.filePath; });
  ARC_ASSERT_MSG(endItr == end(filesOfThisFormat), "Attempted to include 2 copies of the same song file!");
#endif

  m_Files[songFile.format].push_back(songFile);
  m_AvailableFormats.insert(songFile.format);
}

SongFile FindAvailable(const vector<SongFile>& files) {
  SongFile result;
  auto findItr = find_if(begin(files), end(files),
                         [](const SongFile& file) { return FileSystem::Storage::FileExists(file.filePath); });

  if (findItr != end(files)) {
    result = *findItr;
  }

  return result;
}

bool Song::HasStream() {
  bool hasStream = false;

  for (auto& files : m_Files) {
    auto file = FindAvailable(files.second);
    hasStream = (file.filePath.length() > 0);
    if (hasStream) {
      break;
    }
  }

  return hasStream;
}

SongStream Song::GetStream(boost::optional<AudioFormat> specificFormat) {
  SongFile songFile;

  if (specificFormat) {
    auto isFormatAvailable =
        find(begin(m_AvailableFormats), end(m_AvailableFormats), specificFormat) != end(m_AvailableFormats);
    ARC_ASSERT_MSG(isFormatAvailable, "Attempted to get a format that is not available!");

    if (isFormatAvailable) {
      auto& files = m_Files[*specificFormat];
      songFile = FindAvailable(files);
    }
  }

  // if we haven't got a song file yet the format that was specified is likely not available, just pick one for the
  // caller
  if (songFile.filePath.size() == 0) {
    songFile = DetermineBestFormat();
  }

  if (songFile.filePath.size() == 0) {
    throw NoSongFileAvailable() << errinfo_msg("There are no files available in any format for this song!");
  }

  auto file = FileSystem::Storage::LoadFileFromPath(songFile.filePath);
  auto fileReader = FileSystem::Storage::GetReader(file);

  SongStream result;

  result.songData = songFile;
  result.stream = static_pointer_cast<Util::Stream>(fileReader);

  return result;
}

SongFile Song::DetermineBestFormat() {
  // try and select what I think the best format is likely to be
  // assuming lossless is best, followed by AAC followed by mp3
  static vector<AudioFormat> FORMATS = {AudioFormat::ALAC, AudioFormat::FLAC, AudioFormat::WAV,
                                        AudioFormat::AAC,  AudioFormat::MP3};

  for (const auto audioFormat : FORMATS) {
    auto itr = find(begin(m_AvailableFormats), end(m_AvailableFormats), audioFormat);
    if (itr != end(m_AvailableFormats)) {
      auto songFiles = m_Files[audioFormat];
      auto result = FindAvailable(songFiles);

      if (result.filePath.length() != 0) {
        return result;
      }
    }
  }

  throw NoSongFileAvailable() << errinfo_msg("There are no files available in any format for this song!");
}

void Song::RemoveFile(const wstring& path) {
  // a little involved... the songs are stored by their format then a list of files that match the format
  for (auto& files : m_Files) {
    // should just be a remove_if...
    vector<vector<SongFile>::iterator> toDelete;
    for (auto cur = begin(files.second); cur != end(files.second); ++cur) {
      if (cur->filePath == path) {
        toDelete.push_back(cur);
      }
    }
    for (auto& itr : toDelete) {
      files.second.erase(itr);
    }
  }

  // once the files of that path have been cleaned out delete any formats that no longer have
  // any files backing them
  vector<AudioFormat> emptyFormats;
  for (auto format : m_Files) {
    if (format.second.size() == 0) {
      emptyFormats.push_back(format.first);
    }
  }

  for (auto& format : emptyFormats) {
    m_AvailableFormats.erase(format);
    m_Files.erase(format);
  }
}

size_t Song::GetNumFiles() const {
  auto songCount = accumulate(
      begin(m_Files), end(m_Files), size_t(0),
      [](size_t acc, const pair<AudioFormat, vector<SongFile>>& files) { return acc + files.second.size(); });

  return songCount;
}
}
}