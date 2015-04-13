#include "pch.h"

#include "MusicSearcher.hpp"
#include "SearchVM.hpp"

using namespace std;
using namespace Arcusical::MusicProvider;
using namespace Arcusical::Player;
using namespace Arcusical::ViewModel;
using namespace Util;

SearchVM::SearchVM(MusicSearcher& searcher, Playlist& playlist, IPlayer& player)
	: m_searcher(searcher)
	, m_playlist(playlist)
	, m_player(player)
{
	this->SearchTerm = "Search";
	m_onSearchTermChangedSub = this->OnSearchTermChanged += [this](Platform::String^ newValue)
	{
		this->StartSearch(newValue);
	};
}

void Arcusical::ViewModel::SearchVM::StartSearch(Platform::String^ searchTerm)
{
	if (m_searchCancelToken != nullptr)
	{
		m_searchCancelToken->Cancel();
	}

	if (searchTerm->Length() == 0)
	{
		// empty string search makes no sense
		return;
	}

	wstring term{ searchTerm->Data() };
	m_searchCancelToken = make_shared<CancellationToken>();

	std::async([term, this]()
	{
		auto results = m_searcher.Find(term, m_searchCancelToken);
		if (!results.CancellationToken->IsCanceled())
		{
			AlbumListVM^ albums = ref new AlbumListVM(results.Albums, m_playlist, m_player);
			SongListVM^ songs = ref new SongListVM(results.Songs, m_playlist, m_player);
			SearchResults(albums, songs, results.CancellationToken);
		}
	});
}

void Arcusical::ViewModel::SearchVM::SelectCurrent()
{
	ARC_FAIL("TODO::JT");
}
