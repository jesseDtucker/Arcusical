#include "TextUtility.hpp"

using namespace std;

wstring Arcusical::SecondsToString(int time) {
  int seconds = time % 60;
  int minutes = time / 60;
  wstring secondsString = to_wstring(seconds);
  if (seconds < 10) {
    secondsString = L"0" + secondsString;
  }

  return to_wstring(minutes) + L":" + secondsString;
}
