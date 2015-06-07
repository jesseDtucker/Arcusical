#pragma once
#ifndef MUSIC_TYPES_HPP
#define MUSIC_TYPES_HPP

#include "boost\functional\hash.hpp"
#include "boost\uuid\uuid.hpp"
#include <memory>
#include <unordered_map>

#include "Song.hpp"
#include "Album.hpp"

namespace Arcusical{
namespace Model
{
	typedef std::unordered_map<boost::uuids::uuid, Arcusical::Model::Song, boost::hash<boost::uuids::uuid>> SongCollection;
	typedef std::unordered_map<boost::uuids::uuid, Arcusical::Model::Album, boost::hash<boost::uuids::uuid>> AlbumCollection;

	typedef std::unique_ptr<const SongCollection, std::function<void(const SongCollection*)>> SongCollectionLockedPtr;
	typedef std::unique_ptr<const AlbumCollection, std::function<void(const AlbumCollection*)>> AlbumCollectionLockedPtr;

	typedef std::unordered_map<boost::uuids::uuid, const Arcusical::Model::Song*, boost::hash<boost::uuids::uuid>> SongPtrCollection;
	typedef std::unordered_map<boost::uuids::uuid, const Arcusical::Model::Album*, boost::hash<boost::uuids::uuid>> AlbumPtrCollection;

	enum class LoadProgress
	{
		CACHE_LOAD_PENDING,
		CACHE_LOAD_COMPLETE,
		DISK_LOAD_IN_PROGRESS,
		DISK_LOAD_COMPLETE
	};

	struct SongCollectionChanges
	{
		SongCollectionChanges() = default;
		SongCollectionChanges(const SongCollectionChanges&) = delete;
		SongCollectionChanges(SongCollectionChanges&&);

		SongCollectionLockedPtr AllSongs;
		SongPtrCollection NewSongs;
		SongPtrCollection ModifiedSongs;
		SongPtrCollection DeletedSongs;

		LoadProgress Progress;
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

		LoadProgress Progress;
	};

} /* MusicProvider */
} /* Arcusical */

#endif