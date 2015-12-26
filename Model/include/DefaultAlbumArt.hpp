#pragma once
#ifndef DEFAULT_ALBUM_ART
#define DEFAULT_ALUBM_ART

#include <string>

namespace Arcusical {
namespace Model {
bool isDefaultAlbumArt(const std::wstring& path);
std::wstring GetDefaultArt();
}
}

#endif