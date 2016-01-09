#pragma once

#include "Controls\WhatIsPlaying.g.h"
#include "ViewModels/WhatIsPlayingVM.hpp"

namespace Arcusical {
[Windows::Foundation::Metadata::WebHostHidden] public ref class WhatIsPlaying sealed {
 public:
  WhatIsPlaying();
  VM_DECLARATION(ViewModel::WhatIsPlayingVM ^);

  void Show();
  void Hide();

 private:
  void HeaderButtonClicked(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);

  bool m_isShown;
  Windows::UI::Xaml::Media::Animation::Storyboard ^ m_animIn;
  Windows::UI::Xaml::Media::Animation::Storyboard ^ m_animOut;
  Util::Subscription m_whatIsPlayingSelectedEventSub;
};
}
