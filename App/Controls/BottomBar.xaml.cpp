//
// TopBar.xaml.cpp
// Implementation of the TopBar class
//

#include "pch.h"
#include "BottomBar.xaml.h"

#include <cmath>

#include "Events/EventService.hpp"
#include "Events/AlbumSelectedEvent.hpp"
#include "Events/SearchSelectedEvent.hpp"
#include "Events/WhatIsPlayingSelectedEvent.hpp"

using namespace Arcusical;
using namespace Arcusical::Events;
using namespace Arcusical::ViewModel;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

VM_IMPL(SongPlayerVM ^, BottomBar);

const double MIN_JUMP = 1.0;

BottomBar::BottomBar() {
  InitializeComponent();

  v_slider->ValueChanged += ref new Primitives::RangeBaseValueChangedEventHandler([this](
      Object ^ sender, Primitives::RangeBaseValueChangedEventArgs ^ e) {
    if (abs(e->NewValue - e->OldValue) > MIN_JUMP) {
      VM->ChangeTimeTo(e->NewValue);
    }
  });
}

void Arcusical::BottomBar::AlbumSelected(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e) {
  if (VM != nullptr && VM->Album != nullptr) {
    EventService<AlbumSelectedEvent>::BroadcastEvent({VM->Album});
  }
}

void Arcusical::BottomBar::SearchClicked(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e) {
  EventService<SearchSelectedEvent>::BroadcastEvent({});
}

void Arcusical::BottomBar::WhatIsPlayingClicked(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e) {
  EventService<WhatIsPlayingSelectedEvent>::BroadcastEvent({});
}