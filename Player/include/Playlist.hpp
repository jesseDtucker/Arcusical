#pragma once
#ifndef PLAYLIST_HPP
#define PLAYLIST_HPP

#include <memory>
#include <queue>

#include "PropertyHelper.hpp"
#include "Song.hpp"
#include "Subscription.hpp"

namespace Arcusical{

	namespace Player{

		class IPlayer;

		class Playlist final
		{
		public:
			Playlist(IPlayer* player);

			static const std::string ServiceName;

			template<typename T>
			void Enqueue(T begin, T end, bool startPlayback = true);
			void Enqueue(const Model::Song& song, bool startPlayback = true);

			void PlayNext();

			void Clear();

			PROP_GET(std::queue<Model::Song>, SongQueue);
			PROP_GET(bool, Shuffle);

		private:

			bool TryStartPlayback(const Model::Song& song);

			void SelectMoreSongs();

			std::vector<Model::Song> m_recentlyPlayed;
			IPlayer* m_player = nullptr;
			bool m_wasRecentlyCleared = false;
			std::unique_ptr<Util::Subscription> m_songEndedSub = nullptr;
		};

		template<typename T>
		void Playlist::Enqueue(T begin, T end, bool startPlayback)
		{
			ARC_ASSERT(begin != end);
			for (; begin != end; ++begin)
			{
				m_SongQueue.push(*begin);
			}
			if (startPlayback)
			{
				m_Shuffle = false;
				TryStartPlayback(*begin);
			}
		}

		typedef Arcusical::ServiceModel::ServiceResolver<Playlist> PlaylistLocator;
	}
}

#endif