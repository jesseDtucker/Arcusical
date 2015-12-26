//
// Guide.xaml.h
// Declaration of the Guide class
//

#pragma once

#include "Controls\Guide.g.h"
#include "Utility\XamlMacros.hpp"
#include "ViewModels\GuideVM.hpp"

namespace Arcusical {
[Windows::Foundation::Metadata::WebHostHidden] public ref class Guide sealed {
 public:
  Guide();
  VM_DECLARATION(ViewModel::GuideVM ^ );

 private:
  void Play_Clicked(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
  void Shuffle_Clicked(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
};
}
