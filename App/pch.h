//
// pch.h
// Header for standard system include files.
//

#pragma once

#include <collection.h>
#include <ppltasks.h>

#include "App.xaml.h"

// Helpers
#include "Utility/DispatcherHelper.hpp"
#include "Utility/XamlMacros.hpp"

// Utility
#include "Arc_Assert.hpp"

#include "IPlayer.hpp"
#include "Playlist.hpp"
#include "MusicProvider.hpp"
#include "MusicSearcher.hpp"

// VM definitions. XAML compiler chokes if they are not included in either pch or a xaml
// implementation.
#include "ViewModels/AlbumVM.hpp"
#include "ViewModels/AlbumListControlVM.hpp"
#include "ViewModels/GuideVM.hpp"
#include "ViewModels/SearchVM.hpp"
#include "ViewModels/SearchResultsVM.hpp"
#include "ViewModels/SongListControlVM.hpp"
#include "ViewModels/SongListVM.hpp"
#include "ViewModels/SongVM.hpp"
#include "ViewModels/VolumeSilderVM.hpp"

#include "Converters/IsPlayingToIconPath.hpp"
#include "Converters/SongListSizeToVisiblity.hpp"
#include "Converters/AlternateConverter.hpp"