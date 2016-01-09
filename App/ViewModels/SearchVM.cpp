#include "pch.h"

#include "SearchVM.hpp"

#include "AlbumListControlVM.hpp"
#include "SongListControlVM.hpp"

using namespace std;

using namespace Arcusical::MusicProvider;
using namespace Arcusical::Player;
using namespace Arcusical::ViewModel;
using namespace Util;

SearchVM::SearchVM(MusicSearcher& searcher, Playlist& playlist, BackgroundWorker& worker, IPlayer& player)
    : m_searcher(searcher), m_playlist(playlist), m_worker(worker), m_player(player) {
  this->AlbumListControlVM = ref new ViewModel::AlbumListControlVM();
  this->SongListControlVM = ref new ViewModel::SongListControlVM(playlist, worker);

  this->SearchTerm = "";
  m_onSearchTermChangedSub = this->OnSearchTermChanged +=
      [this](Platform::String ^ newValue) { this->StartSearch(newValue); };
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

  m_worker.Append([term, this]() {
    auto results = m_searcher.Find(term, m_searchCancelToken);
    DispatchToUI([this, results]() {
      if (!results.CancellationToken->IsCanceled()) {
        this->AlbumListControlVM->Albums =
            ViewModel::AlbumListControlVM::CreateAlbumList(results.Albums, m_playlist, m_player, m_worker);
        this->SongListControlVM->SongList = ref new SongListVM(results.Songs, m_playlist, m_player, m_worker);
      }
    });
  });
}

void SearchVM::SelectCurrent() {
  m_worker.Append([this]() {
    vector<Model::Song> songs;
    if (this->AlbumListControlVM->Albums->Size > 0) {
      for (auto& album : this->AlbumListControlVM->Albums) {
        auto albumSongs = *album->GetModel()->GetSongs();
        songs.insert(end(songs), begin(albumSongs), end(albumSongs));
      }
    } else if (this->SongListControlVM->SongList->List->Size > 0) {
      songs.resize(this->SongListControlVM->SongList->List->Size);
      transform(begin(this->SongListControlVM->SongList->List), end(this->SongListControlVM->SongList->List),
                begin(songs), [](auto songVM) { return *songVM->GetModel(); });
    }
    if (songs.size() > 0) {
      m_playlist.Clear();
      m_playlist.Enqueue(songs, true);
    }
  });
}