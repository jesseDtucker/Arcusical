#pragma once
#ifndef SONG_SELECTED_EVENT_HPP

#include "PropertyHelper.hpp"

namespace Arcusical
{
namespace ViewModel
{
	ref class SongVM;
}
namespace Events
{

	class SongSelectedEvent final
	{
	public:
		SongSelectedEvent(ViewModel::SongVM^ selectedSong);
		ViewModel::SongVM^ GetSelectedSong() const;

	private:
		ViewModel::SongVM^ m_selectedSong;
	};
}
}

#endif