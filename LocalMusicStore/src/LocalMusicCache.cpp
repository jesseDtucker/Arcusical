#include "LocalMusicCache.hpp"

#include <codecvt>
#include "boost\uuid\uuid_io.hpp"
#include "boost\uuid\string_generator.hpp"

#include "Album.hpp"
#include "Arc_Assert.hpp"
#include "IFolder.hpp"
#include "IFile.hpp"
#include "IAlbumToSongMapper.hpp"
#include "Storage.hpp"
#include "Song.hpp"

#include "CachedSong.pb.h"
#include "CachedAlbum.pb.h"
#include "CachedAlbumList.pb.h"
#include "CachedSongList.pb.h"

using namespace FileSystem;

namespace Arcusical
{
	namespace LocalMusicStore
	{
		const std::wstring LocalMusicCache::ALBUM_CACHE_FILE = L"albums.cache";
		const std::wstring LocalMusicCache::SONG_CACHE_FILE = L"songs.cache";

		static const std::unordered_map<LocalMusicStore::Encoding, Model::AudioFormat> CACHE_ENCODING_TO_MODEL =
		{
			{ LocalMusicStore::UNKNOWN, Model::AudioFormat::UNKNOWN },
			{ LocalMusicStore::AAC, Model::AudioFormat::AAC},
			{ LocalMusicStore::ALAC, Model::AudioFormat::ALAC},
			{ LocalMusicStore::MP3, Model::AudioFormat::MP3},
			{ LocalMusicStore::FLAC, Model::AudioFormat::FLAC}
		};

		static const std::unordered_map<Model::AudioFormat, LocalMusicStore::Encoding> MODEL_ENCODING_TO_CACHE =
		{
			{ Model::AudioFormat::UNKNOWN, LocalMusicStore::UNKNOWN},
			{ Model::AudioFormat::AAC, LocalMusicStore::AAC},
			{ Model::AudioFormat::ALAC, LocalMusicStore::ALAC},
			{ Model::AudioFormat::MP3, LocalMusicStore::MP3},
			{ Model::AudioFormat::FLAC,  LocalMusicStore::FLAC}
		};

		LocalMusicCache::LocalMusicCache(std::shared_ptr<Model::IAlbumToSongMapper>& songMapper)
			: m_songMapper(songMapper)
			, m_localSongs(std::make_shared<std::unordered_map<boost::uuids::uuid, std::shared_ptr<Model::Song>>>())
			, m_localAlbums(std::make_shared<std::unordered_map<boost::uuids::uuid, std::shared_ptr<Model::Album>>>())
			, m_areSongsLoaded(false)
			, m_areAlbumsLoaded(false)
		{
			std::async([this](){this->LoadSongs(); });
		}

		std::future<std::weak_ptr<std::unordered_map<boost::uuids::uuid, std::shared_ptr<Model::Song>>>> LocalMusicCache::GetLocalSongs()
		{
			auto result = std::async([this]()->std::weak_ptr<std::unordered_map<boost::uuids::uuid, std::shared_ptr<Model::Song>>>
			{
				std::unique_lock<std::mutex> lock(m_songsLoadingLock);
				if (!m_areSongsLoaded)
				{
					// if we haven't loaded, then wait until we have
					m_songLoading.wait(lock);
				}

				return m_localSongs;
			});

			return result;
		}

		std::future<std::weak_ptr<std::unordered_map<boost::uuids::uuid, std::shared_ptr<Model::Album>>>> LocalMusicCache::GetLocalAlbums()
		{
			auto result = std::async([this]()->std::weak_ptr<std::unordered_map<boost::uuids::uuid, std::shared_ptr<Model::Album>>>
			{
				std::unique_lock<std::mutex> lock(m_albumsLoadingLock);
				if (!m_areAlbumsLoaded)
				{
					// if we haven't loaded, then wait until we have
					m_albumsLoading.wait(lock);
				}

				return m_localAlbums;
			});

			return result;
		}

		void LocalMusicCache::ClearCache()
		{
			m_localAlbums->clear();
			m_localSongs->clear();
		}

		void LocalMusicCache::AddToCache(const std::vector<std::shared_ptr<Model::Album>>& albums)
		{
			for (auto& album : albums)
			{
				(*m_localAlbums)[album->GetId()] = album;
			}
		}

		void LocalMusicCache::AddToCache(const std::vector<std::shared_ptr<Model::Song>>& songs)
		{
			for (auto& song : songs)
			{
				(*m_localSongs)[song->GetId()] = song;
			}
		}

