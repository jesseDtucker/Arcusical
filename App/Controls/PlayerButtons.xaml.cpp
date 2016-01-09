#include "pch.h"

#include "Arc_Assert.hpp"
#include "Events/EventService.hpp"
#include "Events/SongSelectedEvent.hpp"
#include "IPlayer.hpp"
#include "PlayerButtons.xaml.h"
#include "ViewModels/SongVM.hpp"

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Media::MediaProperties;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

using namespace Arcusical;
using namespace Arcusical::ViewModel;

// The User Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234236

PlayerButtons::PlayerButtons() { InitializeComponent(); }

VM_IMPL(SongPlayerVM ^, PlayerButtons);

void PlayerButtons::PlayPauseButton_Clicked(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e) {
  ARC_ASSERT(VM != nullptr);
  if (VM->IsPlaying) {
    VM->Pause();
  } else {
    VM->Play();
  }
}

void PlayerButtons::Previous_Click(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e) {
  ARC_ASSERT(VM != nullptr);
  VM->Previous();
}

void PlayerButtons::Next_Click(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e) {
  ARC_ASSERT(VM != nullptr);
  VM->Next();
}
