#include "ViewModels/GuideVM.hpp"

#include <algorithm>
#include <random>

#include "Playlist.hpp"

using namespace Arcusical;
using namespace Arcusical::ViewModel;

GuideVM::GuideVM(Player::Playlist& playlist, Util::BackgroundWorker& worker)
    : m_selectedAlbum(nullptr), m_playlist(playlist), m_worker(worker) {}

void GuideVM::SelectedAlbum::set(AlbumVM ^ selectedAlbum) {
  if (m_selectedAlbum == nullptr) {
    m_selectedAlbum = ref new AlbumVM(selectedAlbum);
  } else {
    m_selectedAlbum->SetFrom(*selectedAlbum->GetModel());
  }
  auto songListVM = ref new ViewModel::SongListControlVM(m_playlist, m_worker);
  songListVM->SongList = selectedAlbum->Songs;
  this->SongListControlVM = songListVM;

  DispatchToUI([this]() { OnPropertyChanged("SelectedAlbum"); });
}

AlbumVM ^ GuideVM::SelectedAlbum::get() { return m_selectedAlbum; }

void GuideVM::PlayAll() {
  if (m_selectedAlbum != nullptr) {
    m_worker.Append([this]() {
      auto albumSongs = m_selectedAlbum->GetModel()->GetSongs();
      m_playlist.Clear();
      m_playlist.Enqueue(*albumSongs, true);
    });
  }
}

void GuideVM::Shuffle() {
  if (m_selectedAlbum != nullptr) {
    m_worker.Append([this]() {
      auto albumSongs = *m_selectedAlbum->GetModel()->GetSongs();
      std::shuffle(begin(albumSongs), end(albumSongs), std::mt19937(std::random_device{}()));
      m_playlist.Clear();
      m_playlist.Enqueue(albumSongs);
    });
  }
}