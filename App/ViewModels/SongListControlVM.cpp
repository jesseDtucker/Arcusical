#include "pch.h"

#include "SongListControlVM.hpp"

#include <algorithm>
#include <random>
#include <vector>

#include "Arc_Assert.hpp"
#include "Events/AlbumSelectedEvent.hpp"
#include "Events/EventService.hpp"
#include "Playlist.hpp"
#include "SongListVM.hpp"

using namespace std;
using namespace Windows::UI::Xaml::Data;

using namespace Arcusical::Events;
using namespace Arcusical::Model;
using namespace Arcusical::Player;
using namespace Util;

namespace Arcusical {
namespace ViewModel {

SongListControlVM::SongListControlVM(Playlist& playlist, BackgroundWorker& worker)
    : m_playlist(playlist), m_worker(worker) {}

void SongListControlVM::PlaySongsAfterAndIncluding(SongVM ^ songToStartAt) {
  auto songs = SongList->List;
  m_worker.Append([this, songToStartAt, songs]() {
    bool skippedToSong = m_playlist.SkipTo(*songToStartAt->GetModel());
    if (!skippedToSong) {
      auto songToStartAtItr =
          find_if(begin(songs), end(songs), [songToStartAt](SongVM ^ song) { return songToStartAt == song; });
      if (songToStartAtItr != end(songs)) {
        vector<Song> songsToPlay;
        songsToPlay.reserve(distance(songToStartAtItr, end(songs)));
        transform(songToStartAtItr, end(songs), back_inserter(songsToPlay),
                  [](SongVM ^ songVM) { return *songVM->GetModel(); });
        m_playlist.Clear();
        m_playlist.Enqueue(songsToPlay);
      }
    }
  });
}
}
}