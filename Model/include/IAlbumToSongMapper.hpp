#pragma once

#include "boost\uuid\uuid.hpp"
#include <memory>
#include <set>
#include <unordered_map>

#include "MusicTypes.hpp"

namespace Arcusical {
namespace Model {
class Song;

//
// Class provides a mean for albums to locate their songs from their ids on demand
//
class IAlbumToSongMapper {
 public:
  virtual SongCollection GetSongsFromIds(const std::set<boost::uuids::uuid>& ids) const = 0;
};
}
}
