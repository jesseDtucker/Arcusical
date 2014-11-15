#ifndef SONG_LIST_CONTROL_VM_HPP
#define SONG_LIST_CONTROL_VM_HPP

#include "Utility/XamlMacros.hpp"
#include "Subscription.hpp"

namespace Arcusical{
	namespace Events
	{
		class AlbumSelectedEvent;
}
namespace ViewModel{

	ref class SongListVM;

	[Windows::UI::Xaml::Data::Bindable]
	public ref class SongListControlVM sealed : INotifyPropertyChanged_t
	{
	public:
		NOTIFY_PROPERTY_CHANGED_IMPL;

		PROP_SET_AND_GET_WINRT(SongListVM^, SongList);

	internal:
		SongListControlVM();
	private:
		void OnAlbumSelected(const Events::AlbumSelectedEvent& event);
		Util::SubscriptionPtr m_albumSelectedSubscription;
	};

}
}

#endif