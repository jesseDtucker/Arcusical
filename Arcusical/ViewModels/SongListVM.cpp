#include "pch.h"

#include <vector>

#include "Arc_Assert.hpp"
#include "Song.hpp"
#include "ViewModels/SongListVM.hpp"
#include "ViewModels/SongVM.hpp"

namespace Arcusical{
namespace ViewModel{

	SongListVM::SongListVM(Model::SongCollection& songs)
	{
		List = ref new Platform::Collections::Vector<SongVM^>();

		// temp hack
		std::vector<Model::Song*> sortedSongs;

		for (auto& song : songs)
		{
			sortedSongs.push_back(&song.second);
		}

		std::sort(sortedSongs.begin(), sortedSongs.end(), [](Model::Song* a, Model::Song* b)
		{
			return a->GetTitle() < b->GetTitle();
		});

		for (auto& song : sortedSongs)
		{
			List->Append(ref new SongVM(*song));
		}
	}

}
}