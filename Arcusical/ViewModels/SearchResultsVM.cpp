#include "pch.h"

#include "SearchResultsVM.hpp"

using namespace Arcusical::Player;
using namespace Arcusical::ViewModel;

SearchResultsVM::SearchResultsVM(SearchVM^ searchVm, Playlist& playlist)
{
	this->Albums = ref new AlbumListControlVM();
	this->Songs = ref new SongListControlVM(playlist);
	m_resultsSub = searchVm->SearchResults += [this](ViewModel::AlbumListVM^ albums, ViewModel::SongListVM^ songs)
	{
		auto future = DispatchToUI([this, &albums, &songs]()
		{
			this->Albums->AlbumList = albums;
			this->Songs->SongList = songs;
		});
		future.get();
	};
}
