#pragma once
#ifndef IALBUM_TO_SONG_MAPPER

#include <memory>
#include <set>
#include <unordered_map>

#include "boost\uuid\uuid.hpp"

namespace Arcusical{
namespace Model
{
	class Song;

	//
	// Class provides a mean for albums to locate their songs from their ids on demand
	//
	class IAlbumToSongMapper
	{
	public:
		virtual std::unordered_map<boost::uuids::uuid, Song> GetSongsFromIds(const std::set<boost::uuids::uuid>& ids) = 0;
	};
	}
}

#endif