#pragma once

#include "AsyncProcessor.hpp"
#include "PropertyHelper.hpp"
#include "Utility/XamlMacros.hpp"
#include "ViewModels/AlbumVM.hpp"


namespace Arcusical {
namespace ViewModel {

typedef Windows::Foundation::Collections::IObservableVector<AlbumVM ^> AlbumList;

// TODO::JT. This code is confusing. Is this a factory? An object? both? Clarify the design!
[Windows::UI::Xaml::Data::Bindable] public ref class AlbumListControlVM sealed : INotifyPropertyChanged_t {
 public:
  NOTIFY_PROPERTY_CHANGED_IMPL;

  PROP_SET_AND_GET_WINRT(AlbumList ^, Albums);
  internal :

      AlbumListControlVM();

  static AlbumList ^ CreateAlbumList(const Model::AlbumPtrCollection& albums, Player::Playlist& playlist,
                                     Player::IPlayer& player, Util::BackgroundWorker& worker);

  static AlbumList ^ CreateAlbumList(const std::vector<Model::Album>& albums, Player::Playlist& playlist,
                                     Player::IPlayer& player, Util::BackgroundWorker& worker);

  static AlbumList ^ CreateAlbumList(const Model::AlbumCollection& albums, Player::Playlist& playlist,
                                     Player::IPlayer& player, Util::BackgroundWorker& worker);

 private:
};
}
}
