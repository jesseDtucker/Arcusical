#pragma once
#ifndef MUSIC_TYPES_HPP
#define MUSIC_TYPES_HPP

#include "boost\uuid\uuid.hpp"
#include <memory>
#include <unordered_map>

#include "Song.hpp"
#include "Album.hpp"

namespace Arcusical{
namespace Model
{
	typedef std::unordered_map<boost::uuids::uuid, Arcusical::Model::Song> SongCollection;
	typedef std::unordered_map<boost::uuids::uuid, Arcusical::Model::Album> AlbumCollection;

	typedef std::shared_ptr<SongCollection> SongCollectionLockedPtr;
	typedef std::shared_ptr<AlbumCollection> AlbumCollectionLockedPtr;

	typedef SongCollection* SongCollectionPtr;
	typedef AlbumCollection* AlbumCollectionPtr;

} /* MusicProvider */
} /* Arcusical */

#endif