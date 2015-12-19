#include "pch.h"

#include <future>

#include "Arc_Assert.hpp"
#include "IPlayer.hpp"
#include "Playlist.hpp"
#include "Song.hpp"
#include "SongPlayerVM.hpp"

using namespace std;
using namespace Arcusical::Model;
using namespace Arcusical::Player;

namespace Arcusical
{
	namespace ViewModel
	{
		SongPlayerVM::SongPlayerVM(IPlayer& player, Playlist& playlist, MusicProvider::MusicProvider& provider, Util::BackgroundWorker& worker)
			: m_CurrentSong(nullptr)
			, m_AmountPlayed(0.0)
			, m_AmoutRemaining(0.0)
			, m_Length(0.0)
			, m_IsPlaying(false)
			, m_player(player)
			, m_playlist(playlist)
			, m_provider(provider)
			, m_worker(worker)
		{
			// listen to the song player and respond to changes in state
			m_durationSub = m_player.GetDurationChanged() += [this](double duration)
			{
				DispatchToUI([this, duration]()
				{
					UpdateTime(m_player.GetCurrentTime(), duration);
				});
			};

			m_timeUpdateSub = m_player.GetTimeUpdate() += [this](double currentTime)
			{
				if (abs(this->m_AmountPlayed - currentTime) > 0.1)
				{
					DispatchToUI([this, currentTime]()
					{
						UpdateTime(currentTime, m_player.GetDuration());
					});
				}
			};

			m_playingSub = m_player.GetPlaying() += [this](bool isPlaying)
			{
				DispatchToUI([this, isPlaying]()
				{
					IsPlaying = isPlaying;
				});
			};

			m_songChangedSub = m_player.GetSongChanged() += [this](const boost::optional<Song>& newSong)
			{
				Song song = newSong ? *newSong : Song();
				SongVM^ curSong = ref new SongVM(song, m_playlist, m_player, m_worker);
				auto curAlbum = m_provider.GetAlbum(song.GetAlbumName());

				DispatchToUI([this, curSong, curAlbum]()
				{
					CurrentSong = curSong;
					if (curAlbum)
					{
						if (this->Album != nullptr)
						{
							this->Album->SetFrom(*curAlbum);
						}
						else
						{
							this->Album = ref new AlbumVM(*curAlbum, m_playlist, m_player, m_worker);
						}
					}
				});
			};
		}

		void SongPlayerVM::Play()
		{
			m_worker.Append([this]()
			{
				m_player.Play();
			});
		}

		void SongPlayerVM::Pause()
		{
			m_worker.Append([this]()
			{
				m_player.Stop();
			});
		}

		void SongPlayerVM::Previous()
		{
			m_worker.Append([this]()
			{
				m_playlist.PlayPrevious();
			});
		}

		void SongPlayerVM::Next()
		{
			m_worker.Append([this]()
			{
				m_playlist.PlayNext();
			});
		}

		void SongPlayerVM::UpdateTime(double amountPlayed, double duration)
		{
			Length = duration;
			AmountPlayed = amountPlayed;
			AmoutRemaining = duration - amountPlayed;
		}

		void SongPlayerVM::ChangeTimeTo(double newTime)
		{
			m_player.SetCurrentTime(newTime);
		}

	}
}