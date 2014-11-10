#include "pch.h"

#include <memory>
#include <unordered_map>

#include "Album.hpp"
#include "AlbumListVM.hpp"
#include "AlbumVM.hpp"

namespace Arcusical {
namespace ViewModel{

	AlbumListVM::AlbumListVM(Model::AlbumCollection& albums)
	{
		auto future = Arcusical::DispatchToUI([this, &albums]()
		{
			this->Albums = ref new Platform::Collections::Vector<AlbumVM^>();

			for (auto& album : albums)
			{
				this->Albums->Append(ref new AlbumVM(album.second));
			}
		});
		future.get(); // wait for UI to complete before returning
	}

}
}