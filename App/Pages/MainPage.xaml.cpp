//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"

#include "CheckedCasts.hpp"
#include "Controls/AlbumListControl.xaml.h"
#include "Controls/BottomBar.xaml.h"
#include "Controls/Guide.xaml.h"
#include "Controls/PlayerButtons.xaml.h"
#include "Controls/SearchControl.xaml.h"
#include "Controls/SongListControl.xaml.h"
#include "Controls/WhatIsPlaying.xaml.h"
#include "Events/AlbumSelectedEvent.hpp"
#include "Events/EventService.hpp"
#include "IFile.hpp"
#include "IPlayer.hpp"
#include "Playlist.hpp"
#include "Storage.hpp"
#include "Utility/KeyboardUtil.hpp"
#include "ViewModels/AlbumListControlVM.hpp"

using namespace concurrency;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Core;
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
using namespace Util;

MainPage::MainPage()
    // Models
    : m_playlist(nullptr),
      m_player(nullptr),
      m_musicProvider(nullptr),
      m_searcher(nullptr)
      // View Models
      ,
      m_searchVM(nullptr),
      m_volumeSlideVM(nullptr),
      m_playerVM(nullptr),
      m_albumListVM(nullptr) {
  InitializeComponent();
}

void MainPage::SetupTransportControls(IPlayer* player) {
  ARC_ASSERT(player != nullptr);

  try {
    auto mediaControl = SystemMediaTransportControls::GetForCurrentView();

    mediaControl->ButtonPressed +=
        ref new TypedEventHandler<SystemMediaTransportControls ^, SystemMediaTransportControlsButtonPressedEventArgs ^>(
            this, &MainPage::OnTransportControlButtonPressed);

    mediaControl->IsPauseEnabled = true;
    mediaControl->IsPlayEnabled = true;
    mediaControl->IsPreviousEnabled = true;
    mediaControl->IsNextEnabled = true;

    mediaControl->PlaybackStatus = MediaPlaybackStatus::Stopped;

    m_isPlayingSub = player->GetPlaying() += [mediaControl](bool isPlaying) {
      if (isPlaying) {
        mediaControl->PlaybackStatus = MediaPlaybackStatus::Playing;
      } else {
        mediaControl->PlaybackStatus = MediaPlaybackStatus::Paused;
      }
    };

    m_songChangedSub = player->GetSongChanged() += [this, mediaControl](const boost::optional<Song>& newSong) {
      auto displayUpdater = mediaControl->DisplayUpdater;

      displayUpdater->Type = MediaPlaybackType::Music;
      if (newSong) {
        displayUpdater->MusicProperties->Title = ref new Platform::String(newSong->GetTitle().c_str());
        displayUpdater->MusicProperties->Artist = ref new Platform::String(newSong->GetArtist().c_str());
      } else {
        displayUpdater->MusicProperties->Title = "";
        displayUpdater->MusicProperties->Artist = "";
      }
      // TODO::JT move to task runner
      std::async([this, newSong, displayUpdater]() {
        ARC_ASSERT(m_musicProvider != nullptr);

        wstring imagePath;
        if (newSong) {
          auto album = m_musicProvider->GetAlbum(newSong->GetAlbumName());
          if (album) {
            imagePath = album->GetImageFilePath();
          }
        }

        if (imagePath.size() == 0) {
          auto defaultAlbumImg = ref new Uri("ms-appx:///Assets/DefaultAlbumBackgrounds/default_cyan.png");
          displayUpdater->Thumbnail =
              Windows::Storage::Streams::RandomAccessStreamReference::CreateFromUri(defaultAlbumImg);
          displayUpdater->Update();
        } else if (oldImagePath != imagePath) {
          oldImagePath = imagePath;
          auto file = Storage::LoadFileFromPath(imagePath);
          if (file != nullptr) {
            displayUpdater->Thumbnail =
                Windows::Storage::Streams::RandomAccessStreamReference::CreateFromFile(file->GetRawHandle());
            displayUpdater->Update();
          }
        }
      });
      displayUpdater->Update();
    };
  } catch (Platform::COMException ^ ex) {
    ARC_FAIL("Failed to setup transport controls!");
  }
}

