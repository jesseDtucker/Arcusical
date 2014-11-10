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
#include "MusicTypes.hpp"
#include "SongLoader.hpp"

namespace FileSystem
{
	class IFile;
}

namespace Arcusical
{
namespace LocalMusicStore
{
	class LocalMusicCache;
	class MusicFinder;
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
	class SongIdMapper;

	class MusicProvider final : public IMusicProvider
	{
	public:
		MusicProvider();
		virtual MusicProviderSubscription SubscribeSongs(SongsChangedCallback callback) override;
		virtual MusicProviderSubscription SubscribeAlbums(AlbumsChangedCallback callback) override;
	private:

		void Unsubscribe(SongsChangedCallback callback);
		void Unsubscribe(AlbumsChangedCallback callback);
		void LoadSongs();
		void LoadAlbums();
		void PublishSongs();
		void PublishAlbums();

		bool MergeSongCollections(Model::SongCollection& existingSongs, std::vector<std::shared_ptr<FileSystem::IFile>>& locatedFiles);
		void AddNewSongToExisting(const Model::Song& newSong, Model::Song& existingSong, std::wstring fullPath);

		bool MergeAlbumCollections(const Model::AlbumCollection& existingAlbums, Model::SongCollection& songs);

		std::set<SongsChangedCallback> m_songCallbackSet;
		std::set<AlbumsChangedCallback> m_albumCallbackSet;
		std::mutex m_albumCallbackLock;
		std::mutex m_songCallbackLock;

		std::shared_ptr<SongIdMapper> m_songMapper;
		std::shared_ptr<LocalMusicStore::LocalMusicCache> m_musicCache;
		std::shared_ptr<LocalMusicStore::MusicFinder> m_musicFinder;

		MusicProviderSubscription m_albumSubscription;

		bool m_isLoading = true;
		bool m_hasSongLoadingBegun = false;
		bool m_hasAlbumLoadingBegun = false;
		std::mutex m_isLoadingLock;
	};
}
}

#endif