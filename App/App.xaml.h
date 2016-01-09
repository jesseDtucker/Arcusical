#pragma once

#include <memory>

#include "App.g.h"
#include "AsyncProcessor.hpp"
#include "IPlayer.hpp"
#include "LocalMusicCache.hpp"
#include "MusicProvider.hpp"
#include "MusicSearcher.hpp"
#include "Playlist.hpp"
#include "Subscription.hpp"

namespace Arcusical {
  /// <summary>
  /// Provides application-specific behavior to supplement the default Application class.
  /// </summary>
  ref class App sealed {
   protected:
    virtual void OnLaunched(Windows::ApplicationModel::Activation::LaunchActivatedEventArgs ^ e) override;
    virtual void OnFileActivated(Windows::ApplicationModel::Activation::FileActivatedEventArgs ^ args) override;

    internal : App();

    void SetupApplication();

   private:
    void OnSuspending(Platform::Object ^ sender, Windows::ApplicationModel::SuspendingEventArgs ^ e);
    void OnNavigationFailed(Platform::Object ^ sender, Windows::UI::Xaml::Navigation::NavigationFailedEventArgs ^ e);

    LocalMusicStore::LocalMusicCache m_cache;
    MusicProvider::MusicProvider m_musicProvider;
    MusicProvider::MusicSearcher m_searcher;
    std::unique_ptr<Player::Playlist> m_playlist;
    std::unique_ptr<Player::IPlayer> m_player;
    Util::BackgroundWorker m_backgroundWorker;
  };
}
