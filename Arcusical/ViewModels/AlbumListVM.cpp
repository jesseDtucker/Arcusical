#include "pch.h"

#include <memory>
#include <unordered_map>

#include "Album.hpp"
#include "AlbumListVM.hpp"
#include "AlbumVM.hpp"

namespace Arcusical {
namespace ViewModel{

	AlbumListVM::AlbumListVM(MusicProvider::AlbumListPtr albums)
	{
		auto albumsCollection = albums.lock();

		this->Albums = ref new Platform::Collections::Vector<AlbumVM^>();

		for (auto album : *albumsCollection)
		{
			this->Albums->Append(ref new AlbumVM(album.second));
		}
	}

}
}