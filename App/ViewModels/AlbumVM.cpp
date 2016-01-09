#include "pch.h"

#include "AlbumVM.hpp"

#include <future>

#include "Album.hpp"
#include "Playlist.hpp"

using namespace Windows::UI::Xaml::Data;

using namespace std;
using namespace Arcusical::Player;
using namespace Util;

namespace Arcusical {
namespace ViewModel {

AlbumVM::AlbumVM(const Model::Album& album, Playlist& playlist, IPlayer& player, BackgroundWorker& worker)
    : m_album(album), m_playlist(playlist), m_songs(nullptr), m_player(player), m_worker(worker) {
  this->m_Artist = ref new Platform::String(album.GetArtist().c_str());
  this->m_Title = ref new Platform::String(album.GetTitle().c_str());
  this->m_ImagePath = ref new Platform::String(album.GetImageFilePath().c_str());
}

AlbumVM::AlbumVM(const AlbumVM ^ albumVM)
    : m_album(albumVM->m_album),
      m_playlist(albumVM->m_playlist),
      m_songs(albumVM->m_songs),
      m_player(albumVM->m_player),
      m_worker(albumVM->m_worker) {
  this->m_Artist = ref new Platform::String(m_album.GetArtist().c_str());
  this->m_Title = ref new Platform::String(m_album.GetTitle().c_str());
  this->m_ImagePath = ref new Platform::String(m_album.GetImageFilePath().c_str());
}

void AlbumVM::Play() {
  m_worker.Append([this]() {
    m_playlist.Clear();
    m_playlist.Enqueue(*m_album.GetSongs());
  });
}

const Model::Album* AlbumVM::GetModel() const { return &m_album; }

void AlbumVM::SetFrom(const Model::Album& album) {
  m_album = album;
  Artist = ref new Platform::String(album.GetArtist().c_str());
  Title = ref new Platform::String(album.GetTitle().c_str());
  ImagePath = ref new Platform::String(album.GetImageFilePath().c_str());

  // invalidate the songs if the album has a different amount. They will get
  // reloaded on demand
  if (m_songs != nullptr && m_songs->List->Size != album.GetSongIds().size()) {
    m_songs = nullptr;
    OnPropertyChanged("Songs");
  }
}

SongListVM ^ AlbumVM::Songs::get() {
  if (m_songs == nullptr || m_album.GetSongIds().size() != m_songs->List->Size) {
    auto songs = m_album.GetSongs();
    m_songs = ref new SongListVM(*songs, m_playlist, m_player, m_worker);

    DispatchToUI([this]() { OnPropertyChanged("Songs"); });
  }

  return m_songs;
}

} /*ViewModel*/
} /*Arcusical*/