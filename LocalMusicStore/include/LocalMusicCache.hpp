#ifndef LOCAL_MUSIC_CACHE_HPP
#define LOCAL_MUSIC_CACHE_HPP

#include <atomic>
#include <condition_variable>
#include <future>
#include <memory>
#include <mutex>
#include <windows.h>
#include <unordered_map>

#include "boost\uuid\uuid.hpp"
#include "IAlbumToSongMapper.hpp"
#include "LockHelper.hpp"
#include "MusicTypes.hpp"

namespace Arcusical{
namespace Model
{
	class Song;
	class Album;
}
}

namespace Arcusical
{
namespace LocalMusicStore
{
	class CachedAlbum;
	class CachedSong;

	class LocalMusicCache final : public Model::IAlbumToSongMapper
	{
	public:
		LocalMusicCache();
		LocalMusicCache(const LocalMusicCache& rhs);
		LocalMusicCache& operator=(const LocalMusicCache& rhs);

		Model::SongCollectionLockedPtr GetLocalSongs() const;
		Model::AlbumCollectionLockedPtr GetLocalAlbums() const;

		void ClearCache();
		void AddToCache(const std::vector<Model::Album>& albums);
		void AddToCache(const std::vector<Model::Song>& songs);

		void SaveAlbums() const;
		void SaveSongs() const;

		virtual std::unordered_map<boost::uuids::uuid, Model::Song> GetSongsFromIds(const std::set<boost::uuids::uuid>& ids) const override;

	private:

		void LoadAlbums();
		void LoadSongs();

		Model::AlbumCollection m_localAlbums;
		Model::SongCollection m_localSongs;

		bool m_areSongsLoaded = false;
		bool m_areAlbumsLoaded = false;

		mutable std::condition_variable m_songLoading;
		mutable std::condition_variable m_albumsLoading;

		mutable std::mutex m_songsLoadingLock;
		mutable std::mutex m_albumsLoadingLock;

		mutable Util::SlimRWLock m_albumsEditLock;
		mutable Util::SlimRWLock m_songsEditLock;

		const static std::wstring ALBUM_CACHE_FILE;
		const static std::wstring SONG_CACHE_FILE;
	};
}
}

#endif