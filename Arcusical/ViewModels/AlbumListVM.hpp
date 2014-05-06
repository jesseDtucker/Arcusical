#ifndef ALBUM_LIST_VM_HPP
#define ALBUM_LIST_VM_HPP

#include "PropertyHelper.hpp"
#include "Utility/DispatcherHelper.hpp"
#include "Utility/XamlMacros.hpp"

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
		
	private:

	};
}
}

#endif