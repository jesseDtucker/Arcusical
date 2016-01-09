#pragma once


namespace Arcusical {
namespace Converters {
public
ref class SongListSizeToVisibility sealed : public Windows::UI::Xaml::Data::IValueConverter {
 public:
  virtual Platform::Object ^ Convert(Platform::Object ^ value, Windows::UI::Xaml::Interop::TypeName targetType,
                                     Platform::Object ^ parameter, Platform::String ^ language);
  virtual Platform::Object ^ ConvertBack(Platform::Object ^ value, Windows::UI::Xaml::Interop::TypeName targetType,
                                         Platform::Object ^ parameter, Platform::String ^ language);
};
}
}
