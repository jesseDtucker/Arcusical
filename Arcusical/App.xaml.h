//
// App.xaml.h
// Declaration of the App class.
//

#pragma once

#include <memory>

#include "App.g.h"
#include "Subscription.hpp"

#include "LocalMusicCache.hpp"
#include "MusicSearcher.hpp"

namespace Arcusical
{
	/// <summary>
	/// Provides application-specific behavior to supplement the default Application class.
	/// </summary>
	ref class App sealed
	{
	protected:
		virtual void OnLaunched(Windows::ApplicationModel::Activation::LaunchActivatedEventArgs^ e) override;

	internal:
		App();

		void SetupApplication();

	private:
		void OnSuspending(Platform::Object^ sender, Windows::ApplicationModel::SuspendingEventArgs^ e);
		void OnNavigationFailed(Platform::Object ^sender, Windows::UI::Xaml::Navigation::NavigationFailedEventArgs ^e);

		LocalMusicStore::LocalMusicCache m_cache;
		MusicProvider::MusicSearcher m_searcher;
		
	};
}
