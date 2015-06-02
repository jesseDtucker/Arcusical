#pragma once

#ifndef ALBUM_LIST_CONTROL_VM_HPP
#define ALBUM_LIST_CONTROL_VM_HPP

#include "PropertyHelper.hpp"
#include "Utility/XamlMacros.hpp"

namespace Arcusical {
namespace ViewModel{

	typedef Windows::Foundation::Collections::IObservableVector<AlbumVM^> AlbumList;

	[Windows::UI::Xaml::Data::Bindable]
	public ref class AlbumListControlVM sealed : INotifyPropertyChanged_t
	{
	public:
		NOTIFY_PROPERTY_CHANGED_IMPL;

		PROP_SET_AND_GET_WINRT(AlbumList^, Albums);
	internal:

		AlbumListControlVM();

		static AlbumList^
			CreateAlbumList(const Model::AlbumPtrCollection& albums,
			Player::Playlist& playlist,
			Player::IPlayer& player);

		static AlbumList^
			CreateAlbumList(const std::vector<Model::Album>& albums,
			Player::Playlist& playlist,
			Player::IPlayer& player);

		static AlbumList^
			CreateAlbumList(const Model::AlbumCollection& albums,
			Player::Playlist& playlist,
			Player::IPlayer& player);

	private:

	};

}
}

#endif