//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"

#include "IFile.hpp"
#include "IPlayer.hpp"
#include "Controls/AlbumListControl.xaml.h"
#include "Controls/SongListControl.xaml.h"
#include "Controls/Search.xaml.h"
#include "Controls/SongPlayer.xaml.h"
#include "Playlist.hpp"
#include "Storage.hpp"
#include "ViewModels/AlbumListControlVM.hpp"

using namespace concurrency;
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
using namespace Windows::Media;

using namespace std;
using namespace Arcusical;
using namespace Arcusical::Model;
using namespace Arcusical::ViewModel;
using namespace Arcusical::MusicProvider;
using namespace Arcusical::Player;
using namespace FileSystem;

MainPage::MainPage()
	// Models
	: m_playlist(nullptr)
	, m_player(nullptr)
	, m_musicProvider(nullptr)
	, m_searcher(nullptr)
	// View Models
	, m_searchVM(nullptr)
	, m_songListVM(nullptr)
	, m_volumeSlideVM(nullptr)
	, m_playerVM(nullptr)
	, m_albumListVM(nullptr)
{
	InitializeComponent();
}

void MainPage::SetupTransportControls(IPlayer* player)
{
	ARC_ASSERT(player != nullptr);

	try
	{
		auto mediaControl = SystemMediaTransportControls::GetForCurrentView();

		mediaControl->ButtonPressed += ref new TypedEventHandler<SystemMediaTransportControls^, SystemMediaTransportControlsButtonPressedEventArgs^>
			(this, &MainPage::OnTransportControlButtonPressed);

		mediaControl->IsPauseEnabled = true;
		mediaControl->IsPlayEnabled = true;
		mediaControl->IsPreviousEnabled = true;
		mediaControl->IsNextEnabled = true;

		mediaControl->PlaybackStatus = MediaPlaybackStatus::Stopped;

		m_isPlayingSub = player->GetPlaying() += [mediaControl](bool isPlaying)
		{
			if (isPlaying)
			{
				mediaControl->PlaybackStatus = MediaPlaybackStatus::Playing;
			}
			else
			{
				mediaControl->PlaybackStatus = MediaPlaybackStatus::Paused;
			}
		};

		m_songChangedSub = player->GetSongChanged() += [this, mediaControl](const Song& newSong)
		{
			auto displayUpdater = mediaControl->DisplayUpdater;

			displayUpdater->Type = MediaPlaybackType::Music;
			displayUpdater->MusicProperties->Title = ref new Platform::String(newSong.GetTitle().c_str());
			displayUpdater->MusicProperties->Artist = ref new Platform::String(newSong.GetArtist().c_str());

			std::async([this, newSong, displayUpdater]()
			{
				ARC_ASSERT(m_musicProvider != nullptr);
				auto album = m_musicProvider->GetAlbum(newSong.GetAlbumName());
				auto imagePath = album.GetImageFilePath();

				if (imagePath.size() == 0)
				{
					auto defaultAlbumImg = ref new Uri("ms-appx:///Assets/DefaultAlbumBackgrounds/default_cyan.png");
					displayUpdater->Thumbnail = Windows::Storage::Streams::RandomAccessStreamReference::CreateFromUri(defaultAlbumImg);
					displayUpdater->Update();
				}
				else if(oldImagePath != imagePath)
				{
					oldImagePath = imagePath;
					auto file = Storage::LoadFileFromPath(imagePath);
					if (file != nullptr)
					{
						displayUpdater->Thumbnail = Windows::Storage::Streams::RandomAccessStreamReference::CreateFromFile(file->GetRawHandle());
						displayUpdater->Update();
					}
				}
			});
			
			displayUpdater->Update();
		};
	}
	catch (Platform::COMException^ ex)
	{
		ARC_FAIL("Failed to setup transport controls!");
	}
}

void MainPage::OnTransportControlButtonPressed(SystemMediaTransportControls^ sender, SystemMediaTransportControlsButtonPressedEventArgs^ args)
{
	ARC_ASSERT(m_player != nullptr);
	ARC_ASSERT(m_playlist != nullptr);

	switch (args->Button)
	{
	case SystemMediaTransportControlsButton::Play:
		m_player->Play();
		break;
	case SystemMediaTransportControlsButton::Pause:
		m_player->Stop();
		break;
	case SystemMediaTransportControlsButton::Next:
		m_playlist->PlayNext();
		break;
	case SystemMediaTransportControlsButton::Previous:
		m_playlist->PlayPrevious();
		break;
	default:
		ARC_FAIL("Missed a button!");
	}
}

void Arcusical::MainPage::SetDependencies(	MusicSearcher* musicSearcher, 
											MusicProvider::MusicProvider* musicProvider, 
											IPlayer* player, 
											Playlist* playlist)
{
	ARC_ASSERT(musicSearcher != nullptr);
	ARC_ASSERT(musicProvider != nullptr);
	ARC_ASSERT(player != nullptr);
	ARC_ASSERT(playlist != nullptr);

	m_musicProvider = musicProvider;
	m_player = player;
	m_playlist = playlist;
	m_searcher = musicSearcher;

	m_searchVM = ref new SearchVM(*musicSearcher);
	m_songListVM = ref new SongListControlVM(*playlist);
	m_albumListVM = ref new AlbumListControlVM();
	m_playerVM = ref new SongPlayerVM(*player, *playlist);
	m_volumeSlideVM = ref new VolumeSliderVM(*player);

	m_playerVM->VolumeVM = m_volumeSlideVM;

	AlbumsChangedCallback cb = [this](AlbumCollection& albums){ this->OnAlbumsReady(albums); };
	m_albumSub = m_musicProvider->SubscribeAlbums(cb);

	v_albumListControl->VM = m_albumListVM;
	v_songListControl->VM = m_songListVM;
	v_player->VM = m_playerVM;
	v_search->VM = m_searchVM;
}

void Arcusical::MainPage::OnAlbumsReady(Model::AlbumCollection& albums)
{
	auto future = Arcusical::DispatchToUI([this, &albums]()
	{
		m_albumListVM->AlbumList = ref new AlbumListVM(albums, *m_playlist, *m_player);
	});
	future.get();
}


