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
#include "MusicProvider.hpp"
#include "MusicSearcher.hpp"
#include "Playlist.hpp"

// VM definitions. XAML compiler chokes if they are not included in either pch or a xaml
// implementation.
#include "ViewModels/AlbumListControlVM.hpp"
#include "ViewModels/AlbumVM.hpp"
#include "ViewModels/GuideVM.hpp"
#include "ViewModels/SearchVM.hpp"
#include "ViewModels/SongListControlVM.hpp"
#include "ViewModels/SongListVM.hpp"
#include "ViewModels/SongVM.hpp"
#include "ViewModels/VolumeSilderVM.hpp"
#include "ViewModels/WhatIsPlayingVM.hpp"

#include "Converters/AlternateConverter.hpp"
#include "Converters/IsPlayingToIconPath.hpp"
#include "Converters/SongListSizeToVisiblity.hpp"