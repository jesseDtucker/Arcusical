#include "pch.h"

#include <vector>

#include "Arc_Assert.hpp"
#include "Song.hpp"
#include "ViewModels/SongListVM.hpp"
#include "ViewModels/SongVM.hpp"

namespace Arcusical{
namespace ViewModel{

	SongListVM::SongListVM(MusicProvider::SongListPtr songs)
	{
		List = ref new Platform::Collections::Vector<SongVM^>();

		auto songsPtr = songs.lock();

		ARC_ASSERT(songsPtr != nullptr);
		if (songsPtr != nullptr)
		{
			// temp hack
			std::vector<std::shared_ptr<Model::Song>> sortedSongs;

			for (auto& song : *songsPtr)
			{
				sortedSongs.push_back(song.second);
			}

			std::sort(sortedSongs.begin(), sortedSongs.end(), [](std::shared_ptr<Model::Song> a, std::shared_ptr<Model::Song> b)
			{
				return a->GetTitle() > b->GetTitle();
			});

			for (auto& song : sortedSongs)
			{
				List->Append(ref new SongVM(song));
			}
		}
	}

}
}