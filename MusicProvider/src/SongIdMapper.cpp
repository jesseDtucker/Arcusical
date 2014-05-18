#include "pch.h"

#include "SongIdMapper.hpp"

#include <algorithm>

#include "Arc_Assert.hpp"
#include "Song.hpp"

namespace Arcusical
{
namespace MusicProvider
{
	SongIdMapper::SongIdMapper(GetSongsCall getLocalSongs, GetSongsCall getRemoteSongs)
		: m_getLocalSongs(getLocalSongs)
		, m_getRemoteSongs(getRemoteSongs)
	{
		
	}

	std::unordered_map<boost::uuids::uuid, std::shared_ptr<Model::Song>> SongIdMapper::GetSongsFromIds(const std::set<boost::uuids::uuid>& ids)
	{
		std::unordered_map<boost::uuids::uuid, std::shared_ptr<Model::Song>> results;

		auto localSongs = m_getLocalSongs().lock();
		ARC_ASSERT_MSG(localSongs != nullptr, "Local songs returned a null ptr! This should never happen!");

		if (localSongs != nullptr)
		{
			for (auto& id : ids)
			{
				if (localSongs->find(id) != localSongs->end())
				{
					results[id] = localSongs->at(id);
				}
			}
		}

		return results;
	}
}
}