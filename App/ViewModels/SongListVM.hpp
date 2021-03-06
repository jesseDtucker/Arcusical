#pragma once

#include <collection.h>
#include <vector>

#include "AsyncProcessor.hpp"
#include "MusicTypes.hpp"
#include "PropertyHelper.hpp"
#include "Utility/XamlMacros.hpp"
#include "ViewModels/SongVM.hpp"

namespace Arcusical {
namespace Model {
class Song;
}
namespace Player {
class IPlayer;
class Playlist;
}
namespace ViewModel {
[Windows::UI::Xaml::Data::Bindable] public ref class SongListVM sealed : INotifyPropertyChanged_t {
 public:
  NOTIFY_PROPERTY_CHANGED_IMPL;

  PROP_SET_AND_GET_WINRT(Windows::Foundation::Collections::IVector<ViewModel::SongVM ^> ^, List);

  internal : SongListVM(const std::vector<Model::Song>& songs, Player::Playlist& playlist, Player::IPlayer& player,
                        Util::BackgroundWorker& worker);

 private:
  Util::Subscription m_playerSubscription;
  ViewModel::SongVM ^ m_currentSong;
};
}
}
