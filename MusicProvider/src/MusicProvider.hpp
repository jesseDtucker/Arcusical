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
		virtual MusicProviderSubscription Subscribe(MusicChangedCallback callback) override;
	private:

		void Unsubscribe(MusicChangedCallback callback);
		void LoadSongs();
		void LoadAlbums();
		void Publish(SongListPtr localSongs, SongListPtr remoteSongs);

		bool MergeSongCollections(std::unordered_map<boost::uuids::uuid, std::shared_ptr<Arcusical::Model::Song>>& existingSongs, std::vector<std::shared_ptr<FileSystem::IFile>>& locatedFiles);
		void AddNewSongToExisting(std::shared_ptr<Model::Song> newSong, std::shared_ptr<Model::Song> existingSong, std::wstring fullPath);

		std::set<MusicChangedCallback> m_callbackSet;
		std::shared_ptr<SongIdMapper> m_songMapper;
		std::shared_ptr<LocalMusicStore::LocalMusicCache> m_musicCache;
		std::shared_ptr<LocalMusicStore::MusicFinder> m_musicFinder;

		std::atomic<bool> m_isLoading = true;
		std::atomic<bool> m_hasLoadingBegun = false;
		std::mutex m_isLoadingLock;

		SongLoader m_songLoader;

		// hack for now
		std::shared_ptr<std::unordered_map<boost::uuids::uuid, std::shared_ptr<Arcusical::Model::Song>>> m_hackRemoteList;
	};
}
}

#endif