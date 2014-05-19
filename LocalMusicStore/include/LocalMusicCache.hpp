#ifndef LOCAL_MUSIC_CACHE_HPP
#define LOCAL_MUSIC_CACHE_HPP

#include <atomic>
#include <condition_variable>
#include <future>
#include <memory>
#include <mutex>
#include <unordered_map>

#include "boost\uuid\uuid.hpp"

namespace Arcusical{
namespace Model
{
	class IAlbumToSongMapper;
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

	class LocalMusicCache final
	{
	public:
		LocalMusicCache(std::shared_ptr<Model::IAlbumToSongMapper>& songMapper);

		// TODO::JT must modify this to take a callback instead. This has some thread safety issues atm
		std::future<std::weak_ptr<const std::unordered_map<boost::uuids::uuid, std::shared_ptr<Model::Song>>>> GetLocalSongs();
		std::future<std::weak_ptr<const std::unordered_map<boost::uuids::uuid, std::shared_ptr<Model::Album>>>> GetLocalAlbums();

		void ClearCache();
		void AddToCache(const std::vector<std::shared_ptr<Model::Album>>& albums);
		void AddToCache(const std::vector<std::shared_ptr<Model::Song>>& songs);

		void SaveAlbums();
		void SaveSongs();

	private:

		void LoadAlbums();
		void LoadSongs();

		void FillInCachedAlbumFromModel(CachedAlbum& cachedAlbum, Model::Album& modelAlbum) const;
		void FillInCachedSongFromModel(CachedSong& cachedSong, const Model::Song& modelSong) const;

		void FillInModelAlbumFromCached(Model::Album& modelAlbum, const CachedAlbum& cachedAlbum) const;
		void FillInModelSongFromCached(Model::Song& modelSong, const CachedSong& cachedSong) const;

		void SerializeGuid(std::string* rawBytes, const boost::uuids::uuid guid) const;
		void DeserializeGuid(const std::string* rawBytes, boost::uuids::uuid& guid) const;

		std::shared_ptr<std::unordered_map<boost::uuids::uuid, std::shared_ptr<Model::Album>>> m_localAlbums;
		std::shared_ptr<std::unordered_map<boost::uuids::uuid, std::shared_ptr<Model::Song>>> m_localSongs;

		bool m_areSongsLoaded;
		bool m_areAlbumsLoaded;

		std::condition_variable m_songLoading;
		std::condition_variable m_albumsLoading;

		std::mutex m_songsLoadingLock;
		std::mutex m_albumsLoadingLock;

		std::mutex m_albumsEditLock;
		std::mutex m_songsEditLock;

		std::shared_ptr<Model::IAlbumToSongMapper> m_songMapper;

		const static std::wstring ALBUM_CACHE_FILE;
		const static std::wstring SONG_CACHE_FILE;
	};
}
}

#endif