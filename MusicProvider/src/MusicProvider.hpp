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

#include "IMusicProvider.hpp"
#include "LocalMusicCache.hpp"
#include "MusicFinder.hpp"
#include "MusicTypes.hpp"
#include "SongLoader.hpp"

namespace FileSystem
{
	class IFile;
}

namespace Arcusical
{
namespace Model
{
	class Song;
}
}

namespace Arcusical
{
namespace MusicProvider
{
	class SongIdMapper;

	class MusicProvider final : public IMusicProvider
	{
	public:
		MusicProvider();
		virtual Util::Subscription SubscribeSongs(SongsChangedCallback callback) override;
		virtual Util::Subscription SubscribeAlbums(AlbumsChangedCallback callback) override;
		virtual SongSelector* GetSongSelector() override;
		virtual Model::Album GetAlbum(const std::wstring& name) override;
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

		std::shared_ptr<SongIdMapper> m_songMapper;
		std::unique_ptr<LocalMusicStore::LocalMusicCache> m_musicCache;
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