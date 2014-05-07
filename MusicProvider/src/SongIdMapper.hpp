#pragma once
#ifndef SONG_ID_MAPPER

#include <functional>
#include <memory>
#include <set>
#include <unordered_map>

#include "IAlbumToSongMapper.hpp"

namespace Arcusical
{
namespace MusicProvider
{
	class SongIdMapper final : public Model::IAlbumToSongMapper
	{
	public:

		SongIdMapper(const SongIdMapper&) = delete;
		SongIdMapper& operator=(const SongIdMapper&) = delete;

		typedef const std::function<std::weak_ptr<std::unordered_map<boost::uuids::uuid, std::shared_ptr<Model::Song>>>()> GetSongsCall;
		SongIdMapper(GetSongsCall getLocalSongs, GetSongsCall getRemoteSongs);

		virtual std::unordered_map<boost::uuids::uuid, std::shared_ptr<Model::Song>> GetSongsFromIds(const std::set<boost::uuids::uuid>& ids);
	private:
		GetSongsCall m_getLocalSongs;
		GetSongsCall m_getRemoteSongs;
	};
}
}

#endif