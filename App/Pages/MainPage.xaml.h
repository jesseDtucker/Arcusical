#pragma once

#include "AsyncProcessor.hpp"
#include "Pages/MainPage.g.h"
#include "Subscription.hpp"
#include "ViewModels/AlbumListControlVM.hpp"
#include "ViewModels/GuideVM.hpp"
#include "ViewModels/SearchVM.hpp"
#include "ViewModels/SongListControlVM.hpp"
#include "ViewModels/SongPlayerVM.hpp"
#include "ViewModels/VolumeSilderVM.hpp"

namespace Arcusical {
namespace MusicProvider {
class MusicSearcher;
class MusicProvider;
}
namespace Player {
class IPlayer;
class Playlist;
}
}

namespace Arcusical {
/// <summary>
/// An empty page that can be used on its own or navigated to within a Frame.
/// </summary>
public
ref class MainPage sealed {
 public:
  MainPage();
  internal : void SetDependencies(MusicProvider::MusicSearcher* musicSearcher,
                                  MusicProvider::MusicProvider* musicProvider, Player::IPlayer* player,
                                  Player::Playlist* playlist, Util::BackgroundWorker* worker);

 private:
  void SetupTransportControls(Player::IPlayer* player);
  void OnTransportControlButtonPressed(Windows::Media::SystemMediaTransportControls ^ sender,
                                       Windows::Media::SystemMediaTransportControlsButtonPressedEventArgs ^ args);

  void OnAlbumsReady(const Model::AlbumCollectionChanges& albums);

  std::wstring oldImagePath;

  Util::Subscription m_isPlayingSub;
  Util::Subscription m_songChangedSub;
  Util::Subscription m_isEndedSub;

  MusicProvider::MusicSearcher* m_searcher;
  MusicProvider::MusicProvider* m_musicProvider;
  Player::Playlist* m_playlist;
  Player::IPlayer* m_player;
  Util::BackgroundWorker* m_backgroundWorker;

  ViewModel::AlbumListControlVM ^ m_albumListVM;
  ViewModel::GuideVM ^ m_guideVM;
  ViewModel::SearchVM ^ m_searchVM;
  ViewModel::VolumeSliderVM ^ m_volumeSlideVM;
  ViewModel::SongPlayerVM ^ m_playerVM;
  ViewModel::WhatIsPlayingVM ^ m_whatIsPlayingVM;

  Util::Subscription m_albumSub;
  Util::Subscription m_albumSelectedSub;
  void KeyPressed(Windows::UI::Core::CoreWindow ^ window, Windows::UI::Core::KeyEventArgs ^ e);
};
}
