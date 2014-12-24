#include "pch.h"

#include "Playlist.hpp"
#include "Song.hpp"

namespace Arcusical{
namespace Player {
	
	Playlist::Playlist(const IPlayer& player)
	{
		// TODO::JT list for player events
		ARC_FAIL("TODO::JT");
	}

	void Playlist::Enqueue(Model::Song song)
	{
		m_SongQueue.push(song);
		ARC_FAIL("TODO::JT, need to play music on enque");
	}

	void Playlist::Clear()
	{
		m_SongQueue = {};
	}
}
}