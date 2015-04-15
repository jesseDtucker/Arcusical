#ifndef ALBUM_LIST_VM_HPP
#define ALBUM_LIST_VM_HPP

#include <vector>

#include "PropertyHelper.hpp"
#include "Utility/DispatcherHelper.hpp"
#include "Utility/XamlMacros.hpp"

namespace Arcusical{
namespace Player {
	class Playlist;
	class IPlayer;
}
}

namespace Arcusical{
namespace ViewModel
{
	[Windows::UI::Xaml::Data::Bindable]
	public ref class AlbumListVM sealed : INotifyPropertyChanged_t
	{
	public:
		NOTIFY_PROPERTY_CHANGED_IMPL;

		PROP_SET_AND_GET_WINRT(Windows::Foundation::Collections::IVector<AlbumVM^>^, Albums);
	internal:
		
		AlbumListVM(const Model::AlbumCollection& albums, Player::Playlist& playlist, Player::IPlayer& player);
		AlbumListVM(const std::vector<Model::Album>& albums, Player::Playlist& playlist, Player::IPlayer& player);

	private:
		void SortAndCreate(std::vector<const Model::Album*>& albums, Player::Playlist& playlist, Player::IPlayer& player);
	};
}
}

#endif