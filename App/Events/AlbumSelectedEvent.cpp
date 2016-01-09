#include "pch.h"

#include "Events/AlbumSelectedEvent.hpp"
#include "ViewModels/AlbumVM.hpp"

namespace Arcusical {
namespace Events {
AlbumSelectedEvent::AlbumSelectedEvent(ViewModel::AlbumVM ^ selectedSong) : m_selectedAlbum(selectedSong) {}

ViewModel::AlbumVM ^ AlbumSelectedEvent::GetSelectedAlbum() const { return m_selectedAlbum; }
}
}