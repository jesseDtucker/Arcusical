#include "pch.h"

#include "PlayProgressTooltipConverter.hpp"

#include "Utility/TextUtility.hpp"

using namespace std;
using namespace Arcusical::Converters;
using namespace Platform;
using namespace Windows::UI::Xaml::Interop;

Object ^
    PlayProgressTooltipConverter::Convert(Object ^ value, TypeName targetType, Object ^ parameter, String ^ language) {
  auto progressDouble = dynamic_cast<Platform::Box<double> ^>(value);
  int progress = (int)(progressDouble);
  return ref new String(SecondsToString(progress).c_str());
}

Object ^ PlayProgressTooltipConverter::ConvertBack(Object ^ value, TypeName targetType, Object ^ parameter,
                                                   String ^ language) {
  ARC_FAIL("Not implemented");
  return nullptr;
}
