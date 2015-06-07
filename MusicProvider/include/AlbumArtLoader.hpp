#pragma once
#ifndef ALBUM_ART_LOADER_HPP
#define ALBUM_ART_LOADER_HPP

#include <atomic>
#include "boost/functional/hash.hpp"
#include "boost/uuid/uuid.hpp"
#include <random>
#include <string>
#include <tuple>

#include "AsyncProcessor.hpp"
#include "PropertyHelper.hpp"
#include "WorkBuffer.hpp"

namespace Arcusical
{
namespace LocalMusicStore
{
	class LocalMusicCache;
}
namespace MusicProvider
{
	class AlbumArtLoader final
	{
	public:
		AlbumArtLoader(LocalMusicStore::LocalMusicCache* cache);

		typedef boost::uuids::uuid AlbumId;
		typedef std::tuple<AlbumId, std::wstring> AlbumLoadResult;
		Util::InputBuffer<AlbumId>* AlbumsNeedingArt();
		Util::InputBuffer<AlbumId>* AlbumsToVerify();

		void NotifyLoadingComplete();
		Util::MulticastDelegate<void(const std::vector<boost::uuids::uuid>&)> OnArtLoaded;

	private:

		void VerifyAlbums();
		std::vector<AlbumLoadResult> EmbededAlbumLoad(const std::vector<AlbumId>& albums);
		void RecordAlbumArt();
		void FixupAlbumArt(AlbumLoadResult& result);

		Util::AsyncProcessor<AlbumId, AlbumLoadResult> m_embededLoader;
		Util::WorkBuffer<AlbumId> m_albumsToVerify;

		LocalMusicStore::LocalMusicCache* m_cache;
		std::vector<std::wstring> m_defaultArtBag;
		std::default_random_engine m_rand;
	};
}
}

#endif