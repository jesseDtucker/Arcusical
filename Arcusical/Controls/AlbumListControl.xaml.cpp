//
// AlbumListControl.xaml.cpp
// Implementation of the AlbumListControl class
//

#include "pch.h"

#include <algorithm>

#include "AlbumListControl.xaml.h"
#include "Events/AlbumSelectedEvent.hpp"
#include "Events/EventService.hpp"

using namespace Arcusical;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

using namespace ViewModel;

AlbumListControl::AlbumListControl()
{
	InitializeComponent();
}

void Arcusical::AlbumListControl::AlbumClicked(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e)
{
	auto clickedAlbum = dynamic_cast<ViewModel::AlbumVM^>(e->ClickedItem);
	if (clickedAlbum != nullptr)
	{
		Events::AlbumSelectedEvent selectedEvent(clickedAlbum);
		Events::EventService<Events::AlbumSelectedEvent>::BroadcastEvent(selectedEvent);
	}
}
