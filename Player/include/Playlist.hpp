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
			void Enqueue(const T& collection, bool startPlayback = true);
			void Enqueue(const Model::Song& song, bool startPlayback = true);

			void PlayNext();
			void PlayPrevious(double goToStartThreshold = 5.0); // if the song is beyond the threshold (in seconds) then just go to the start of the song

			void Clear();

			PROP_GET(std::vector<Model::Song>, SongQueue);
			PROP_GET(bool, Shuffle);

		private:

			bool TryStartPlayback();

			void SelectMoreSongs();

			std::vector<Model::Song> m_recentlyPlayed;
			IPlayer* m_player = nullptr;
			bool m_wasRecentlyCleared = false;
			Util::Subscription m_songEndedSub = nullptr;
		};

		template<typename T>
		void Playlist::Enqueue(const T& collection, bool startPlayback)
		{
			for (auto itr = crbegin(collection); itr != crend(collection); ++itr)
			{
				m_SongQueue.push_back(*itr);
			}
			if (startPlayback)
			{
				m_Shuffle = false;
				TryStartPlayback();
			}
		}

		typedef Arcusical::ServiceModel::ServiceResolver<Playlist> PlaylistLocator;
	}
}

#endif