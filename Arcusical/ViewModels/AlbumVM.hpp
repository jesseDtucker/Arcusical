#ifndef ALBUM_VM_HPP
#define ALBUM_VM_HPP

#include <memory>

#include "PropertyHelper.hpp"
#include "ViewModels/SongListVM.hpp"
#include "Utility/DispatcherHelper.hpp"
#include "Utility/XamlMacros.hpp"

namespace Arcusical{
namespace Model { class Album; }
namespace Player { class Playlist; class IPlayer; }
}

namespace Arcusical {
namespace ViewModel{

	[Windows::UI::Xaml::Data::Bindable]
	public ref class AlbumVM sealed : INotifyPropertyChanged_t
	{
	public:
		NOTIFY_PROPERTY_CHANGED_IMPL;

		PROP_SET_AND_GET_WINRT(Platform::String^, Title);
		PROP_SET_AND_GET_WINRT(Platform::String^, Artist);
		PROP_SET_AND_GET_WINRT(Platform::String^, ImagePath);

		property SongListVM^ Songs
		{
			SongListVM^ get();
		}

		void Play();
		
	internal:
		AlbumVM(const Model::Album& album, Player::Playlist& playlist, Player::IPlayer& player);

	private:
		Model::Album m_album;
		Player::Playlist& m_playlist;
		Player::IPlayer& m_player;
		SongListVM^ m_songs;
	};

}
}

#endif