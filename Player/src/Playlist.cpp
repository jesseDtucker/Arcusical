#include <algorithm>
#include <string>

#include "IPlayer.hpp"
#include "Playlist.hpp"
#include "Song.hpp"
#include "IMusicProvider.hpp"

using namespace std;

namespace Arcusical{
namespace Player {

	const string Arcusical::Player::Playlist::ServiceName("Playlist");

	static const int MAX_HISTORY_SIZE = 400;
	static const int RANDOM_SIZE = 25;
	
	Playlist::Playlist(IPlayer* player)
		: m_player(player)
	{
		m_songEndedSub = player->GetEnded() += [this]()
		{
			PlayNext();
		};
	}

	void Playlist::PlayNext()
	{
		if (m_SongQueue.empty())
		{
			SelectMoreSongs();
		}

		if (!m_SongQueue.empty())
		{
			auto nextSong = m_SongQueue.back();
			m_player->SetSong(nextSong);
			m_SongQueue.pop_back();
			m_player->Play();
			m_recentlyPlayed.push_back(nextSong);

			if (m_recentlyPlayed.size() > MAX_HISTORY_SIZE)
			{
				auto newEnd = copy_n(begin(m_recentlyPlayed), MAX_HISTORY_SIZE / 2, begin(m_recentlyPlayed));
				m_recentlyPlayed.resize(distance(begin(m_recentlyPlayed), newEnd));
			}
		}
	}

	void Playlist::PlayPrevious(double goToStartThreshold /* = 5.0 */)
	{
		if (m_player->GetCurrentTime() > goToStartThreshold || m_recentlyPlayed.size() <= 1)
		{
			// then just go to the start of the song
			m_player->SetCurrentTime(0.0);
		}
		else
		{
			// start playing the previous song
			if (m_recentlyPlayed.size() > 1)
			{
				// put the current song on the queue and then the song before that
				auto curSong = m_recentlyPlayed.back();
				m_recentlyPlayed.pop_back();
				auto prevSong = m_recentlyPlayed.back();
				m_recentlyPlayed.pop_back();

				m_SongQueue.insert(end(m_SongQueue), { curSong, prevSong });
				PlayNext();
			}
		}
	}

	bool Playlist::TryStartPlayback()
	{
		// start playback only if no song is playing and it is not currently paused
		bool startNext = !m_player->GetIsPlaying() && !m_player->GetIsPaused();
		if (m_wasRecentlyCleared || startNext)
		{
			PlayNext();
		}
		m_wasRecentlyCleared = false;

		return startNext;
	}

	void Playlist::Enqueue(const Model::Song& song, bool startPlayback)
	{
		m_SongQueue.push_back(song);
		if (startPlayback)
		{
			m_Shuffle = false;
			TryStartPlayback();
		}
	}

	void Playlist::Clear()
	{
		m_SongQueue = {};
		m_Shuffle = false;
		m_wasRecentlyCleared = true;
	}

	void Playlist::SelectMoreSongs()
	{
		auto musicProvider = MusicProvider::MusicProviderLocator::ResolveService().lock();

		ARC_ASSERT(musicProvider != nullptr);

		auto songFilter = [this](const Model::Song& song)
		{
			return find_if(begin(m_recentlyPlayed), end(m_recentlyPlayed), [&song](const Model::Song& playedSong)
			{
				return playedSong == song;
			}) == end(m_recentlyPlayed);
		};

		auto songSelector = musicProvider->GetSongSelector();

		if (m_recentlyPlayed.size() == 0)
		{
			m_Shuffle = true;
		}

		if (!m_Shuffle)
		{
			auto prevSong = m_recentlyPlayed.back();
			auto albumSongs = songSelector->GetFromSameAlbum(prevSong, songFilter);
			if (albumSongs.size() > 0)
			{
				Enqueue(albumSongs, false);
			}
			else
			{
				auto artistSongs = songSelector->GetFromSameArtist(prevSong, songFilter);
				if (artistSongs.size() > 0)
				{
					Enqueue(artistSongs, false);
				}
			}
		}
		
		if (m_SongQueue.size() == 0)
		{
			m_Shuffle = true;
			auto randomSongs = songSelector->GetRandomSongs(RANDOM_SIZE, songFilter);
			if (randomSongs.size() > 0)
			{
				Enqueue(randomSongs, false);
			}
		}
	}
}
}