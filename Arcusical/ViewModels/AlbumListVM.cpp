#include "pch.h"

#include <algorithm>
#include <memory>
#include <unordered_map>

#include "Album.hpp"
#include "AlbumListVM.hpp"
#include "AlbumVM.hpp"

using namespace std;
using namespace Arcusical::Model;
using namespace Arcusical::Player;

const int BIG_ALBUM_THRESHOLD = 100; // anything with more than 100 songs is considered big

namespace Arcusical {
namespace ViewModel{

	AlbumListVM::AlbumListVM(const AlbumCollection& albums, Playlist& playlist, IPlayer& player)
	{
		vector<const Album*> albumPtrs;
		albumPtrs.resize(albums.size());

		transform(begin(albums), end(albums), begin(albumPtrs), [](const AlbumCollection::value_type& album)
		{
			return &album.second;
		});

		SortAndCreate(albumPtrs, playlist, player);
	}

	AlbumListVM::AlbumListVM(const vector<Album>& albums, Player::Playlist& playlist, Player::IPlayer& player)
	{
		vector<const Album*> albumPtrs;
		albumPtrs.resize(albums.size());

		transform(begin(albums), end(albums), begin(albumPtrs), [](const Album& album)
		{
			return &album;
		});

		SortAndCreate(albumPtrs, playlist, player);
	}

	void AlbumListVM::SortAndCreate(vector<const Album*>& albums, Playlist& playlist, IPlayer& player)
	{
		// sort songs in alphabetical order
		sort(begin(albums), end(albums), [](const Album* a, const Album* b)
		{
			return a->GetTitle() < b->GetTitle();
		});

		// most albums will have their songs loaded on demand
		// However, really big albums take too long for the UI to handle
		// so we'll pre-load those
		vector<AlbumVM^> bigAlbums;
		auto albumListVM = ref new Platform::Collections::Vector<AlbumVM^>();

		for (auto& album : albums)
		{
			auto albumVM = ref new AlbumVM(*album, playlist, player);
			albumListVM->Append(albumVM);
			if (album->GetSongIds().size() > BIG_ALBUM_THRESHOLD)
			{
				bigAlbums.push_back(albumVM);
			}
		}

		if (bigAlbums.size() > 0)
		{
			// go ahead and do the song pre-load for these monstrous albums
			async([bigAlbums]()
			{
				for (auto bigAlbum : bigAlbums)
				{
					bigAlbum->Songs;
				}
			});
		}

		auto future = Arcusical::DispatchToUI([this, albumListVM]()
		{
			this->Albums = albumListVM;
		});
		future.get(); // wait for UI to complete before returning
	}

}
}