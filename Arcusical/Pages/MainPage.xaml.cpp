//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"

#include "IFile.hpp"
#include "IPlayer.hpp"
#include "Controls/SongListControl.xaml.h"
#include "Controls/AlbumListControl.xaml.h"
#include "Playlist.hpp"
#include "Storage.hpp"
#include "ViewModels/AlbumListControlVM.hpp"

#include "IMusicProvider.hpp"

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
using namespace Arcusical::Player;
using namespace FileSystem;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

MainPage::MainPage()
{
	using namespace ViewModel;

	InitializeComponent();

	SetupTransportControls();
}

void MainPage::SetupTransportControls()
{
	auto player = PlayerLocator::ResolveService().lock();
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

		m_songChangedSub = player->GetSongChanged() += [mediaControl](const Song& newSong)
		{
			auto displayUpdater = mediaControl->DisplayUpdater;
			displayUpdater->ClearAll();

			displayUpdater->Type = MediaPlaybackType::Music;
			displayUpdater->MusicProperties->Title = ref new Platform::String(newSong.GetTitle().c_str());
			displayUpdater->MusicProperties->Artist = ref new Platform::String(newSong.GetArtist().c_str());

			std::async([newSong, displayUpdater]()
			{
				auto musicProvider = MusicProvider::MusicProviderLocator::ResolveService().lock();
				ARC_ASSERT(musicProvider != nullptr);
				auto album = musicProvider->GetAlbum(newSong.GetAlbumName());
				auto imagePath = album.GetImageFilePath();

				if (imagePath.size() > 0)
				{
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
	auto player = PlayerLocator::ResolveService().lock();
	auto playlist = PlaylistLocator::ResolveService().lock();
	ARC_ASSERT(player != nullptr);
	ARC_ASSERT(playlist != nullptr);

	switch (args->Button)
	{
	case SystemMediaTransportControlsButton::Play:
		player->Play();
		break;
	case SystemMediaTransportControlsButton::Pause:
		player->Stop();
		break;
	case SystemMediaTransportControlsButton::Next:
		playlist->PlayNext();
		break;
	case SystemMediaTransportControlsButton::Previous:
		playlist->PlayPrevious();
		break;
	default:
		ARC_FAIL("Missed a button!");
	}
}