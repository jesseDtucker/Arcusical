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
#include "FilterProcessor.hpp"
#include "PropertyHelper.hpp"
#include "Storage.hpp"
#include "TransformProcessor.hpp"
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
		typedef std::pair<AlbumId, bool> IdBoolPair;

		Util::InputBuffer<AlbumId>* AlbumsNeedingArt();
		Util::InputBuffer<AlbumId>* AlbumsToVerify();
		void NotifyLoadingComplete();
		Util::MulticastDelegate<void(const std::vector<boost::uuids::uuid>&)> OnArtLoaded;

	private:

		std::vector<IdBoolPair> VerifyAlbums(const std::vector<AlbumId>&);
		std::vector<IdPathPair> EmbededAlbumLoad(const std::vector<AlbumId>&);
		// TODO::JT remove this hack to record can return void
		std::vector<int> RecordAlbumArt(const std::vector<IdPathPair>&);
		std::vector<IdPathPair> DelayedAlbumLoad(const std::vector<AlbumId>&);

		Util::AsyncProcessor<AlbumId, IdBoolPair> m_verifier;
		Util::FilterProcessor<IdBoolPair> m_verifyFilter;
		Util::TransformProcessor<IdBoolPair, AlbumId> m_verifyIdStripper;
		Util::AsyncProcessor<AlbumId, IdPathPair> m_embededLoader;
		Util::FilterProcessor<IdPathPair> m_loadedFilter;
		Util::TransformProcessor<IdPathPair, AlbumId> m_loadedIdStripper;
		Util::AsyncProcessor<AlbumId, IdPathPair> m_delayedLoader;
		Util::FilterProcessor<IdPathPair> m_delayedLoadedFilter;
		Util::TransformProcessor<IdPathPair, int> m_recorder;

		LocalMusicStore::LocalMusicCache* m_cache;
		std::future<std::shared_ptr<Util::WorkBuffer<FileSystem::FilePtr>>> m_imagePathFuture;
		std::vector<std::wstring> m_imagePaths;
	};
}
}

#endif