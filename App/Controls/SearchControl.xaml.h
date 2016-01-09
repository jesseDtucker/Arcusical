#pragma once

#include "Controls\SearchControl.g.h"
#include "Utility\XamlMacros.hpp"
#include "ViewModels\SearchVM.hpp"

namespace Arcusical {
  [Windows::Foundation::Metadata::WebHostHidden] public ref class SearchControl sealed {
   public:
    SearchControl();

    void SetActive();
    bool IsActive();
    void ShowResults();
    void HideResults();

    VM_DECLARATION(ViewModel::SearchVM ^);

   private:
    void BackClicked(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
    void TextBoxKeyDown(Platform::Object ^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs ^ e);

    Windows::UI::Xaml::Media::Animation::Storyboard ^ m_animIn;
    Windows::UI::Xaml::Media::Animation::Storyboard ^ m_animOut;
    bool m_isShown;

    Util::Subscription m_searchSelectedSub;
  };
}
