#pragma once
#ifndef SEARCH_RESULTS_HPP
#define SEARCH_RESULTS_HPP

#include "PropertyHelper.hpp"
#include "ViewModels/AlbumListControlVM.hpp"
#include "ViewModels/SearchVM.hpp"
#include "ViewModels/SongListControlVM.hpp"
#include "Subscription.hpp"
#include "Utility/XamlMacros.hpp"

namespace Arcusical
{
	namespace Player { class Playlist; }

	namespace ViewModel
	{
		[Windows::UI::Xaml::Data::Bindable]
		public ref class SearchResultsVM sealed : INotifyPropertyChanged_t
		{
		public:
			NOTIFY_PROPERTY_CHANGED_IMPL;

			PROP_SET_AND_GET_WINRT(ViewModel::AlbumListControlVM^, Albums);
			PROP_SET_AND_GET_WINRT(ViewModel::SongListControlVM^, Songs);
			PROP_SET_AND_GET_WINRT(bool, HasResults);

		internal:
			SearchResultsVM(SearchVM^ searchVm, Player::Playlist& playlist, Util::BackgroundWorker& worker);
		private:
			Util::Subscription m_resultsSub;
			Util::Subscription m_selectedSub;
			Player::Playlist& m_playlist;
			Util::BackgroundWorker& m_worker;
		};
	}
}

#endif