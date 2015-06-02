#include "pch.h"

#include "SearchResultsVM.hpp"

using namespace Arcusical::Player;
using namespace Arcusical::ViewModel;

SearchResultsVM::SearchResultsVM(SearchVM^ searchVm, Playlist& playlist)
{
	this->Albums = ref new AlbumListControlVM();
	this->Songs = ref new SongListControlVM(playlist);
	m_resultsSub = searchVm->SearchResults += [this](ViewModel::AlbumList^ albums, ViewModel::SongListVM^ songs, Util::CancellationTokenRef token)
	{
		auto future = DispatchToUI([this, &albums, &songs, token]()
		{
			// double checking for cancel just in case one occurred while we were waiting to update the UI
			if (!token->IsCanceled())
			{
				this->Albums->Albums = albums;
				this->Songs->SongList = songs;
			}
		});
		future.get();
	};
}
