#pragma once
#ifndef IPLAYER_HPP
#define IPLAYER_HPP

#include <memory>

#include "MulticastDelegate.hpp"
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
		IPlayer() : m_IsPlaying(false) { }
		IPlayer(const IPlayer&) = delete;
		IPlayer& operator=(const IPlayer&) = delete;
		virtual ~IPlayer() = default;

		static const std::string ServiceName;
		
		virtual void SetSong(const Model::Song& song) = 0;
		virtual Model::Song* GetCurrentSong() = 0;
		PROP_GET(bool, IsPlaying)

		virtual void Play() = 0;
		virtual void Stop() = 0;

		virtual double GetDuration() = 0; // in seconds
		virtual double GetCurrentTime() = 0; // in seconds

		PROP_GET(Util::MulticastDelegate<void(double)>, DurationChanged);
		PROP_GET(Util::MulticastDelegate<void()>, Playing);
		PROP_GET(Util::MulticastDelegate<void()>, Paused);
		PROP_GET(Util::MulticastDelegate<void()>, Ended);
		PROP_GET(Util::MulticastDelegate<void(double)>, TimeUpdate);
	};

	typedef Arcusical::ServiceModel::ServiceResolver<IPlayer> PlayerLocator;
}
}


#endif