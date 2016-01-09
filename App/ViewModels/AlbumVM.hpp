#ifndef ALBUM_VM_HPP
#define ALBUM_VM_HPP

#include <memory>

#include "AsyncProcessor.hpp"
#include "PropertyHelper.hpp"
#include "Utility/DispatcherHelper.hpp"
#include "Utility/XamlMacros.hpp"
#include "ViewModels/SongListVM.hpp"

namespace Arcusical {
namespace Model {
class Album;
}
namespace Player {
class Playlist;
class IPlayer;
}
}

namespace Arcusical {
namespace ViewModel {

[Windows::UI::Xaml::Data::Bindable] public ref class AlbumVM sealed : INotifyPropertyChanged_t {
 public:
  NOTIFY_PROPERTY_CHANGED_IMPL;

  PROP_SET_AND_GET_WINRT(Platform::String ^, Title);
  PROP_SET_AND_GET_WINRT(Platform::String ^, Artist);
  PROP_SET_AND_GET_WINRT(Platform::String ^, ImagePath);

  property SongListVM ^ Songs { SongListVM ^ get(); }

      void
      Play();
  internal : AlbumVM(const Model::Album& album, Player::Playlist& playlist, Player::IPlayer& player,
                     Util::BackgroundWorker& worker);
  AlbumVM(const AlbumVM ^ albumVM);
  void SetFrom(const Model::Album& album);  // intended for use with live/progressive updates

  const Model::Album* GetModel() const;

 private:
  Model::Album m_album;
  Player::Playlist& m_playlist;
  Player::IPlayer& m_player;
  Util::BackgroundWorker& m_worker;
  SongListVM ^ m_songs;
};
}
}

#endif