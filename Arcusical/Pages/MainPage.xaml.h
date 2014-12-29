//
// MainPage.xaml.h
// Declaration of the MainPage class.
//

#pragma once

#include "Pages/MainPage.g.h"

namespace Arcusical
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	public ref class MainPage sealed
	{
	public:
		MainPage();
	private:
		void SetupTransportControls();
		void OnTransportControlButtonPressed(Windows::Media::SystemMediaTransportControls^ sender, Windows::Media::SystemMediaTransportControlsButtonPressedEventArgs^ args);

		Util::Subscription m_isPlayingSub;
		Util::Subscription m_songChangedSub;
		Util::Subscription m_isEndedSub;
	};
}
