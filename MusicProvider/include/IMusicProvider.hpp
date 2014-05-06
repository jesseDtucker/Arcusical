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
}
}

namespace Arcusical
{
namespace MusicProvider
{
	class IMusicProvider;

	typedef Arcusical::ServiceModel::ServiceResolver<IMusicProvider> MusicProviderLocator;

	typedef std::weak_ptr<std::unordered_map<boost::uuids::uuid, std::shared_ptr<Arcusical::Model::Song>>> SongListPtr;
	// Signature: (LocalSongs, RemoteSongs)
	typedef Util::Delegate<void(SongListPtr, SongListPtr)> MusicChangedCallback;
	typedef std::shared_ptr<Util::Subscription> MusicProviderSubscription;

	class IMusicProvider
	{
	public:
		virtual MusicProviderSubscription Subscribe(MusicChangedCallback callback) = 0;

		static const std::string ServiceName;
	};
}
}

#endif