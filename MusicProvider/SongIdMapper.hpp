#pragma once
#ifndef SONG_ID_MAPPER

#include <functional>
#include <memory>
#include <vector>

#include "IAlbumToSongMapper.hpp"

namespace Arcusical
{
namespace MusicProvider
{
	class SongIdMapper final : public Model::IAlbumToSongMapper
	{
	public:
		SongIdMapper(const std::function<std::vector<std::shared_ptr<Model::Song>>&()> getSongsFunction);

		virtual const std::vector<std::shared_ptr<Model::Song>>& GetSongsFromIds(const std::vector<boost::uuids::uuid>& ids);
	private:
		std::function<std::vector<std::shared_ptr<Model::Song>>&()> m_getSongsFunction;
		int test;
	};
}
}

#endif