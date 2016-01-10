#pragma once

#include "Controls\BottomBar.g.h"
#include "ViewModels/SongPlayerVM.hpp"

namespace Arcusical {
[Windows::Foundation::Metadata::WebHostHidden] public ref class BottomBar sealed {
 public:
  BottomBar();

  VM_DECLARATION(ViewModel::SongPlayerVM ^);

 private:
  void AlbumSelected(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
  void SearchClicked(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
  void WhatIsPlayingClicked(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);

  void OnSliderPointerDown(Platform::Object ^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs ^ e);
  void OnSliderPointerReleased(Platform::Object ^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs ^ e);

  bool m_isScrubbing;
  Util::Subscription m_amountPlayedSub;
};
}
