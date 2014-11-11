#pragma once
#ifndef SONG_LOADER_HPP
#define SONG_LOADER_HPP

#include "boost/uuid/uuid.hpp"
#include "boost/uuid/uuid_generators.hpp"
#include <memory>

#include "MPEG4_Parser.hpp"

namespace FileSystem
{
	class IFile;
}

namespace Arcusical 
{
namespace Model
{
	class Song;
	class Album;
}
}

namespace Arcusical
{
namespace MusicProvider
{
	class SongIdMapper;

	Model::Song LoadSong(FileSystem::IFile& file);
	Model::Album CreateAlbum(std::wstring& name, Model::Song& song, std::shared_ptr<SongIdMapper>& mapper);
	std::wstring LoadAlbumImage(Model::Song& song, const std::wstring& albumName, bool checkFileSystem = false);
}
}

#endif