#include "pch.h"

#include "Song.hpp"
#include "ViewModels/SongListVM.hpp"
#include "ViewModels/SongVM.hpp"

#include "Arc_Assert.hpp"

namespace Arcusical{
namespace ViewModel{

	SongListVM::SongListVM(MusicProvider::SongListPtr songs)
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