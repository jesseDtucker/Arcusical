#include "pch.h"

#include "Events/SongSelectedEvent.hpp"
namespace Arcusical {
namespace Events {
SongSelectedEvent::SongSelectedEvent(ViewModel::SongVM ^ selectedSong) : m_selectedSong(selectedSong) {}

ViewModel::SongVM ^ SongSelectedEvent::GetSelectedSong() const { return m_selectedSong; }
}
}