#pragma once
#ifndef IS_PLAYING_TO_ICON_PATH_HPP
#define IS_PLAYING_TO_ICON_PATH_HPP

namespace Arcusical {
namespace Converters {
public
ref class IsPlayingToIconPath sealed : public Windows::UI::Xaml::Data::IValueConverter {
 public:
  virtual Platform::Object ^ Convert(Platform::Object ^ value, Windows::UI::Xaml::Interop::TypeName targetType,
                                     Platform::Object ^ parameter, Platform::String ^ language);
  virtual Platform::Object ^ ConvertBack(Platform::Object ^ value, Windows::UI::Xaml::Interop::TypeName targetType,
                                         Platform::Object ^ parameter, Platform::String ^ language);

 private:
  static Platform::String ^ PLAY_ICON;
  static Platform::String ^ PAUSE_ICON;
};
}
}

#endif