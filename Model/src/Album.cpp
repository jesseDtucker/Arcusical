#include "pch.h"

#include <algorithm>
#include <functional>
#include <unordered_map>


#include "Album.hpp"
#include "IAlbumToSongMapper.hpp"

using namespace std;

namespace Arcusical{
namespace Model
{
	Album::Album(shared_ptr<IAlbumToSongMapper> songMapper)
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

	vector<Song>* Album::GetSongs() const
	{
		if (this->m_songs.size() == 0)
		{
			auto songs = m_songMapper->GetSongsFromIds(m_SongIds);
			for (auto& song : songs)
			{
				m_songs.push_back(song.second);
			}

			sort(begin(m_songs), end(m_songs), less<Song>());
		}

		return &m_songs;
	}
}
}