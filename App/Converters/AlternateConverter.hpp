#pragma once

namespace Arcusical {
namespace Converters {
public
ref class AlternateConverter sealed : public Windows::UI::Xaml::Data::IValueConverter {
 public:
  AlternateConverter();

  virtual Platform::Object ^ Convert(Platform::Object ^ value, Windows::UI::Xaml::Interop::TypeName targetType,
                                     Platform::Object ^ parameter, Platform::String ^ language);
  virtual Platform::Object ^ ConvertBack(Platform::Object ^ value, Windows::UI::Xaml::Interop::TypeName targetType,
                                         Platform::Object ^ parameter, Platform::String ^ language);

 private:
  Windows::UI::Xaml::Media::SolidColorBrush ^ m_normalBrush;
  Windows::UI::Xaml::Media::SolidColorBrush ^ m_alternateBrush;
};
}
}