		void LocalMusicCache::SaveCache() const
		{
			SaveAlbums();
			SaveSongs();
		}

		void LocalMusicCache::LoadAlbums()
		{
			if (Storage::ApplicationFolder().ContainsFile(ALBUM_CACHE_FILE))
			{
				std::vector<unsigned char> buffer;
				auto albumCacheFile = Storage::ApplicationFolder().GetFile(ALBUM_CACHE_FILE);
				albumCacheFile->ReadFromFile(buffer);

				CachedAlbumList cachedAlbums;
				cachedAlbums.ParseFromArray(buffer.data(), buffer.size());

				for (int i = 0; i < cachedAlbums.albums_size(); ++i)
				{
					auto album = std::make_shared<Model::Album>(m_songMapper);
					FillInModelAlbumFromCached(*album, cachedAlbums.albums(i));
					(*m_localAlbums)[album->GetId()] = album;
				}
			}

			std::unique_lock<std::mutex> lock(m_albumsLoadingLock);
			m_areAlbumsLoaded = true;
			m_albumsLoading.notify_all();
		}

		void LocalMusicCache::LoadSongs()
		{
			if (Storage::ApplicationFolder().ContainsFile(SONG_CACHE_FILE))
			{
				std::vector<unsigned char> buffer;
				auto songCacheFile = Storage::ApplicationFolder().GetFile(SONG_CACHE_FILE);
				songCacheFile->ReadFromFile(buffer);

				CachedSongList cachedSongs;
				cachedSongs.ParseFromArray(buffer.data(), buffer.size());

				for (int i = 0; i < cachedSongs.songs_size(); ++i)
				{
					auto song = std::make_shared<Model::Song>();
					FillInModelSongFromCached(*song, cachedSongs.songs(i));
					(*m_localSongs)[song->GetId()] = song;
				}
			}

			std::unique_lock<std::mutex> lock(m_songsLoadingLock);
			m_areSongsLoaded = true;
			m_songLoading.notify_all();
		}

		void LocalMusicCache::SaveAlbums() const
		{
			CachedAlbumList cachedAlbums;

			for (auto& album : *m_localAlbums)
			{
				auto cachedAlbum = cachedAlbums.add_albums();
				FillInCachedAlbumFromModel(*cachedAlbum, *album.second);
			}

			// ensure we actually have contents to write to file
			if (cachedAlbums.ByteSize() > 0)
			{
				std::vector<unsigned char> buffer(cachedAlbums.ByteSize());
				auto result = cachedAlbums.SerializeToArray(buffer.data(), buffer.size());
				ARC_ASSERT_MSG(result, "Failed to serialize album list!");

				auto albumCacheFile = Storage::ApplicationFolder().CreateNewFile(LocalMusicCache::ALBUM_CACHE_FILE);
				albumCacheFile->WriteToFile(buffer);
			}
		}

		void LocalMusicCache::SaveSongs() const
		{
			CachedSongList cachedSongs;

			for (auto& song : *m_localSongs)
			{
				auto cachedSong = cachedSongs.add_songs();
				FillInCachedSongFromModel(*cachedSong, *song.second);
			}

			// ensure we actually have contents to write to file
			if (cachedSongs.ByteSize() > 0)
			{
				std::vector<unsigned char> buffer(cachedSongs.ByteSize());
				auto result = cachedSongs.SerializeToArray(buffer.data(), buffer.size());
				ARC_ASSERT_MSG(result, "Failed to serialize song list!");

				auto songCacheFile = Storage::ApplicationFolder().CreateNewFile(SONG_CACHE_FILE);
				songCacheFile->WriteToFile(buffer);
			}
		}

		void LocalMusicCache::FillInCachedAlbumFromModel(CachedAlbum& cachedAlbum, Model::Album& modelAlbum) const
		{
			// set the id
			auto guid = cachedAlbum.mutable_id();
			auto rawBytes = guid->mutable_rawdata();
			SerializeGuid(rawBytes, modelAlbum.GetId());

			//setup converter
			typedef std::codecvt_utf8<wchar_t> convert_type;
			std::wstring_convert<convert_type, wchar_t> converter;

			// apply strings
			cachedAlbum.set_title(converter.to_bytes(modelAlbum.GetTitle()));
			cachedAlbum.set_artist(converter.to_bytes(modelAlbum.GetArtist()));
			cachedAlbum.set_imagefile(converter.to_bytes(modelAlbum.GetImageFilePath()));

			// add ids
			for (const auto& song : modelAlbum.GetSongs())
			{
				auto songGuid = cachedAlbum.add_songlist();
				SerializeGuid(songGuid->mutable_rawdata(), song.first);
			}
		}

