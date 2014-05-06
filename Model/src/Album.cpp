#include "pch.h"

#include "Album.hpp"

#include "IAlbumToSongMapper.hpp"

#include <unordered_map>

namespace Arcusical{
namespace Model
{
	Album::Album(std::shared_ptr<IAlbumToSongMapper> songMapper)
		: m_songMapper(songMapper)
	{

	}

	bool Album::operator==(const Album& rhs) const
	{
		return this->GetId() == rhs.GetId();
	}

	bool Album::operator!=(const Album& rhs) const
	{
		return !(*this == rhs);
	}

	std::unordered_map<boost::uuids::uuid, std::shared_ptr<Song>>& Album::GetSongs()
	{
		if (this->m_songs.size() == 0)
		{
			m_songs = m_songMapper->GetSongsFromIds(m_SongIds);
		}

		return m_songs;
	}
}
}