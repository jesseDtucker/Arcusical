#include "pch.h"

#include "MusicSearcher.hpp"
#include "SearchVM.hpp"

using namespace std;
using namespace Arcusical::ViewModel;
using namespace Arcusical::MusicProvider;
using namespace Util;

SearchVM::SearchVM(MusicSearcher& searcher)
	: m_searcher(searcher)
{
	this->OnSearchTermChanged += [this](Platform::String^ newValue)
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

	wstring term{ searchTerm->Data() };
	m_searchCancelToken = make_shared<CancellationToken>();

	std::async([term, this]()
	{
		auto results = m_searcher.Find(term, m_searchCancelToken);
		if (!results.CancellationToken->IsCanceled())
		{

		}
	});
}
