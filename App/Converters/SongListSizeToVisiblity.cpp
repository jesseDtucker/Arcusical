#include "pch.h"

#include "Converters/SongListSizeToVisiblity.hpp"

#include "Arc_Assert.hpp"
#include "ViewModels/SongListVM.hpp"

using namespace Windows::UI::Xaml;

using namespace Arcusical::ViewModel;
using namespace Arcusical::Converters;

Platform::Object ^ SongListSizeToVisibility::Convert(Platform::Object ^ value,
                                                     Windows::UI::Xaml::Interop::TypeName targetType,
                                                     Platform::Object ^ parameter, Platform::String ^ language) {
  if (value != nullptr) {
    auto songList = dynamic_cast<SongListVM ^>(value);
    ARC_ASSERT(songList != nullptr);
    if (songList != nullptr) {
      return (songList->List->Size > 1) ? (Visibility::Visible) : (Visibility::Collapsed);
    }
  }

  return Visibility::Collapsed;
}

Platform::Object ^ SongListSizeToVisibility::ConvertBack(Platform::Object ^ value,
                                                         Windows::UI::Xaml::Interop::TypeName targetType,
                                                         Platform::Object ^ parameter, Platform::String ^ language) {
  ARC_FAIL("Not implemented!");
  return nullptr;
}