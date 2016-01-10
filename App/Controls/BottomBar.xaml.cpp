#include "pch.h"

#include <cmath>

#include "BottomBar.xaml.h"
#include "Events/AlbumSelectedEvent.hpp"
#include "Events/EventService.hpp"
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

const double MIN_JUMP = 1.0;

BottomBar::BottomBar() : m_isScrubbing(false) {
  InitializeComponent();

  v_slider->AddHandler(UIElement::PointerPressedEvent,
                       ref new PointerEventHandler(this, &BottomBar::OnSliderPointerDown), true);
  v_slider->AddHandler(UIElement::PointerReleasedEvent,
                       ref new PointerEventHandler(this, &BottomBar::OnSliderPointerReleased), true);
}

SongPlayerVM ^ BottomBar::VM::get() {
  if (m_viewModel != this->DataContext) {
    m_viewModel = dynamic_cast<SongPlayerVM ^>(this->DataContext);
  }
  return m_viewModel;
}

void BottomBar::VM::set(SongPlayerVM ^ vm) {
  ARC_ASSERT(Arcusical::HasThreadAccess());
  m_viewModel = vm;
  this->DataContext = m_viewModel;
  if (m_viewModel != nullptr) {
    m_amountPlayedSub = m_viewModel->OnAmountPlayedChanged += [this](double newAmount) {
      if (!m_isScrubbing) {
        v_slider->Value = newAmount;
      }
    };
  }
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

void Arcusical::BottomBar::OnSliderPointerDown(Platform::Object ^ sender,
                                               Windows::UI::Xaml::Input::PointerRoutedEventArgs ^ e) {
  if (e->GetCurrentPoint(this)->Properties->IsLeftButtonPressed) {
    m_isScrubbing = true;
  }
}

void Arcusical::BottomBar::OnSliderPointerReleased(Platform::Object ^ sender,
                                                   Windows::UI::Xaml::Input::PointerRoutedEventArgs ^ e) {
  if (m_isScrubbing) {
    m_isScrubbing = false;
    VM->ChangeTimeTo(v_slider->Value);
  }
}
