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

			for (auto& song : songs)
			{
				List->Append(ref new SongVM(song, playlist, player));
			}
		});

		future.get(); // wait for the above code to complete!
	}

}
}