#pragma once

#include <memory>

#include "PropertyHelper.hpp"
#include "Utility/XamlMacros.hpp"
#include "ViewModels/AlbumVM.hpp"
#include "ViewModels/SongListControlVM.hpp"

namespace Arcusical {
namespace ViewModel {

[Windows::UI::Xaml::Data::Bindable] public ref class WhatIsPlayingVM sealed : INotifyPropertyChanged_t {
 public:
  NOTIFY_PROPERTY_CHANGED_IMPL;

  PROP_SET_AND_GET_WINRT(ViewModel::SongListControlVM ^, SongListControlVM);
  PROP_SET_AND_GET_WINRT(ViewModel::AlbumVM ^, ActiveAlbum);
  PROP_SET_AND_GET_WINRT(Platform::String ^, HeaderTitle);
  internal : WhatIsPlayingVM(Player::Playlist& playlist, Player::IPlayer& player, Util::BackgroundWorker& worker,
                             MusicProvider::MusicProvider& musicProvider);

 private:
  void OnPlaylistChanged();
  void OnSongChanged(const boost::optional<Model::Song>&);

  Player::Playlist& m_playlist;
  Player::IPlayer& m_player;
  Util::BackgroundWorker& m_worker;
  MusicProvider::MusicProvider& m_musicProvider;
  Util::Subscription m_playlistChangedSub;
  Util::Subscription m_songPlayingChangedSub;
};
}
}
