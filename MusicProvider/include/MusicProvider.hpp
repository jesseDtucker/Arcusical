#pragma once

#ifndef MUSIC_PROVIDER_HPP
#define MUSIC_PROVIDER_HPP

#include <atomic>
#include "boost/optional.hpp"
#include "boost/uuid/uuid.hpp"
#include <condition_variable>
#include <memory>
#include <mutex>
#include <set>
#include <unordered_map>
#include <vector>

#include "MusicTypes.hpp"
#include "SongLoader.hpp"
#include "SongSelector.hpp"

namespace FileSystem
{
	class IFile;
}

namespace Arcusical
{
namespace LocalMusicStore
{
	class LocalMusicCache;
}
namespace Model
{
	class Song;
}
}

namespace Arcusical
{
namespace MusicProvider
{
	typedef Util::Delegate<void(const Model::SongCollectionChanges&)> SongsChangedCallback;
	typedef Util::Delegate<void(const Model::AlbumCollectionChanges&)> AlbumsChangedCallback;

	class MusicProvider final
	{
	public:
		MusicProvider(LocalMusicStore::LocalMusicCache* cache);
		Util::Subscription SubscribeSongs(SongsChangedCallback callback);
		Util::Subscription SubscribeAlbums(AlbumsChangedCallback callback);
		SongSelector* GetSongSelector();
		boost::optional<Model::Album> GetAlbum(const std::wstring& name);
	private:

		void LoadSongs();
		void LoadAlbums();
		
		Util::MulticastDelegate<SongsChangedCallback::CB_Type> m_songCallbacks;
		Util::MulticastDelegate<AlbumsChangedCallback::CB_Type> m_albumCallbacks;

		std::mutex m_albumCallbackLock;
		std::mutex m_songCallbackLock;

		LocalMusicStore::LocalMusicCache* m_musicCache;
		SongSelector m_songSelector;

		Util::Subscription m_songSubscription;

		std::vector<std::wstring> m_defaultAlbumImgBag;

		bool m_hasSongLoadingBegun = false;
		bool m_hasAlbumLoadingBegun = false;
		std::mutex m_isLoadingLock;
	};
}
}

#endif