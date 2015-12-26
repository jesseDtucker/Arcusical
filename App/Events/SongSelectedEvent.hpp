#pragma once
#ifndef SONG_SELECTED_EVENT_HPP
#define SONG_SELECTED_EVENT_HPP

namespace Arcusical {
namespace ViewModel {
ref class SongVM;
}
namespace Events {

class SongSelectedEvent final {
 public:
  SongSelectedEvent(ViewModel::SongVM ^ selectedSong);
  ViewModel::SongVM ^ GetSelectedSong() const;

 private:
  ViewModel::SongVM ^ m_selectedSong;
};
}
}

#endif