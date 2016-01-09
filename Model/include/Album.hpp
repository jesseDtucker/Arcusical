#pragma once

#include "boost\uuid\uuid.hpp"
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "Song.hpp"
#include "Util.hpp"


namespace Arcusical {
namespace Model {
class IAlbumToSongMapper;

class Album final {
 public:
  Album(const IAlbumToSongMapper* songMapper);
  Album(const Album&) = default;
  Album& operator=(const Album& other) = default;

  PROP_SET_AND_GET(boost::uuids::uuid, Id);
  PROP_SET_AND_GET(std::wstring, Title);
  PROP_SET_AND_GET(std::wstring, Artist);
  PROP_SET_AND_GET(std::wstring, ImageFilePath);
  PROP_SET_AND_GET(std::set<boost::uuids::uuid>, SongIds);

  std::vector<Model::Song>* GetSongs() const;

  bool operator==(const Album& rhs) const;
  bool operator!=(const Album& rhs) const;

 private:
  const IAlbumToSongMapper* m_songMapper;
  mutable std::vector<Model::Song> m_songs;  // these are fetched on demand
};
}
}
