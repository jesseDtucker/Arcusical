#include "pch.h"

#include "AlternateConverter.hpp"
using namespace Arcusical::Converters;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI;

static const float OPACITY = 0.7f;

AlternateConverter::AlternateConverter() {
  m_normalBrush = ref new SolidColorBrush(Colors::WhiteSmoke);
  m_alternateBrush = ref new SolidColorBrush(Colors::LightGray);
  m_normalBrush->Opacity = OPACITY;
  m_alternateBrush->Opacity = OPACITY;
}

Platform::Object ^ AlternateConverter::Convert(Platform::Object ^ value,
                                               Windows::UI::Xaml::Interop::TypeName targetType,
                                               Platform::Object ^ parameter, Platform::String ^ language) {
  auto val = dynamic_cast<Platform::Box<bool> ^>(value);
  ARC_ASSERT(val != nullptr);
  return val ? m_alternateBrush : m_normalBrush;
}

Platform::Object ^ AlternateConverter::ConvertBack(Platform::Object ^ value,
                                                   Windows::UI::Xaml::Interop::TypeName targetType,
                                                   Platform::Object ^ parameter, Platform::String ^ language) {
  ARC_FAIL("Not implemented!");
  return nullptr;
}
