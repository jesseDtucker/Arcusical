#pragma once

#include "Controls/SongListControl.g.h"
#include "Utility/XamlMacros.hpp"
#include "ViewModels/SongListControlVM.hpp"

using namespace Windows::UI::Xaml;

namespace Arcusical {
[Windows::Foundation::Metadata::WebHostHidden] public ref class SongListControl sealed {
 public:
  SongListControl();

  VM_DECLARATION(ViewModel::SongListControlVM ^);

 private:
  void SongDoubleTapped(Platform::Object ^ sender, Windows::UI::Xaml::Input::DoubleTappedRoutedEventArgs ^ e);
};
}
