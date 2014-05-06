//
// SongPlayer.xaml.cpp
// Implementation of the SongPlayer class
//

#include "pch.h"
#include "SongPlayer.xaml.h"

#include <future>

#include "Arc_Assert.hpp"
#include "Commands/PlaySongCommand.hpp"
#include "Events/EventService.hpp"
#include "Events/SongSelectedEvent.hpp"
#include "IPlayer.hpp"
#include "ViewModels/SongVM.hpp"

using namespace Arcusical;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Media::MediaProperties;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

// The User Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234236

SongPlayer::SongPlayer()
: m_currentSong(nullptr)
{
	InitializeComponent();
	this->DataContext = ViewModel::SongVM::GetEmptySong();

	std::function<void(const Events::SongSelectedEvent&)> songSelectedCallback = [this]
		(const Events::SongSelectedEvent& selectedSongEvent)
	{ 
		this->OnSongSelected(selectedSongEvent); 
	};
	m_songSelectedSubscription = Events::EventService<Events::SongSelectedEvent>::RegisterListener(songSelectedCallback);
}

void SongPlayer::OnSongSelected(const Events::SongSelectedEvent& selectedSongEvent)
{
	ARC_ASSERT(selectedSongEvent.GetSelectedSong() != nullptr);
	m_currentSong = selectedSongEvent.GetSelectedSong();
	this->DataContext = m_currentSong;
}