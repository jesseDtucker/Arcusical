#include "pch.h"

#include "ViewModels/SongListVM.hpp"

#include <algorithm>
#include <future>
#include <vector>

#include "Arc_Assert.hpp"
#include "Song.hpp"
#include "ViewModels/SongVM.hpp"

using namespace std;
using namespace Arcusical::Player;
using namespace Util;

namespace Arcusical {
namespace ViewModel {

SongListVM::SongListVM(const std::vector<Model::Song>& songs, Playlist& playlist, IPlayer& player,
                       BackgroundWorker& worker)
    : m_currentSong(nullptr) {
  auto future = Arcusical::DispatchToUI([this, &songs, &player, &playlist, &worker]() {
    List = ref new Platform::Collections::Vector<SongVM ^>();

    bool isAlternate = false;
    auto currentPlayingSong = player.GetCurrentSong();

    for (auto& song : songs) {
      auto songVM = ref new SongVM(song, playlist, player, worker);
      songVM->IsAlternate = isAlternate;
      isAlternate = !isAlternate;
      if (currentPlayingSong != nullptr && *currentPlayingSong == song) {
        m_currentSong = songVM;
        songVM->IsPlaying = true;
      }
      List->Append(songVM);
    }
  });

  m_playerSubscription = player.GetSongChanged() += [this](const boost::optional<Model::Song>& playingSong) {
    DispatchToUI([this, playingSong]() {
      if (m_currentSong) {
        m_currentSong->IsPlaying = false;
      }
      if (playingSong) {
        auto playingSongVMItr = find_if(begin(List), end(List),
                                        [&playingSong](SongVM ^ song) { return *playingSong == *song->GetModel(); });
        if (playingSongVMItr != end(List)) {
          m_currentSong = *playingSongVMItr;
          m_currentSong->IsPlaying = true;
        }
      }
    });
  };

  future.get();  // wait for the above code to complete!
}
}
}