		void LocalMusicCache::FillInCachedSongFromModel(CachedSong& cachedSong, const Model::Song& modelSong) const
		{
			// set the id
			auto guid = cachedSong.mutable_id();
			auto rawBytes = guid->mutable_rawdata();
			SerializeGuid(rawBytes, modelSong.GetId());

			//setup converter
			std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;

			// apply strings
			cachedSong.set_title(converter.to_bytes(modelSong.GetTitle()));
			cachedSong.set_artist(converter.to_bytes(modelSong.GetArtist()));

			cachedSong.set_length(modelSong.GetLength());

			for (const auto& file : modelSong.GetFiles())
			{
				auto songFile = cachedSong.add_files();

				auto cacheEncoding = MODEL_ENCODING_TO_CACHE.find(file.first);
				ARC_ASSERT_MSG(cacheEncoding != std::end(MODEL_ENCODING_TO_CACHE), "Attempted to cache a format that doesn't have any cache equivelent!");
				if (cacheEncoding != std::end(MODEL_ENCODING_TO_CACHE))
				{
					songFile->set_encoding((*cacheEncoding).second);
				}
				else
				{
					songFile->set_encoding(LocalMusicStore::UNKNOWN);
				}
				
				songFile->set_file(converter.to_bytes(file.second.filePath));
				songFile->set_bitrate(file.second.bitRate);
				songFile->set_samplesize(file.second.sampleSize);
				songFile->set_channelcount(file.second.channelCount);
			}
		}

		void LocalMusicCache::SerializeGuid(std::string* rawBytes, const boost::uuids::uuid guid) const
		{
			*rawBytes = boost::uuids::to_string(guid);
		}

		void LocalMusicCache::DeserializeGuid(const std::string* rawBytes, boost::uuids::uuid& guid) const
		{
			guid = boost::uuids::string_generator()(*rawBytes);
		}
		
		void LocalMusicCache::FillInModelAlbumFromCached(Model::Album& modelAlbum, const CachedAlbum& cachedAlbum) const
		{
			boost::uuids::uuid id;
			DeserializeGuid(&cachedAlbum.id().rawdata(), id);
			modelAlbum.SetId(id);

			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
			modelAlbum.SetArtist(converter.from_bytes(cachedAlbum.artist()));
			modelAlbum.SetTitle(converter.from_bytes(cachedAlbum.title()));
			modelAlbum.SetImageFilePath(converter.from_bytes(cachedAlbum.imagefile()));

			for (int i = 0; i < cachedAlbum.songlist_size(); ++i)
			{
				auto cachedSongId = cachedAlbum.songlist(i);
				boost::uuids::uuid songId;
				DeserializeGuid(&cachedSongId.rawdata(), songId);
				modelAlbum.GetMutableSongIds().insert(songId);
			}
		}

		void LocalMusicCache::FillInModelSongFromCached(Model::Song& modelSong, const CachedSong& cachedSong) const
		{
			boost::uuids::uuid songId;
			DeserializeGuid(&cachedSong.id().rawdata(), songId);
			modelSong.SetId(songId);

			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
			modelSong.SetArtist(converter.from_bytes(cachedSong.artist()));
			modelSong.SetLength(cachedSong.length());
			modelSong.SetTitle(converter.from_bytes(cachedSong.title()));
			
			for (const auto& cachedFile : cachedSong.files())
			{
				Model::AudioFormat audioFormat = Model::AudioFormat::UNKNOWN;
				auto encodingItr = CACHE_ENCODING_TO_MODEL.find(cachedFile.encoding());

				ARC_ASSERT_MSG(encodingItr != std::end(CACHE_ENCODING_TO_MODEL), "Found a format in cache that isn't in the model map!")
				if (encodingItr != std::end(CACHE_ENCODING_TO_MODEL))
				{
					audioFormat = (*encodingItr).second;
				}
				Arcusical::Model::SongFile songFile;
				songFile.filePath = converter.from_bytes(cachedFile.file());
				songFile.bitRate = cachedFile.bitrate();
				songFile.channelCount = cachedFile.channelcount();
				songFile.sampleSize = cachedFile.samplesize();
				modelSong.AddFile(audioFormat, songFile);
			}
		}


}
}