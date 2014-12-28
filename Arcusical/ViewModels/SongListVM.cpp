#include "pch.h"

#include <vector>

#include "Arc_Assert.hpp"
#include "Song.hpp"
#include "ViewModels/SongListVM.hpp"
#include "ViewModels/SongVM.hpp"

namespace Arcusical{
namespace ViewModel{

	SongListVM::SongListVM(const std::vector<Model::Song>& songs)
	{
		auto future = Arcusical::DispatchToUI([this, songs]()
		{
			List = ref new Platform::Collections::Vector<SongVM^>();

			for (auto& song : songs)
			{
				List->Append(ref new SongVM(song));
			}
		});

		future.get(); // wait for the above code to complete!
	}

}
}