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

		void PlaySongsAfterAndIncluding(SongVM^ song);
		void Shuffle();

	internal:
		SongListControlVM(Player::Playlist& playlist);
	private:
		Player::Playlist& m_playlist;
	};

}
}

#endif