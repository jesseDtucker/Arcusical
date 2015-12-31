//
// App.xaml.cpp
// Implementation of the App class.
//

#include "pch.h"

#include "Pages/MainPage.xaml.h"

#include <condition_variable>
#include <future>
#include <memory>
#include <mutex>

#include "AsyncProcessor.hpp"
#include "../src/File.hpp"  // Another temp hack. FileSystem library needs a reimagining.
#include "Playlist.hpp"
#include "MusicProvider.hpp"
#include "../src/Win8Player.hpp"  // This is a hack for the time being. I need something similar to flow engine for resolving dependencies...
#include "MusicSearcher.hpp"
#include "Storage.hpp"

using namespace Platform;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

using namespace std;
using namespace Arcusical;
using namespace Arcusical::LocalMusicStore;
using namespace Arcusical::MusicProvider;
using namespace Arcusical::Player;
using namespace FileSystem;

/// <summary>
/// Initializes the singleton application object.  This is the first line of authored code
/// executed, and as such is the logical equivalent of main() or WinMain().
/// </summary>
App::App() : m_cache(), m_musicProvider(&m_cache), m_searcher(&m_cache) {
  InitializeComponent();
  Suspending += ref new SuspendingEventHandler(this, &App::OnSuspending);

  SetupApplication();
}

void App::SetupApplication() {
  // some of the services use COM and so should not be initialized on the UI thread
  m_backgroundWorker.Append([&]() {
    m_player = make_unique<Win8Player>(m_backgroundWorker);
    m_playlist = make_unique<Playlist>(m_player.get(), &m_musicProvider);
  });
}

/// <summary>
/// Invoked when the application is launched normally by the end user.	Other entry points
/// will be used such as when the application is launched to open a specific file.
/// </summary>
/// <param name="e">Details about the launch request and process.</param>
void App::OnLaunched(Windows::ApplicationModel::Activation::LaunchActivatedEventArgs ^ e) {

#if _DEBUG
  // Show graphics profiling information while debugging.
  if (IsDebuggerPresent()) {
    // Display the current frame rate counters
    DebugSettings->EnableFrameRateCounter = false;
  }
#endif

  auto rootFrame = dynamic_cast<Frame ^ >(Window::Current->Content);
  auto args = (e != nullptr) ? e->Arguments : "";

  // Do not repeat app initialization when the Window already has content,
  // just ensure that the window is active
  if (rootFrame == nullptr) {
    // Create a Frame to act as the navigation context and associate it with
    // a SuspensionManager key
    rootFrame = ref new Frame();

    // Set the default language
    rootFrame->Language = Windows::Globalization::ApplicationLanguages::Languages->GetAt(0);

    rootFrame->NavigationFailed +=
        ref new Windows::UI::Xaml::Navigation::NavigationFailedEventHandler(this, &App::OnNavigationFailed);

    if (rootFrame->Content == nullptr) {
      // When the navigation stack isn't restored navigate to the first page,
      // configuring the new page by passing required information as a navigation
      // parameter
      rootFrame->Navigate(TypeName(MainPage::typeid), args);
    }

    MainPage ^ mainPage = dynamic_cast<MainPage ^ >(rootFrame->Content);
    ARC_ASSERT(mainPage != nullptr);
    m_backgroundWorker.Start();
    m_backgroundWorker.Append([mainPage, this]() {
      mainPage->SetDependencies(&m_searcher, &m_musicProvider, m_player.get(), m_playlist.get(), &m_backgroundWorker);
    });

    // Place the frame in the current Window
    Window::Current->Content = rootFrame;
    // Ensure the current window is active
    Window::Current->Activate();
  } else {
    if (rootFrame->Content == nullptr) {
      // When the navigation stack isn't restored navigate to the first page,
      // configuring the new page by passing required information as a navigation
      // parameter
      rootFrame->Navigate(TypeName(MainPage::typeid), args);
    }
    // Ensure the current window is active
    Window::Current->Activate();
  }
}

/// <summary>
/// Invoked when application execution is being suspended.	Application state is saved
/// without knowing whether the application will be terminated or resumed with the contents
/// of memory still intact.
/// </summary>
/// <param name="sender">The source of the suspend request.</param>
/// <param name="e">Details about the suspend request.</param>
void App::OnSuspending(Object ^ sender, SuspendingEventArgs ^ e) {
  (void)sender;  // Unused parameter
  (void)e;       // Unused parameter

  // TODO: Save application state and stop any background activity
}

/// <summary>
/// Invoked when Navigation to a certain page fails
/// </summary>
/// <param name="sender">The Frame which failed navigation</param>
/// <param name="e">Details about the navigation failure</param>
void App::OnNavigationFailed(Platform::Object ^ sender, Windows::UI::Xaml::Navigation::NavigationFailedEventArgs ^ e) {
  throw ref new FailureException("Failed to load Page " + e->SourcePageType.Name);
}

void App::OnFileActivated(Windows::ApplicationModel::Activation::FileActivatedEventArgs ^ args) {
  OnLaunched(nullptr);
  m_backgroundWorker.Append([this, args]() {
    vector<shared_ptr<FileSystem::IFile> > files;
    transform(begin(args->Files), end(args->Files), back_inserter(files), [](auto storageItem) {
      auto storageFile = dynamic_cast<StorageFile ^ >(storageItem);  // Should be safe for now.
      ARC_ASSERT(storageFile != nullptr);  // TODO::JT remove this assert and sort out the filesystem library.
      return make_shared<FileSystem::File>(storageFile);
    });
    auto songs = m_musicProvider.GetSongsFromFiles(files);
    m_playlist->Clear();
    m_playlist->Enqueue(songs, true);
  });
}