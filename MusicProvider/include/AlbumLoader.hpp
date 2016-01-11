#pragma once

#include <vector>

#include "MusicTypes.hpp"

namespace FileSystem {
class IFile;
}

namespace Arcusical {
namespace Model {
class IAlbumToSongMapper;
class Album;
}
}

namespace Arcusical {
namespace MusicProvider {
struct AlbumMergeResult {
  std::vector<Model::Album> newAlbums;
  std::vector<Model::Album> modifiedAlbums;
  std::vector<Model::Album> deletedAlbums;
};

AlbumMergeResult MergeAlbums(const Model::AlbumCollection& existingAlbums, const Model::SongCollection& songs,
                             const Model::IAlbumToSongMapper* mapper);
AlbumMergeResult FindDeletedAlbums(const Model::AlbumCollection& existingAlbums, const Model::SongCollection& songs);
}
}
