//
// Search.xaml.cpp
// Implementation of the Search class
//

#include "pch.h"
#include "Search.xaml.h"

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

using namespace Arcusical;
using namespace Arcusical::ViewModel;
using namespace Arcusical::MusicProvider;


Search::Search() {
    InitializeComponent();
}

VM_IMPL(SearchVM^, Search);

void Search::NotifyTextEntered(Platform::String^ text) {
    if (!IsReceivingText()) {
        v_searchBox->Text = "";
        v_searchBox->Focus(Windows::UI::Xaml::FocusState::Keyboard);
    }
}

bool Search::IsReceivingText() {
    return !(v_searchBox->FocusState == Windows::UI::Xaml::FocusState::Unfocused);
}

void Search::TextBox_KeyDown(Platform::Object^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs^ e) {
    if (e->Key == Windows::System::VirtualKey::Enter) {
        VM->SelectCurrent();
    }
}
