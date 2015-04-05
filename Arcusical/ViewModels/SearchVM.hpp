#pragma once
#ifndef SEARCH_VM_HPP
#define SEARCH_VM_HPP

#include <string>

#include "MulticastDelegate.hpp"
#include "PropertyHelper.hpp"
#include "Utility/XamlMacros.hpp"
#include "ViewModels/AlbumListVM.hpp"
#include "ViewModels/SongListVM.hpp"
#include "CancellationToken.hpp"

namespace Arcusical {
	namespace Player { class IPlayer; class Playlist; }
	namespace MusicProvider{ class MusicSearcher; }

namespace ViewModel{

	[Windows::UI::Xaml::Data::Bindable]
	public ref class SearchVM sealed : INotifyPropertyChanged_t
	{
	public:
		NOTIFY_PROPERTY_CHANGED_IMPL;

		PROP_SET_AND_GET_WINRT(Platform::Boolean, IsDefault);
		PROP_SET_AND_GET_WINRT(Platform::String^, SearchTerm);

		void SelectCurrent();

	internal:
		SearchVM(MusicProvider::MusicSearcher& searcher, Player::Playlist& playlist, Player:: IPlayer& player);
		Util::MulticastDelegate<void(AlbumListVM^, SongListVM^)> SearchResults;

	private:

		void StartSearch(Platform::String^ searchTerm);

		MusicProvider::MusicSearcher& m_searcher;
		Player::Playlist& m_playlist;
		Player::IPlayer& m_player;

		Util::CancellationTokenRef m_searchCancelToken = nullptr;
		Util::Subscription m_onSearchTermChangedSub;
	};
}
}
#endif