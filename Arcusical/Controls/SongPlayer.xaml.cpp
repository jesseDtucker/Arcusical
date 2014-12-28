//
// SongPlayer.xaml.cpp
// Implementation of the SongPlayer class
//

#include "pch.h"
#include "SongPlayer.xaml.h"

#include <future>

#include "Arc_Assert.hpp"
#include "Events/EventService.hpp"
#include "Events/SongSelectedEvent.hpp"
#include "IPlayer.hpp"
#include "ViewModels/SongVM.hpp"



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

using namespace std;
using namespace Arcusical;
using namespace Arcusical::ViewModel;

// The User Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234236

SongPlayer::SongPlayer()
: m_playerVM(ref new SongPlayerVM())
{
	InitializeComponent();
	this->DataContext = m_playerVM;
}

void SongPlayer::PlayPauseButton_Clicked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	async([this]()
	{
		if (m_playerVM->IsPlaying)
		{
			m_playerVM->Pause();
		}
		else
		{
			m_playerVM->Play();
		}
	});
}