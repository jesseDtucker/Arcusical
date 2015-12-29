//
// BottomBar.xaml.h
// Declaration of the BottomBar class
//

#pragma once

#include "Controls\BottomBar.g.h"

#include "ViewModels/SongPlayerVM.hpp"

namespace Arcusical {
[Windows::Foundation::Metadata::WebHostHidden] public ref class BottomBar sealed {
 public:
  BottomBar();

  VM_DECLARATION(ViewModel::SongPlayerVM ^ );

 private:
  void AlbumSelected(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
  void SearchClicked(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
  void WhatIsPlayingClicked(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
};
}
