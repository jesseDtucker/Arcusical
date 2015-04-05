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

namespace Arcusical {
namespace ViewModel{

	AlbumListVM::AlbumListVM(AlbumCollection& albums, Playlist& playlist, IPlayer& player)
	{
		vector<Album*> albumPtrs;
		albumPtrs.resize(albums.size());

		transform(begin(albums), end(albums), begin(albumPtrs), [](AlbumCollection::value_type& album)
		{
			return &album.second;
		});

		SortAndCreate(albumPtrs, playlist, player);
	}

	AlbumListVM::AlbumListVM(vector<Album>& albums, Player::Playlist& playlist, Player::IPlayer& player)
	{
		vector<Album*> albumPtrs;
		albumPtrs.resize(albums.size());

		transform(begin(albums), end(albums), begin(albumPtrs), [](Album& album)
		{
			return &album;
		});

		SortAndCreate(albumPtrs, playlist, player);
	}

	void AlbumListVM::SortAndCreate(vector<Album*>& albums, Playlist& playlist, IPlayer& player)
	{
		// sort songs in alphabetical order
		sort(begin(albums), end(albums), [](Album* a, Album* b)
		{
			return a->GetTitle() < b->GetTitle();
		});

		auto future = Arcusical::DispatchToUI([this, &albums, &playlist, &player]()
		{
			this->Albums = ref new Platform::Collections::Vector<AlbumVM^>();

			for (auto& album : albums)
			{
				this->Albums->Append(ref new AlbumVM(*album, playlist, player));
			}
		});
		future.get(); // wait for UI to complete before returning
	}

}
}