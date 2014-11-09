#pragma once

#ifndef ALBUM_HPP
#define ALBUM_HPP

#include <memory>
#include <string>
#include <set>
#include <unordered_map>

#include "boost\uuid\uuid.hpp"

#include "Util.hpp"

namespace Arcusical{
namespace Model
{
	class Song;
	class IAlbumToSongMapper;

	class Album final
	{
	public:
		Album(std::shared_ptr<IAlbumToSongMapper> songMapper);
		Album(const Album&) = default;
		Album& operator=(const Album& other) = default;

		PROP_SET_AND_GET(boost::uuids::uuid, Id);
		PROP_SET_AND_GET(std::wstring, Title);
		PROP_SET_AND_GET(std::wstring, Artist);
		PROP_SET_AND_GET(std::wstring, ImageFilePath);
		PROP_SET_AND_GET(std::set<boost::uuids::uuid>, SongIds);

		std::unordered_map<boost::uuids::uuid, Song*>* GetSongs();

		bool operator==(const Album& rhs) const;
		bool operator!=(const Album& rhs) const;
	private:
		std::shared_ptr<IAlbumToSongMapper> m_songMapper;
		std::unordered_map<boost::uuids::uuid, Song*> m_songs;
	};
}
}

#endif