#pragma once

#include "AsyncProcessor.hpp"
#include "Subscription.hpp"
#include "Utility/XamlMacros.hpp"

namespace Arcusical {
namespace Events {
class AlbumSelectedEvent;
}
namespace ViewModel {

ref class SongListVM;

[Windows::UI::Xaml::Data::Bindable] public ref class SongListControlVM sealed : INotifyPropertyChanged_t {
 public:
  NOTIFY_PROPERTY_CHANGED_IMPL;

  PROP_SET_AND_GET_WINRT(SongListVM ^, SongList);

  void PlaySongsAfterAndIncluding(SongVM ^ song);

  internal : SongListControlVM(Player::Playlist& playlist, Util::BackgroundWorker& worker);

 private:
  Player::Playlist& m_playlist;
  Util::BackgroundWorker& m_worker;
};
}
}
