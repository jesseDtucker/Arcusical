#include "pch.h"

#include "AlbumListControlVM.hpp"

using namespace Platform::Collections;
using namespace std;
using namespace Windows::Foundation::Collections;

using namespace Arcusical::Model;
using namespace Arcusical::Player;
using namespace Util;

namespace Arcusical {
namespace ViewModel {
static Vector<AlbumVM ^> ^
    CreateAlbumCollection(vector<const Album*>& albums, Playlist& playlist, IPlayer& player, BackgroundWorker& worker);

AlbumListControlVM::AlbumListControlVM() { Albums = ref new Vector<AlbumVM ^>(); }

IObservableVector<AlbumVM ^> ^ AlbumListControlVM::CreateAlbumList(const AlbumPtrCollection& albums, Playlist& playlist,
                                                                   IPlayer& player, BackgroundWorker& worker) {
  vector<const Album*> albumPtrs;
  albumPtrs.reserve(albums.size());

  transform(begin(albums), end(albums), back_inserter(albumPtrs),
            [](const Model::AlbumPtrCollection::value_type& album) { return album.second; });

  return CreateAlbumCollection(albumPtrs, playlist, player, worker);
}

IObservableVector<AlbumVM ^> ^ AlbumListControlVM::CreateAlbumList(const vector<Album>& albums, Playlist& playlist,
                                                                   IPlayer& player, BackgroundWorker& worker) {
  vector<const Album*> albumPtrs;
  albumPtrs.reserve(albums.size());

  transform(begin(albums), end(albums), back_inserter(albumPtrs), [](const Model::Album& album) { return &album; });

  return CreateAlbumCollection(albumPtrs, playlist, player, worker);
}

AlbumList ^ AlbumListControlVM::CreateAlbumList(const Model::AlbumCollection& albums, Player::Playlist& playlist,
                                                Player::IPlayer& player, BackgroundWorker& worker) {
  vector<const Album*> albumPtrs;
  albumPtrs.reserve(albums.size());

  transform(begin(albums), end(albums), back_inserter(albumPtrs),
            [](const Model::AlbumCollection::value_type& album) { return &album.second; });

  return CreateAlbumCollection(albumPtrs, playlist, player, worker);
}

Vector<AlbumVM ^> ^
    CreateAlbumCollection(vector<const Album*>& albums, Playlist& playlist, IPlayer& player, BackgroundWorker& worker) {
      auto albumVMs = ref new Vector<AlbumVM ^>();
      transform(begin(albums), end(albums), back_inserter(albumVMs), [&playlist, &player, &worker](const Album* album) {
        return ref new AlbumVM(*album, playlist, player, worker);
      });
      return albumVMs;
    }
}
}