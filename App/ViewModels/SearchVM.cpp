#include "pch.h"

#include "SearchVM.hpp"

#include "AlbumListControlVM.hpp"
#include "SongListControlVM.hpp"

using namespace std;

using namespace Arcusical::MusicProvider;
using namespace Arcusical::Player;
using namespace Arcusical::ViewModel;
using namespace Util;

static const int MAX_ALBUMS = 21;
static const int MAX_SONGS = 100;

SearchVM::SearchVM(MusicSearcher& searcher, Playlist& playlist, BackgroundWorker& worker, IPlayer& player)
    : m_searcher(searcher), m_playlist(playlist), m_worker(worker), m_player(player), m_latestResults(nullptr) {
  this->AlbumListControlVM = ref new ViewModel::AlbumListControlVM();
  this->SongListControlVM = ref new ViewModel::SongListControlVM(playlist, worker);

  this->SearchTerm = "";
  m_onSearchTermChangedSub = this->OnSearchTermChanged +=
      [this](Platform::String ^ newValue) { this->StartSearch(newValue); };
  m_searchWorker.Start();
}

void TrimSearchResults(SearchResult& searchResults) {
  if (searchResults.Albums.size() > MAX_ALBUMS) {
    // safe to add an empty album as we are shrinking and so none will be created.
    searchResults.Albums.resize(MAX_ALBUMS, {nullptr});
  }
  if (searchResults.Songs.size() > MAX_SONGS) {
    searchResults.Songs.resize(MAX_SONGS);
  }
}

void Arcusical::ViewModel::SearchVM::StartSearch(Platform::String ^ searchTerm) {
  if (m_searchCancelToken != nullptr) {
    m_searchCancelToken->Cancel();
  }

  if (searchTerm->Length() == 0) {
    // empty string search makes no sense
    return;
  }

  wstring term{searchTerm->Data()};
  m_searchCancelToken = make_shared<CancellationToken>();

  m_searchWorker.Append([term, this]() {
    bool updateUI = false;
    auto results = m_searcher.Find(term, m_searchCancelToken);
    // Trim the results so that the UI does not get overburdened.
    TrimSearchResults(results);
    {
      unique_lock<mutex>(m_latestLock);
      if (m_latestResults != results &&
          (results.Albums.size() > 0 && results.Songs.size() > 0 || !results.CancellationToken->IsCanceled())) {
        updateUI = true;
        areLatestSelected = false;
        m_latestResults = results;
        m_latestAlbums = ViewModel::AlbumListControlVM::CreateAlbumList(results.Albums, m_playlist, m_player, m_worker);
        m_latestSongs = ref new SongListVM(results.Songs, m_playlist, m_player, m_worker);
      }
    }
    if (updateUI) {
      DispatchToUI([this, results]() {
        unique_lock<mutex>(m_latestLock);
        this->AlbumListControlVM->Albums = m_latestAlbums;
        this->SongListControlVM->SongList = m_latestSongs;
      });
    }
  });
}

void SearchVM::SelectCurrent() {
  if (areLatestSelected) {
    return;
  }
  areLatestSelected = true;
  m_worker.Append([this]() {
    vector<Model::Album> albumResults;
    vector<Model::Song> songResults;
    {
      unique_lock<mutex>(m_latestLock);
      albumResults = m_latestResults.Albums;
      songResults = m_latestResults.Songs;
    }
    vector<Model::Song> songs;
    if (albumResults.size() > 0) {
      for (auto& album : albumResults) {
        auto albumSongs = album.GetSongs();
        songs.insert(end(songs), begin(*albumSongs), end(*albumSongs));
      }
    } else if (songResults.size() > 0) {
      songs = move(songResults);
    }
    if (songs.size() > 0) {
      m_playlist.Clear();
      m_playlist.Enqueue(songs, true);
    }
  });
}