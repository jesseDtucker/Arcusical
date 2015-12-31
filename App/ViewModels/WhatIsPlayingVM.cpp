#include "pch.h"

#include "WhatIsPlayingVM.hpp"

#include "IPlayer.hpp"
#include "Playlist.hpp"

using namespace std;

using namespace Arcusical;
using namespace Arcusical::ViewModel;
using namespace Arcusical::Player;
using namespace Util;

WhatIsPlayingVM::WhatIsPlayingVM(Playlist& playlist, IPlayer& player, BackgroundWorker& worker,
                                 MusicProvider::MusicProvider& musicProvider)
    : m_playlist(playlist), m_worker(worker), m_player(player), m_musicProvider(musicProvider) {
  this->SongListControlVM = ref new ViewModel::SongListControlVM(playlist, worker);
  m_playlistChangedSub = playlist.PlaylistChanged += [this]() { this->OnPlaylistChanged(); };
  m_songPlayingChangedSub = player.GetSongChanged() += [this](auto song) { this->OnSongChanged(song); };
}

void WhatIsPlayingVM::OnPlaylistChanged() {
  auto& recentlyPlayed = m_playlist.GetRecentlyPlayed();
  auto& upNext = m_playlist.GetSongQueue();
  auto songCount = recentlyPlayed.size() + upNext.size();
  if (songCount == 0) {
    return;
  }

  vector<Model::Song> allSongs;
  allSongs.reserve(songCount);
  allSongs.insert(end(allSongs), begin(recentlyPlayed), end(recentlyPlayed));
  allSongs.insert(end(allSongs), rbegin(upNext), rend(upNext));

  DispatchToUI([
    this,
    allSongs = move(allSongs)
  ]() { this->SongListControlVM->SongList = ref new SongListVM(allSongs, m_playlist, m_player, m_worker); });
}

void WhatIsPlayingVM::OnSongChanged(const boost::optional<Model::Song>& song) {
  AlbumVM ^ albumVM = nullptr;
  if (song) {
    auto& currentAlbum = m_musicProvider.GetAlbum(song->GetAlbumName());
    if (currentAlbum) {
      albumVM = ref new AlbumVM(*currentAlbum, m_playlist, m_player, m_worker);
    }
  }
  DispatchToUI([this, albumVM]() {
    ActiveAlbum = albumVM;
    if (albumVM) {
      HeaderTitle = albumVM->Title;
    }
  });
}