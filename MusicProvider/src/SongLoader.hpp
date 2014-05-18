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

	class SongLoader final
	{
	public:
		SongLoader() = default;
		SongLoader(const SongLoader&) = delete;

		std::shared_ptr<Model::Song> LoadSong(std::shared_ptr<FileSystem::IFile> file);
		std::shared_ptr<Model::Album> CreateAlbum(std::wstring name, std::shared_ptr<Model::Song> song, std::shared_ptr<SongIdMapper>& mapper);

	private:
		std::shared_ptr<Model::Song> LoadMpeg4Song(std::shared_ptr<FileSystem::IFile> file);
		std::shared_ptr<Model::Song> LoadMP3(std::shared_ptr<FileSystem::IFile> file);
		std::shared_ptr<Model::Song> LoadWav(std::shared_ptr<FileSystem::IFile> file);

		MPEG4::MPEG4_Parser m_mpegParser;

		boost::uuids::random_generator m_idGenerator;
	};
}
}

#endif