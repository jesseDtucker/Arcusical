#include "pch.h"

#include "Converters/IsPlayingConverters.hpp"

using Windows::UI::Text::FontWeights;

namespace Arcusical {
namespace Converters {
Platform::String ^ IsPlayingToIconPath::PAUSE_ICON = "/Assets/Icons/Pause.png";
Platform::String ^ IsPlayingToIconPath::PLAY_ICON = "/Assets/Icons/Play.png";

Platform::Object ^ IsPlayingToIconPath::Convert(Platform::Object ^ value,
                                                Windows::UI::Xaml::Interop::TypeName targetType,
                                                Platform::Object ^ parameter, Platform::String ^ language) {
  auto isPlaying = dynamic_cast<Platform::Box<bool> ^>(value);
  if (isPlaying != nullptr) {
    return isPlaying ? PAUSE_ICON : PLAY_ICON;
  } else {
    return PLAY_ICON;
  }
}

Platform::Object ^ IsPlayingToIconPath::ConvertBack(Platform::Object ^ value,
                                                    Windows::UI::Xaml::Interop::TypeName targetType,
                                                    Platform::Object ^ parameter, Platform::String ^ language) {
  ARC_FAIL("Not implemented!");
  return nullptr;
}

Platform::Object ^ IsPlayingToFontWeight::Convert(Platform::Object ^ value,
                                                  Windows::UI::Xaml::Interop::TypeName targetType,
                                                  Platform::Object ^ parameter, Platform::String ^ language) {
  auto isPlaying = dynamic_cast<Platform::Box<bool> ^>(value);
  if (isPlaying) {
    return FontWeights::Bold;
  } else {
    return FontWeights::Normal;
  }
}

Platform::Object ^ IsPlayingToFontWeight::ConvertBack(Platform::Object ^ value,
                                                      Windows::UI::Xaml::Interop::TypeName targetType,
                                                      Platform::Object ^ parameter, Platform::String ^ language) {
  ARC_FAIL("Not implemented!");
  return nullptr;
}
}
}