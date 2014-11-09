#pragma once
#ifndef SONG_ID_MAPPER
#define SONG_ID_MAPPER

#include <functional>
#include <memory>
#include <set>
#include <unordered_map>

#include "IAlbumToSongMapper.hpp"
#include "IMusicProvider.hpp"
#include "MusicTypes.hpp"

namespace Arcusical
{
namespace MusicProvider
{
	class SongIdMapper final : public Model::IAlbumToSongMapper
	{
	public:

		SongIdMapper(const SongIdMapper&) = delete;
		SongIdMapper& operator=(const SongIdMapper&) = delete;

		typedef const std::function<Model::SongCollectionPtr()> GetSongsCall;
		SongIdMapper(GetSongsCall getLocalSongs);

		std::unordered_map<boost::uuids::uuid, Model::Song*> GetSongsFromIds(const std::set<boost::uuids::uuid>& ids) override;
	private:
		GetSongsCall m_getLocalSongs;
	};
}
}

#endif