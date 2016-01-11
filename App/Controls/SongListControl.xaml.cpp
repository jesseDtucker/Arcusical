#include "pch.h"

#include "SongListControl.xaml.h"
#include "ViewModels/SongVM.hpp"

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
using namespace ViewModel;

SongListControl::SongListControl() { InitializeComponent(); }

VM_IMPL(SongListControlVM ^, SongListControl);

void SongListControl::SongDoubleTapped(Platform::Object ^ sender,
                                       Windows::UI::Xaml::Input::DoubleTappedRoutedEventArgs ^ e) {
  ARC_ASSERT(VM != nullptr);

  auto uiElement = dynamic_cast<FrameworkElement ^>(sender);
  ARC_ASSERT(uiElement != nullptr);
  if (uiElement != nullptr) {
    auto song = dynamic_cast<SongVM ^>(uiElement->DataContext);
    ARC_ASSERT(song != nullptr);
    if (song != nullptr) {
      VM->PlaySongsAfterAndIncluding(song);
    }
  }
}
