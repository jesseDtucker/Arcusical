#pragma once
#ifndef IPLAYER_HPP
#define IPLAYER_HPP

#include <memory>

#include "MulticastDelegate.hpp"
#include "PropertyHelper.hpp"
#include "ServiceResolver.hpp"

namespace Arcusical
{
	namespace Model
	{
		class Song;
	}

namespace Player
{
	class IPlayer
	{
	public:
		IPlayer() : m_IsPlaying(false), m_IsPaused(false) { }
		IPlayer(const IPlayer&) = delete;
		IPlayer& operator=(const IPlayer&) = delete;
		virtual ~IPlayer() = default;

		static const std::string ServiceName;
		
		virtual void SetSong(const Model::Song& song) = 0;
		virtual Model::Song* GetCurrentSong() = 0;
		PROP_GET(bool, IsPlaying); // true, playback is occurring, false, may be paused or stopped
		PROP_GET(bool, IsPaused);

		virtual void Play() = 0;
		virtual void Stop() = 0;

		virtual double GetDuration() = 0; // in seconds
		virtual double GetCurrentTime() = 0; // in seconds

		PROP_GET_EX(Util::MulticastDelegate<void(double)>, DurationChanged, m_durationChanged, NOT_CONST);
		PROP_GET_EX(Util::MulticastDelegate<void()>, Playing, m_playing, NOT_CONST);
		PROP_GET_EX(Util::MulticastDelegate<void()>, Paused, m_paused, NOT_CONST);
		PROP_GET_EX(Util::MulticastDelegate<void()>, Ended, m_ended, NOT_CONST);
		PROP_GET_EX(Util::MulticastDelegate<void(double)>, TimeUpdate, m_timeUpdate, NOT_CONST);
	};

	typedef Arcusical::ServiceModel::ServiceResolver<IPlayer> PlayerLocator;
}
}


#endif