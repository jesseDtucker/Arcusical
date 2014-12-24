#pragma once
#ifndef PLAYLIST_HPP
#define PLAYLIST_HPP

#include <queue>

#include "PropertyHelper.hpp"
#include "Song.hpp"

namespace Arcusical{

	namespace Player{

		class IPlayer;

		class Playlist final
		{
		public:
			Playlist(const IPlayer& player);

			template<typename T>
			void Enqueue(T begin, T end);
			void Enqueue(Model::Song song);

			void Clear();

			PROP_GET(std::queue<Model::Song>, SongQueue);

		private:
		};

		template<typename T>
		void Playlist::Enqueue(T begin, T end)
		{
			for (; begin != end; ++begin)
			{
				m_songQueue.push(*begin);
			}
			ARC_FAIL("TODO::JT, need to play music on enque");
		}
	}
}

#endif