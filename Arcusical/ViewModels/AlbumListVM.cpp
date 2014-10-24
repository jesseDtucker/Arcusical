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

		//TODO::JT fix the threading issue that is here!
		this->Albums = ref new Platform::Collections::Vector<AlbumVM^>();

		int j = 1, i = 1;
		// arbitrary delay
		while (this->Albums == nullptr);
		{
			j = (i + j) / 2;
			++i;
		}

		// DNS
		ARC_ASSERT(this->Albums != nullptr);
		for (auto album : *albumsCollection)
		{
			this->Albums->Append(ref new AlbumVM(album.second));
		}
	}

}
}