//
// SongPlayer.xaml.h
// Declaration of the SongPlayer class
//

#pragma once

#include <memory>

#include "Controls\SongPlayer.g.h"
#include "ViewModels\SongVM.hpp"

namespace Arcusical
{
	namespace Player
	{
		class IPlayer;
	}

	namespace Events
	{
		class SongSelectedEvent;
	}

	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class SongPlayer sealed
	{
	public:
		SongPlayer();

	private:
		// control input callbacks
		void PlayPauseButton_Clicked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnSongSelected(const Events::SongSelectedEvent& selectedSong);

		ViewModel::SongVM^ m_currentSong;
		std::shared_ptr<Util::Subscription> m_songSelectedSubscription;
	};
}
