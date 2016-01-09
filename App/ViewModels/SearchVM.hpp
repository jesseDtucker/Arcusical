#pragma once
#ifndef SEARCH_HPP
#define SEARCH_HPP

#include "CancellationToken.hpp"
#include "PropertyHelper.hpp"
#include "Subscription.hpp"
#include "Utility/XamlMacros.hpp"
#include "ViewModels/AlbumListControlVM.hpp"
#include "ViewModels/SearchVM.hpp"
#include "ViewModels/SongListControlVM.hpp"

namespace Arcusical {
namespace MusicProvider {
class MusicSearcher;
}
namespace Player {
class IPlayer;
class Playlist;
}

namespace ViewModel {
[Windows::UI::Xaml::Data::Bindable] public ref class SearchVM sealed : INotifyPropertyChanged_t {
 public:
  NOTIFY_PROPERTY_CHANGED_IMPL;

  PROP_SET_AND_GET_WINRT(Platform::String ^, SearchTerm);
  PROP_SET_AND_GET_WINRT(ViewModel::AlbumListControlVM ^, AlbumListControlVM);
  PROP_SET_AND_GET_WINRT(ViewModel::SongListControlVM ^, SongListControlVM);

  void SelectCurrent();

  internal : SearchVM(MusicProvider::MusicSearcher& searcher, Player::Playlist& playlist,
                      Util::BackgroundWorker& worker, Player::IPlayer& player);

 private:
  void StartSearch(Platform::String ^ searchTerm);

  MusicProvider::MusicSearcher& m_searcher;
  Util::CancellationTokenRef m_searchCancelToken = nullptr;
  Util::Subscription m_onSearchTermChangedSub;
  Player::Playlist& m_playlist;
  Player::IPlayer& m_player;
  Util::BackgroundWorker& m_worker;
};
}
}

#endif