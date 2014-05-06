#include "pch.h"

#include "Converters/IsPlayingToIconPath.hpp"

namespace Arcusical
{
namespace Converters
{
	Platform::String^ IsPlayingToIconPath::PAUSE_ICON = "/Assets/Icons/Pause.png";
	Platform::String^ IsPlayingToIconPath::PLAY_ICON = "/Assets/Icons/Play.png";

	Platform::Object^ IsPlayingToIconPath::Convert(Platform::Object^ value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object^ parameter, Platform::String^ language)
	{
		ViewModel::SongVM^ song = (ViewModel::SongVM^)(value);
		if (song != nullptr)
		{
			return song->IsPlaying ? PAUSE_ICON : PLAY_ICON;
		}
		else
		{
			return PLAY_ICON;
		}
	}

	Platform::Object^ IsPlayingToIconPath::ConvertBack(Platform::Object^ value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object^ parameter, Platform::String^ language)
	{
		ARC_FAIL("Not implemented!");
		return nullptr;
	}
}
}