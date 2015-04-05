#pragma once

#ifndef ALBUM_LIST_CONTROL_VM_HPP
#define ALBUM_LIST_CONTROL_VM_HPP

#include "PropertyHelper.hpp"
#include "Utility/XamlMacros.hpp"
#include "ViewModels/AlbumListVM.hpp"

namespace Arcusical {
namespace ViewModel{

	[Windows::UI::Xaml::Data::Bindable]
	public ref class AlbumListControlVM sealed : INotifyPropertyChanged_t
	{
	public:
		NOTIFY_PROPERTY_CHANGED_IMPL;

		PROP_SET_AND_GET_WINRT(AlbumListVM^, AlbumList);
	internal:

		AlbumListControlVM();

	private:

	};

}
}

#endif