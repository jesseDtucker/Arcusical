//
// SongPlayer.xaml.h
// Declaration of the SongPlayer class
//

#pragma once

#include <memory>

#include "Controls\SongPlayer.g.h"
#include "ViewModels\SongPlayerVM.hpp"
#include "Utility\XamlMacros.hpp"

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

		VM_DECLARATION(ViewModel::SongPlayerVM^);
	private:
		// control input callbacks
		void PlayPauseButton_Clicked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void Previous_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void Next_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};
}
