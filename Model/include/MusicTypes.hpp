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

	typedef std::unique_ptr<const SongCollection, std::function<void(const SongCollection*)>> SongCollectionLockedPtr;
	typedef std::unique_ptr<const AlbumCollection, std::function<void(const AlbumCollection*)>> AlbumCollectionLockedPtr;

	typedef std::unordered_map<boost::uuids::uuid, const Arcusical::Model::Song*> SongPtrCollection;
	typedef std::unordered_map<boost::uuids::uuid, const Arcusical::Model::Album*> AlbumPtrCollection;

	struct SongCollectionChanges
	{
		SongCollectionChanges() = default;
		SongCollectionChanges(const SongCollectionChanges&) = delete;
		SongCollectionChanges(SongCollectionChanges&&);

		SongCollectionLockedPtr AllSongs;
		SongPtrCollection NewSongs;
		SongPtrCollection ModifiedSongs;
		SongPtrCollection DeletedSongs;
	};

	struct AlbumCollectionChanges
	{
		AlbumCollectionChanges() = default;
		AlbumCollectionChanges(const AlbumCollectionChanges&) = delete;
		AlbumCollectionChanges(AlbumCollectionChanges&&);

		AlbumCollectionLockedPtr AllAlbums;
		AlbumPtrCollection NewAlbums;
		AlbumPtrCollection ModifiedAlbums;
		AlbumPtrCollection DeletedAlbums;
	};

} /* MusicProvider */
} /* Arcusical */

#endif