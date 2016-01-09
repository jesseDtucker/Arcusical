//
// SearchResultsControl.xaml.cpp
// Implementation of the SearchResultsControl class
//

#include "pch.h"

#include "Events/EventService.hpp"
#include "Events/SearchSelectedEvent.hpp"
#include "SearchControl.xaml.h"
#include "Utility/KeyboardUtil.hpp"

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

using namespace Arcusical;
using namespace Arcusical::Events;
using namespace Arcusical::ViewModel;

// The User Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234236

VM_IMPL(SearchVM ^, SearchControl);

SearchControl::SearchControl() {
  InitializeComponent();
  m_animIn = safe_cast<Storyboard ^>(v_root->Resources->Lookup("slideInAnim"));
  m_animOut = safe_cast<Storyboard ^>(v_root->Resources->Lookup("slideOutAnim"));
  m_searchSelectedSub =
      EventService<SearchSelectedEvent>::RegisterListener({[this](const auto unused) { ShowResults(); }});
}

void SearchControl::SetActive() {
  if (!IsActive()) {
    if (!m_isShown) {
      v_searchBox->Text = "";
    }
    v_searchBox->Focus(Windows::UI::Xaml::FocusState::Keyboard);
    ShowResults();
  }
}

bool SearchControl::IsActive() { return !(v_searchBox->FocusState == Windows::UI::Xaml::FocusState::Unfocused); }

void SearchControl::TextBoxKeyDown(Platform::Object ^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs ^ e) {
  if (e->Key == Windows::System::VirtualKey::Enter) {
    VM->SelectCurrent();
    HideResults();
  } else if (IsAlphaNumeric(e->Key)) {
    ShowResults();
  }
}

void SearchControl::ShowResults() {
  if (!m_isShown) {
    m_isShown = true;
    m_animOut->Stop();
    m_animIn->Begin();
  }
}
void Arcusical::SearchControl::HideResults() {
  if (m_isShown) {
    VM->SearchTerm = "";
    m_isShown = false;
    m_animIn->Stop();
    m_animOut->Begin();
  }
}

void SearchControl::BackClicked(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e) { HideResults(); }
