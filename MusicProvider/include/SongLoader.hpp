#pragma once
#ifndef SONG_LOADER_HPP
#define SONG_LOADER_HPP

#include "boost/uuid/uuid.hpp"
#include "boost/uuid/uuid_generators.hpp"
#include <memory>

#include "MusicTypes.hpp"

namespace FileSystem
{
	class IFile;
}

namespace Arcusical 
{
namespace Model
{
	class IAlbumToSongMapper;
	class Song;
	class Album;
}
}

namespace Arcusical
{
namespace MusicProvider
{
	struct SongMergeResult
	{
		std::vector<Model::Song> newSongs;
		std::vector<Model::Song> modifiedSongs;
		std::vector<Model::Song> deletedSongs;
	};

	struct AlbumMergeResult
	{
		std::vector<Model::Album> newAlbums;
		std::vector<Model::Album> modifiedAlbums;
		std::vector<Model::Album> deletedAlbums;
	};

	SongMergeResult MergeSongs(const Model::SongCollection& existingSongs, const std::vector<std::shared_ptr<FileSystem::IFile>>& files);
	AlbumMergeResult MergeAlbums(const Model::AlbumCollection& existingAlbums, const Model::SongCollection& songs, const Model::IAlbumToSongMapper* mapper);

	SongMergeResult FindDeletedSongs(const Model::SongCollection& existingSongs, const std::vector<std::shared_ptr<FileSystem::IFile>>& files);
	AlbumMergeResult FindDeletedAlbums(const Model::AlbumCollection& existingAlbums, const Model::SongCollection& songs);
}
}

#endif