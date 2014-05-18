#pragma once

#ifndef IMUSIC_PROVIDER_HPP
#define IMUSIC_PROVIDER_HPP

#include "boost\uuid\uuid.hpp"
#include <memory>
#include <functional>
#include <unordered_map>

#include "Delegate.hpp"
#include "Subscription.hpp"
#include "ServiceResolver.hpp"

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

	typedef std::weak_ptr<const std::unordered_map<boost::uuids::uuid, std::shared_ptr<Arcusical::Model::Song>>> SongListPtr;
	typedef std::weak_ptr<const std::unordered_map<boost::uuids::uuid, std::shared_ptr<Arcusical::Model::Album>>> AlbumListPtr;
	// Signature: (LocalSongs, RemoteSongs)
	typedef Util::Delegate<void(SongListPtr, SongListPtr)> SongsChangedCallback;
	typedef Util::Delegate<void(AlbumListPtr)> AlbumsChangedCallback;
	typedef std::shared_ptr<Util::Subscription> MusicProviderSubscription;

	class IMusicProvider
	{
	public:
		virtual MusicProviderSubscription SubscribeSongs(SongsChangedCallback callback) = 0;
		virtual MusicProviderSubscription SubscribeAlbums(AlbumsChangedCallback callback) = 0;

		static const std::string ServiceName;
	};
}
}

#endif