void MainPage::OnTransportControlButtonPressed(SystemMediaTransportControls ^ sender,
                                               SystemMediaTransportControlsButtonPressedEventArgs ^ args) {
  ARC_ASSERT(m_player != nullptr);
  ARC_ASSERT(m_playlist != nullptr);

  switch (args->Button) {
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

void Arcusical::MainPage::SetDependencies(MusicSearcher* musicSearcher, MusicProvider::MusicProvider* musicProvider,
                                          IPlayer* player, Playlist* playlist, BackgroundWorker* worker) {
  ARC_ASSERT(musicSearcher != nullptr);
  ARC_ASSERT(musicProvider != nullptr);
  ARC_ASSERT(player != nullptr);
  ARC_ASSERT(playlist != nullptr);
  ARC_ASSERT(worker != nullptr);

  m_musicProvider = musicProvider;
  m_player = player;
  m_playlist = playlist;
  m_searcher = musicSearcher;
  m_backgroundWorker = worker;

  auto vmLoads = DispatchToUI([&]() {
    m_searchVM = ref new SearchVM(*musicSearcher, *playlist, *m_backgroundWorker, *player);
    m_albumListVM = ref new AlbumListControlVM();
    m_playerVM = ref new SongPlayerVM(*player, *playlist, *musicProvider, *m_backgroundWorker);
    m_volumeSlideVM = ref new VolumeSliderVM(*player);
    m_guideVM = ref new GuideVM(*playlist, *m_backgroundWorker);
    m_whatIsPlayingVM = ref new WhatIsPlayingVM(*m_playlist, *m_player, *m_backgroundWorker, *m_musicProvider);

    m_playerVM->VolumeVM = m_volumeSlideVM;
    v_albumListControl->VM = m_albumListVM;
    v_bottomBar->VM = m_playerVM;

    v_guide->VM = m_guideVM;

    v_searchPane->VM = m_searchVM;

    v_whatIsPlaying->VM = m_whatIsPlayingVM;

    auto wnd = Windows::ApplicationModel::Core::CoreApplication::MainView->CoreWindow;
    wnd->KeyDown += ref new TypedEventHandler<CoreWindow ^, KeyEventArgs ^>(this, &MainPage::KeyPressed);
  });
  vmLoads.wait();

  function<void(const Events::AlbumSelectedEvent&)> albumSelctedCB = [this](const Events::AlbumSelectedEvent& ev) {
    v_guide->VM->SelectedAlbum = ev.GetSelectedAlbum();
  };
  m_albumSelectedSub = Events::EventService<Events::AlbumSelectedEvent>::RegisterListener(albumSelctedCB);

  AlbumsChangedCallback cb = [this](const AlbumCollectionChanges& albums) { this->OnAlbumsReady(albums); };
  m_albumSub = m_musicProvider->SubscribeAlbums(cb);

  // SetupTransportControls(player);
}

int FindIndexOf(AlbumList ^ albums, boost::uuids::uuid id) {
  int result = -1;

  for (auto i = 0u; i < albums->Size; ++i) {
    auto album = albums->GetAt(i);
    if (album->GetModel()->GetId() == id) {
      result = i;
      break;
    }
  }

  ARC_ASSERT(result != -1);

  return result;
}

void Arcusical::MainPage::OnAlbumsReady(const Model::AlbumCollectionChanges& albumChanges) {
  // compare the albums we have with how many are to be added and removed against the total number.
  // if the numbers match do a partial update, if they do not do a full refresh
  if (albumChanges.NewAlbums.size() + m_albumListVM->Albums->Size - albumChanges.DeletedAlbums.size() !=
      albumChanges.AllAlbums->size()) {
    // then the sizes do not add up. In this case just refresh the list and carry on
    auto allAblums =
        AlbumListControlVM::CreateAlbumList(*albumChanges.AllAlbums, *m_playlist, *m_player, *m_backgroundWorker);
    sort(begin(allAblums), end(allAblums), [](AlbumVM ^ a, AlbumVM ^ b) { return a->Title < b->Title; });
    Arcusical::DispatchToUI([this, allAblums]() { m_albumListVM->Albums = allAblums; });
  } else {
    // numbers match up, just do a partial update
    // create a list of new view models
    auto newAlbums =
        AlbumListControlVM::CreateAlbumList(albumChanges.NewAlbums, *m_playlist, *m_player, *m_backgroundWorker);

    // modified albums need to be copied out because they will but used by another thread
    vector<Album> modifiedAlbums;
    modifiedAlbums.reserve(albumChanges.ModifiedAlbums.size());
    transform(begin(albumChanges.ModifiedAlbums), end(albumChanges.ModifiedAlbums), back_inserter(modifiedAlbums),
              [](AlbumPtrCollection::value_type albumPtr) { return *albumPtr.second; });

    // for the deleted albums just make a copy of their ids, the other details are no important
    vector<boost::uuids::uuid> deletedAlbums;
    deletedAlbums.reserve(albumChanges.DeletedAlbums.size());

    transform(begin(albumChanges.DeletedAlbums), end(albumChanges.DeletedAlbums), back_inserter(deletedAlbums),
              [](AlbumPtrCollection::value_type album) { return album.first; });

    Arcusical::DispatchToUI([this, newAlbums, modifiedAlbums, deletedAlbums]() {
      // add new albums to the list
      // change the modified albums
      // and remove the deleted albums
      for (const auto&& newAlbum : newAlbums) {
        auto idx = count_if(begin(m_albumListVM->Albums), end(m_albumListVM->Albums),
                            [newAlbum](AlbumVM ^ a) { return a->Title < newAlbum->Title; });
        ARC_ASSERT(idx >= 0 && idx <= decltype(idx)(m_albumListVM->Albums->Size));
        m_albumListVM->Albums->InsertAt((unsigned int)(idx), newAlbum);
      }

      for (const auto& modified : modifiedAlbums) {
        auto index = FindIndexOf(m_albumListVM->Albums, modified.GetId());
        if (index >= 0) {
          m_albumListVM->Albums->GetAt(index)->SetFrom(modified);
        }
      }

      for (const auto& deletedId : deletedAlbums) {
        auto index = FindIndexOf(m_albumListVM->Albums, deletedId);
        if (index >= 0) {
          m_albumListVM->Albums->RemoveAt(index);
        }
      }
    });
  }
}

void Arcusical::MainPage::KeyPressed(Windows::UI::Core::CoreWindow ^ window, Windows::UI::Core::KeyEventArgs ^ e) {
  if (e->VirtualKey == Windows::System::VirtualKey::Escape) {
    v_searchPane->HideResults();
    v_whatIsPlaying->Hide();
  } else if (e->VirtualKey == Windows::System::VirtualKey::Space && !v_searchPane->IsActive()) {
    m_backgroundWorker->Append([this]() {
      if (m_player->GetIsPlaying()) {
        m_player->Stop();
      } else {
        m_player->Play();
      }
    });
  } else if (IsAlphaNumeric(e->VirtualKey)) {
    v_searchPane->SetActive();
  }
}
