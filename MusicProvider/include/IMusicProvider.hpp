#pragma once

#ifndef IMUSIC_PROVIDER_HPP
#define IMUSIC_PROVIDER_HPP

#include "boost\uuid\uuid.hpp"
#include <memory>
#include <string>

#include "Delegate.hpp"
#include "MusicTypes.hpp"
#include "Subscription.hpp"
#include "ServiceResolver.hpp"
#include "SongSelector.hpp"

namespace Arcusical{
namespace Model
{
	class Song;
	class Album;
}
}

namespace Arcusical
{
namespace MusicProvider
{
	class IMusicProvider;

	typedef Arcusical::ServiceModel::ServiceResolver<IMusicProvider> MusicProviderLocator;

	typedef Util::Delegate<void(Model::SongCollection&)> SongsChangedCallback;
	typedef Util::Delegate<void(Model::AlbumCollection&)> AlbumsChangedCallback;

	class IMusicProvider
	{
	public:

		virtual Util::Subscription SubscribeSongs(SongsChangedCallback callback) = 0;
		virtual Util::Subscription SubscribeAlbums(AlbumsChangedCallback callback) = 0;

		virtual SongSelector* GetSongSelector() = 0;
		virtual Model::Album GetAlbum(const std::wstring& name) = 0;

		static const std::string ServiceName;
	};
}
}

#endif