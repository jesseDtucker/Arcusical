#include "pch.h"

#include <memory>
#include <unordered_map>

#include "Album.hpp"
#include "AlbumListVM.hpp"
#include "AlbumVM.hpp"

using namespace Arcusical::Player;

namespace Arcusical {
namespace ViewModel{

	AlbumListVM::AlbumListVM(Model::AlbumCollection& albums, Playlist& playlist, IPlayer& player)
	{
		// sort songs in alphabetical order
		std::vector<Model::Album*> sortedAlbums;

		for (auto& album : albums)
		{
			sortedAlbums.push_back(&album.second);
		}

		std::sort(std::begin(sortedAlbums), std::end(sortedAlbums), [](Model::Album* a, Model::Album* b)
		{
			return a->GetTitle() < b->GetTitle();
		});

		auto future = Arcusical::DispatchToUI([this, &sortedAlbums, &playlist, &player]()
		{
			this->Albums = ref new Platform::Collections::Vector<AlbumVM^>();

			for (auto& album : sortedAlbums)
			{
				this->Albums->Append(ref new AlbumVM(*album, playlist, player));
			}
		});
		future.get(); // wait for UI to complete before returning
	}

}
}