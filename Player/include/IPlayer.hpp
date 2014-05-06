#pragma once
#ifndef IPLAYER_HPP
#define IPLAYER_HPP

#include <memory>

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
		IPlayer() = default;
		IPlayer(const IPlayer&) = delete;
		IPlayer& operator=(const IPlayer&) = delete;
		virtual ~IPlayer() = default;

		static const std::string ServiceName;

		virtual void SetSong(std::shared_ptr<Model::Song> song) = 0;
		virtual std::shared_ptr<Model::Song> GetCurrentSong() = 0;

		virtual void Play() = 0;
		virtual void Stop() = 0;
	};

	typedef Arcusical::ServiceModel::ServiceResolver<IPlayer> PlayerLocator;
}
}


#endif