#pragma once
#ifndef SEARCH_VM_HPP
#define SEARCH_VM_HPP

#include <string>

#include "PropertyHelper.hpp"
#include "Utility/XamlMacros.hpp"
#include "CancellationToken.hpp"

namespace Arcusical {

namespace MusicProvider
{
	class MusicSearcher;
}

namespace ViewModel{

	[Windows::UI::Xaml::Data::Bindable]
	public ref class SearchVM sealed : INotifyPropertyChanged_t
	{
	public:
		NOTIFY_PROPERTY_CHANGED_IMPL;

		PROP_SET_AND_GET_WINRT(Platform::Boolean, IsDefault);
		PROP_SET_AND_GET_WINRT(Platform::String^, SearchTerm);

	internal:
		SearchVM(MusicProvider::MusicSearcher& searcher);

	private:

		void StartSearch(Platform::String^ searchTerm);

		MusicProvider::MusicSearcher& m_searcher;
		Util::CancellationTokenRef m_searchCancelToken = nullptr;
	};
}
}
#endif