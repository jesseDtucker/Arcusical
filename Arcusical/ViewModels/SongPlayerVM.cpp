#include "pch.h"

#include "Arc_Assert.hpp"
#include "IPlayer.hpp"
#include "Song.hpp"
#include "SongPlayerVM.hpp"

using namespace Arcusical::Model;

namespace Arcusical
{
	namespace ViewModel
	{
		SongPlayerVM::SongPlayerVM()
			: m_CurrentSong(nullptr)
			, m_AmountPlayed(0.0f)
			, m_AmoutRemaining(0.0f)
			, m_IsPlaying(false)
		{
			// listen to the song player and respond to changes in state
			auto player = Player::PlayerLocator::ResolveService().lock();
			ARC_ASSERT(player != nullptr);

			m_durationSub = player->GetDurationChanged() += [this, player](double duration)
			{
				DispatchToUI([this, player, duration]()
				{
					UpdateTime(player->GetCurrentTime(), duration);
				});
			};

			m_timeUpdateSub = player->GetTimeUpdate() += [this, player](double currentTime)
			{
				if (abs(this->m_AmountPlayed - currentTime) > 0.1)
				{
					DispatchToUI([this, player, currentTime]()
					{
						UpdateTime(currentTime, player->GetDuration());
					});
				}
			};

			m_playingSub = player->GetPlaying() += [this](bool isPlaying)
			{
				DispatchToUI([this, isPlaying]()
				{
					IsPlaying = isPlaying;
				});
			};

			m_songChangedSub = player->GetSongChanged() += [this](const Song& newSong)
			{
				SongVM^ curSong = ref new SongVM(newSong);
				DispatchToUI([this, curSong]()
				{
					CurrentSong = curSong;
				});
			};
		}

		void SongPlayerVM::Play()
		{
			auto player = Player::PlayerLocator::ResolveService().lock();
			ARC_ASSERT(player != nullptr);
			player->Play();
		}

		void SongPlayerVM::Pause()
		{
			auto player = Player::PlayerLocator::ResolveService().lock();
			ARC_ASSERT(player != nullptr);
			player->Stop();
		}

		void SongPlayerVM::UpdateTime(double amountPlayed, double duration)
		{
			m_songLength = duration;
			AmountPlayed = amountPlayed;
			AmoutRemaining = duration - amountPlayed;
		}

	}
}