//
// SongListControl.xaml.cpp
// Implementation of the SongListControl class
//

#include "pch.h"
#include "SongListControl.xaml.h"

#include "Events/EventService.hpp"
#include "Events/SongSelectedEvent.hpp"

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
// The User Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234236

SongListControl::SongListControl()
{
	InitializeComponent();
}

void Arcusical::SongListControl::SongClicked(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e)
{
	auto clickedSong = dynamic_cast<ViewModel::SongVM^>(e->ClickedItem);
	if (clickedSong != nullptr)
	{
		Events::SongSelectedEvent selectedEvent(clickedSong);
		Events::EventService<Events::SongSelectedEvent>::BroadcastEvent(selectedEvent);
	}
}