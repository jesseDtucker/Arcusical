#include "pch.h"

#include <future>
#include <vector>

#include "Arc_Assert.hpp"
#include "Song.hpp"
#include "ViewModels/SongListVM.hpp"
#include "ViewModels/SongVM.hpp"

using namespace Arcusical::Player;

namespace Arcusical{
namespace ViewModel{

	SongListVM::SongListVM(const std::vector<Model::Song>& songs, Playlist& playlist, IPlayer& player)
	{
		auto future = Arcusical::DispatchToUI([this, &songs, &player, &playlist]()
		{
			List = ref new Platform::Collections::Vector<SongVM^>();

			bool isAlternate = false;

			for (auto& song : songs)
			{
				auto songVM = ref new SongVM(song, playlist, player);
				songVM->IsAlternate = isAlternate;
				isAlternate = !isAlternate;
				List->Append(songVM);
			}
		});

		future.get(); // wait for the above code to complete!
	}

}
}