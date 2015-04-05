#pragma once

#ifndef MUSIC_PROVIDER_HPP
#define MUSIC_PROVIDER_HPP

#include <atomic>
#include "boost/uuid/uuid.hpp"
#include <condition_variable>
#include <memory>
#include <mutex>
#include <set>
#include <unordered_map>
#include <vector>

#include "MusicFinder.hpp"
#include "MusicTypes.hpp"
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
	typedef Util::Delegate<void(Model::SongCollection&)> SongsChangedCallback;
	typedef Util::Delegate<void(Model::AlbumCollection&)> AlbumsChangedCallback;

	class MusicProvider final
	{
	public:
		MusicProvider(LocalMusicStore::LocalMusicCache* cache);
		Util::Subscription SubscribeSongs(SongsChangedCallback callback);
		Util::Subscription SubscribeAlbums(AlbumsChangedCallback callback);
		SongSelector* GetSongSelector();
		Model::Album GetAlbum(const std::wstring& name);
	private:

		void Unsubscribe(SongsChangedCallback callback);
		void Unsubscribe(AlbumsChangedCallback callback);
		void LoadSongs();
		void LoadAlbums();
		void PublishSongs();
		void PublishAlbums();

		bool MergeSongCollections(Model::SongCollection& existingSongs, std::vector<std::shared_ptr<FileSystem::IFile>>& locatedFiles);
		void FixupSongs(std::vector<Model::Song>& newSongs, Model::SongCollection& existingSongs);
		void AddNewSongToExisting(const Model::Song& newSong, Model::Song& existingSong, std::wstring fullPath);
		void FixupAlbums(std::vector<Model::Album>& newAlbums);
		void FixupAlbumImage(Model::Album& album);

		bool MergeAlbumCollections(Model::AlbumCollection& existingAlbums, Model::SongCollection& songs);

		std::set<SongsChangedCallback> m_songCallbackSet;
		std::set<AlbumsChangedCallback> m_albumCallbackSet;
		std::mutex m_albumCallbackLock;
		std::mutex m_songCallbackLock;

		LocalMusicStore::LocalMusicCache* m_musicCache;
		LocalMusicStore::MusicFinder m_musicFinder;
		SongSelector m_songSelector;

		Util::Subscription m_albumSubscription;

		std::vector<std::wstring> m_defaultAlbumImgBag;

		bool m_isLoading = true;
		bool m_hasSongLoadingBegun = false;
		bool m_hasAlbumLoadingBegun = false;
		std::mutex m_isLoadingLock;
	};
}
}

#endif