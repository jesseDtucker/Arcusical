#pragma once


namespace Arcusical {
namespace ViewModel {
ref class AlbumVM;
}
namespace Events {

class AlbumSelectedEvent final {
 public:
  AlbumSelectedEvent(ViewModel::AlbumVM ^ selectedAlbum);
  ViewModel::AlbumVM ^ GetSelectedAlbum() const;

 private:
  ViewModel::AlbumVM ^ m_selectedAlbum;
};
}
}
