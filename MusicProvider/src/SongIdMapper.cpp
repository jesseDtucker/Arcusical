#include "pch.h"

#include "SongIdMapper.hpp"

#include <algorithm>

#include "Arc_Assert.hpp"
#include "Song.hpp"

namespace Arcusical
{
namespace MusicProvider
{
	SongIdMapper::SongIdMapper(GetSongsCall getLocalSongs)
		: m_getLocalSongs(getLocalSongs)
	{
		
	}

	std::unordered_map<boost::uuids::uuid, Model::Song*> SongIdMapper::GetSongsFromIds(const std::set<boost::uuids::uuid>& ids)
	{
		std::unordered_map<boost::uuids::uuid, Model::Song*> results;

		auto localSongs = m_getLocalSongs();
		ARC_ASSERT_MSG(localSongs != nullptr, "Local songs returned a null ptr! This should never happen!");

		if (localSongs != nullptr)
		{
			for (auto& id : ids)
			{
				if (localSongs->find(id) != localSongs->end())
				{
					results[id] = &localSongs->at(id);
				}
			}
		}

		return results;
	}
}
}