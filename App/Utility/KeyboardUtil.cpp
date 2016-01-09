#include "pch.h"

#include "KeyboardUtil.hpp"
bool IsInRange(int value, int lower, int upper) { return value >= lower && value <= upper; }

bool Arcusical::IsAlphaNumeric(Windows::System::VirtualKey key) {
  int val = (int)(key);
  // Source of values: https://msdn.microsoft.com/en-us/library/windows/apps/windows.system.virtualkey.aspx
  return IsInRange(val, 48, 57) ||  // Numbers
         IsInRange(val, 65, 90) ||  // Letters
         IsInRange(val, 96, 105);   // Numpad
}