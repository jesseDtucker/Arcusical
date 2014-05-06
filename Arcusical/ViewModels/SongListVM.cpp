#include "pch.h"

#include "Song.hpp"
#include "ViewModels/SongListVM.hpp"
#include "ViewModels/SongVM.hpp"

#include "Arc_Assert.hpp"

namespace Arcusical{
namespace ViewModel{

	SongListVM::SongListVM(std::weak_ptr<std::unordered_map<boost::uuids::uuid, std::shared_ptr<Model::Song>>> songs)
	{
		List = ref new Platform::Collections::Vector<SongVM^>();

		auto songsPtr = songs.lock();

		ARC_ASSERT(songsPtr != nullptr);
		if (songsPtr != nullptr)
		{
			for (auto song : *songsPtr)
			{
				List->Append(ref new SongVM(song.second));
			}
		}
	}

}
}