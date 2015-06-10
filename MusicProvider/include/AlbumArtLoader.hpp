#pragma once
#ifndef ALBUM_ART_LOADER_HPP
#define ALBUM_ART_LOADER_HPP

#include <atomic>
#include "boost/functional/hash.hpp"
#include "boost/uuid/uuid.hpp"
#include <future>
#include <string>
#include <utility>

#include "AsyncProcessor.hpp"
#include "PropertyHelper.hpp"
#include "Storage.hpp"
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
		~AlbumArtLoader();

		typedef boost::uuids::uuid AlbumId;
		typedef std::pair<AlbumId, std::wstring> IdPathPair;
		Util::InputBuffer<AlbumId>* AlbumsNeedingArt();
		Util::InputBuffer<AlbumId>* AlbumsToVerify();

		void NotifyLoadingComplete();
		Util::MulticastDelegate<void(const std::vector<boost::uuids::uuid>&)> OnArtLoaded;

	private:

		void VerifyAlbums();
		std::vector<IdPathPair> EmbededAlbumLoad(const std::vector<AlbumId>& albums);
		void RecordAlbumArt();
		void SaveAlbums(const std::vector<IdPathPair>& loadResults);
		void DelayedArtLoad();

		Util::AsyncProcessor<AlbumId, IdPathPair> m_embededLoader;
		Util::WorkBuffer<AlbumId> m_albumsToVerify;
		Util::WorkBuffer<AlbumId> m_delayedAlbumsToLoad; // ie. ones that failed the load before all songs were available

		// async operations
		std::future<void> m_verifyFuture;
		std::future<void> m_recordFuture;
		std::future<void> m_delayedLoadFuture;

		LocalMusicStore::LocalMusicCache* m_cache;
		std::shared_ptr<Util::WorkBuffer<FileSystem::FilePtr>> m_imageFilesWB;
	};
}
}

#endif