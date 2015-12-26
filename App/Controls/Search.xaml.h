//
// Search.xaml.h
// Declaration of the Search class
//

#pragma once

#include "Controls\Search.g.h"
#include "ViewModels\SearchVM.hpp"
#include "Utility\XamlMacros.hpp"

namespace Arcusical {
namespace MusicProvider {
class MusicSearcher;
}
}

namespace Arcusical {
[Windows::Foundation::Metadata::WebHostHidden] public ref class Search sealed {
 public:
  Search();
  VM_DECLARATION(ViewModel::SearchVM ^ );
  void NotifyTextEntered(Platform::String ^ text);
  bool IsReceivingText();

 private:
  void TextBox_KeyDown(Platform::Object ^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs ^ e);
};
}
