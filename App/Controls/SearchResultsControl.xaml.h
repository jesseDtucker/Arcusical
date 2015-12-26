//
// SearchResultsControl.xaml.h
// Declaration of the SearchResultsControl class
//

#pragma once

#include "Controls\SearchResultsControl.g.h"
#include "ViewModels\SearchResultsVM.hpp"
#include "Utility/XamlMacros.hpp"

namespace Arcusical {
[Windows::Foundation::Metadata::WebHostHidden] public ref class SearchResultsControl sealed {
 public:
  SearchResultsControl();

  void ShowResults();
  void HideResults();

  VM_DECLARATION(ViewModel::SearchResultsVM ^ );

 private:
  void ShowIfNeeded();
  void BackClicked(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);

  Windows::UI::Xaml::Media::Animation::Storyboard ^ m_animIn;
  Windows::UI::Xaml::Media::Animation::Storyboard ^ m_animOut;
  bool m_isShown;

  Util::Subscription m_hasResultsSub;
};
}
