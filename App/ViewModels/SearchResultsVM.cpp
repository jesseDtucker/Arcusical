#include "pch.h"

#include "SearchResultsVM.hpp"

using namespace std;

using namespace Arcusical::Player;
using namespace Arcusical::ViewModel;

SearchResultsVM::SearchResultsVM(SearchVM ^ searchVm, Playlist& playlist, Util::BackgroundWorker& worker)
    : m_playlist(playlist), m_worker(worker) {
  this->Albums = ref new AlbumListControlVM();
  this->Songs = ref new SongListControlVM(playlist, worker);
  m_resultsSub = searchVm->SearchResults += [this](ViewModel::AlbumList ^ albums, ViewModel::SongListVM ^ songs,
                                                   Util::CancellationTokenRef token) {
    auto future = DispatchToUI([this, &albums, &songs, token]() {
      // double checking for cancel just in case one occurred while we were waiting to update the UI
      if (!token->IsCanceled()) {
        this->Albums->Albums = albums;
        this->Songs->SongList = songs;
        auto numElements = albums->Size + songs->List->Size;
        this->HasResults = numElements > 0;
      }
    });
    future.get();
  };
  m_selectedSub = searchVm->SelectActive += [this]() {
    m_worker.Append([this]() {
      vector<Model::Song> songs;
      if (this->Albums->Albums->Size > 0) {
        for (auto& album : this->Albums->Albums) {
          auto albumSongs = *album->GetModel()->GetSongs();
          songs.insert(end(songs), begin(albumSongs), end(albumSongs));
        }
      } else if (this->Songs->SongList->List->Size > 0) {
        songs.resize(this->Songs->SongList->List->Size);
        transform(begin(this->Songs->SongList->List), end(this->Songs->SongList->List), begin(songs),
                  [](auto songVM) { return *songVM->GetModel(); });
      }
      if (songs.size() > 0) {
        m_playlist.Clear();
        m_playlist.Enqueue(songs, true);
      }
      DispatchToUI([this]() { this->HasResults = false; });
    });
  };
}
