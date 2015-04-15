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
	};

	struct AlbumMergeResult
	{
		std::vector<Model::Album> newAlbums;
		std::vector<Model::Album> modifiedAlbums;
	};

	SongMergeResult MergeSongs(const Model::SongCollection& existingSongs, const std::vector<std::shared_ptr<FileSystem::IFile>>& files);
	AlbumMergeResult MergeAlbums(const Model::AlbumCollection& existingAlbums, const Model::SongCollection& songs, const Model::IAlbumToSongMapper* mapper);
}
}

#endif