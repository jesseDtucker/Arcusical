#include "pch.h"

#include "Events/EventService.hpp"
#include "Events/WhatIsPlayingSelectedEvent.hpp"
#include "WhatIsPlaying.xaml.h"

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
using namespace Windows::UI::Xaml::Media::Animation;
using namespace Windows::UI::Xaml::Navigation;

VM_IMPL(WhatIsPlayingVM ^, WhatIsPlaying);

WhatIsPlaying::WhatIsPlaying() : m_isShown(false) {
  InitializeComponent();

  m_animIn = safe_cast<Storyboard ^>(v_root->Resources->Lookup("slideInAnim"));
  m_animOut = safe_cast<Storyboard ^>(v_root->Resources->Lookup("slideOutAnim"));

  m_whatIsPlayingSelectedEventSub = EventService<WhatIsPlayingSelectedEvent>::RegisterListener({[this](auto unused) {
    if (m_isShown) {
      this->Hide();
    } else {
      this->Show();
    }
  }});
}

void WhatIsPlaying::Show() {
  if (!m_isShown) {
    m_isShown = true;
    m_animOut->Stop();
    m_animIn->Begin();
  }
}

void WhatIsPlaying::Hide() {
  if (m_isShown) {
    m_isShown = false;
    m_animIn->Stop();
    m_animOut->Begin();
  }
}

void Arcusical::WhatIsPlaying::HeaderButtonClicked(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e) {
  Hide();
}
