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

// VM definitions. XAML compiler chokes if they are not included in either pch or a xaml
// implementation.
#include "ViewModels/AlbumVM.hpp"
#include "ViewModels/AlbumListVM.hpp"
#include "ViewModels/AlbumListControlVM.hpp"
#include "ViewModels/SongListControlVM.hpp"
#include "ViewModels/SongListVM.hpp"
#include "ViewModels/SongVM.hpp"


#include "Converters/IsPlayingToIconPath.hpp"