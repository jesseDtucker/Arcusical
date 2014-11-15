#include "pch.h"

#include <vector>

#include "Arc_Assert.hpp"
#include "Song.hpp"
#include "ViewModels/SongListVM.hpp"
#include "ViewModels/SongVM.hpp"

namespace Arcusical{
namespace ViewModel{

	SongListVM::SongListVM(const Model::SongPtrCollection& songs)
	{
		// temp hack
		std::vector<Model::Song*> sortedSongs;

		for (auto& song : songs)
		{
			sortedSongs.push_back(song.second);
		}

		std::sort(sortedSongs.begin(), sortedSongs.end(), [](Model::Song* a, Model::Song* b)
		{
			return a->GetTitle() < b->GetTitle();
		});

		auto future = Arcusical::DispatchToUI([this, sortedSongs]()
		{
			List = ref new Platform::Collections::Vector<SongVM^>();

			for (auto& song : sortedSongs)
			{
				List->Append(ref new SongVM(*song));
			}
		});

		future.get(); // wait for the above code to complete!
	}

}
}