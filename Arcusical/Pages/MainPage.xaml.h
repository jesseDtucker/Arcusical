//
// MainPage.xaml.h
// Declaration of the MainPage class.
//

#pragma once

#include "Pages/MainPage.g.h"

#include "ViewModels/AlbumListControlVM.hpp"
#include "ViewModels/SearchVM.hpp"
#include "ViewModels/SearchResultsVM.hpp"
#include "ViewModels/SongListControlVM.hpp"
#include "ViewModels/VolumeSilderVM.hpp"
#include "ViewModels/SongPlayerVM.hpp"
#include "Subscription.hpp"

namespace Arcusical
{
namespace MusicProvider { class MusicSearcher; class MusicProvider; }
namespace Player { class IPlayer; class Playlist; }
}

namespace Arcusical
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	public ref class MainPage sealed
	{
	public:
		MainPage();
	internal:
		void SetDependencies(	MusicProvider::MusicSearcher* musicSearcher,
								MusicProvider::MusicProvider* musicProvider,
								Player::IPlayer* player,
								Player::Playlist* playlist);
	private:
		void SetupTransportControls(Player::IPlayer* player);
		void OnTransportControlButtonPressed(Windows::Media::SystemMediaTransportControls^ sender, Windows::Media::SystemMediaTransportControlsButtonPressedEventArgs^ args);

		void OnAlbumsReady(Model::AlbumCollection& albums);

		std::wstring oldImagePath;

		Util::Subscription m_isPlayingSub;
		Util::Subscription m_songChangedSub;
		Util::Subscription m_isEndedSub;

		MusicProvider::MusicSearcher* m_searcher;
		MusicProvider::MusicProvider* m_musicProvider;
		Player::Playlist* m_playlist;
		Player::IPlayer* m_player;

		ViewModel::AlbumListControlVM^ m_albumListVM;
		ViewModel::SongListControlVM^ m_songListVM;
		ViewModel::SearchVM^ m_searchVM;
		ViewModel::VolumeSliderVM^ m_volumeSlideVM;
		ViewModel::SongPlayerVM^ m_playerVM;
		ViewModel::SearchResultsVM^ m_searchResultsVM;

		Util::Subscription m_albumSub;
	};
}
