#pragma once
#ifndef SONG_LOADER_HPP
#define SONG_LOADER_HPP

#include <memory>
#include "boost/uuid/uuid.hpp"

#include "MusicTypes.hpp"

namespace FileSystem {
class IFile;
}

namespace Arcusical {
namespace Model {
class Song;
}
}

namespace Arcusical {
namespace MusicProvider {
struct SongMergeResult {
  std::vector<Model::Song> newSongs;
  std::vector<Model::Song> modifiedSongs;
  std::vector<Model::Song> deletedSongs;
};

SongMergeResult MergeSongs(const Model::SongCollection& existingSongs,
                           const std::vector<std::shared_ptr<FileSystem::IFile>>& files);
SongMergeResult FindDeletedSongs(const Model::SongCollection& existingSongs,
                                 const std::vector<std::shared_ptr<FileSystem::IFile>>& files);
}
}

#endif