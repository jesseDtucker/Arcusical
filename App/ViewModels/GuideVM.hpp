#pragma once
#ifndef GUIDE_VM_HPP
#define GUIDE_VM_HPP

#include <memory>

#include "PropertyHelper.hpp"
#include "Utility/XamlMacros.hpp"
#include "ViewModels/AlbumVM.hpp"
#include "ViewModels/SearchVM.hpp"
#include "ViewModels/SongListControlVM.hpp"

namespace Arcusical {
namespace ViewModel {

[Windows::UI::Xaml::Data::Bindable] public ref class GuideVM sealed : INotifyPropertyChanged_t {
 public:
  NOTIFY_PROPERTY_CHANGED_IMPL;

  property ViewModel::AlbumVM ^ SelectedAlbum {
    ViewModel::AlbumVM ^ get();
    void set(ViewModel::AlbumVM ^ );
  }
  PROP_SET_AND_GET_WINRT(ViewModel::SongListControlVM ^, SongListControlVM);

  void PlayAll();
  void Shuffle();
internal:
  GuideVM(Player::Playlist& playlist, Util::BackgroundWorker& worker);

 private:
  ViewModel::AlbumVM ^ m_selectedAlbum;
  Player::Playlist& m_playlist;
  Util::BackgroundWorker& m_worker;
};
}
}

#endif