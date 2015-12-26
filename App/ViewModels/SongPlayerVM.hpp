#pragma once
#ifndef SONG_PLAYER_VM_HPP
#define SONG_PLAYER_VM_HPP

#include "AsyncProcessor.hpp"
#include "PropertyHelper.hpp"
#include "Subscription.hpp"
#include "Utility/XamlMacros.hpp"
#include "ViewModels/AlbumVM.hpp"
#include "ViewModels/SongVM.hpp"
#include "ViewModels/VolumeSilderVM.hpp"

namespace Arcusical {
namespace Player {
class IPlayer;
class Playlist;
}
}
namespace Arcusical {
namespace MusicProvider {
class MusicProvider;
}
}

namespace Arcusical {
namespace ViewModel {
[Windows::UI::Xaml::Data::Bindable] public ref class SongPlayerVM sealed : INotifyPropertyChanged_t {
 public:
  NOTIFY_PROPERTY_CHANGED_IMPL;

  PROP_SET_AND_GET_WINRT(SongVM ^, CurrentSong);
  PROP_SET_AND_GET_WINRT(double, AmountPlayed);    // seconds
  PROP_SET_AND_GET_WINRT(double, AmoutRemaining);  // seconds
  PROP_SET_AND_GET_WINRT(double, Length);
  PROP_SET_AND_GET_WINRT(Platform::Boolean, IsPlaying);
  PROP_SET_AND_GET_WINRT(ViewModel::VolumeSliderVM ^, VolumeVM);
  PROP_SET_AND_GET_WINRT(ViewModel::AlbumVM ^, Album);

  void Play();
  void Pause();
  void Previous();
  void Next();

  void ChangeTimeTo(double newTime);

internal:
  SongPlayerVM(Player::IPlayer& player, Player::Playlist& playlist, MusicProvider::MusicProvider& provider,
               Util::BackgroundWorker& worker);

 private:
  void UpdateTime(double amountPlayed, double duration);

  Util::Subscription m_durationSub;
  Util::Subscription m_timeUpdateSub;
  Util::Subscription m_playingSub;
  Util::Subscription m_songChangedSub;

  Player::IPlayer& m_player;
  Player::Playlist& m_playlist;
  MusicProvider::MusicProvider& m_provider;
  Util::BackgroundWorker& m_worker;
};
}
}

#endif