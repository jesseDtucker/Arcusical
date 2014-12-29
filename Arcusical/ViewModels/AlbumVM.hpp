#ifndef ALBUM_VM_HPP
#define ALBUM_VM_HPP

#include <memory>

#include "PropertyHelper.hpp"
#include "ViewModels/SongListVM.hpp"
#include "Utility/DispatcherHelper.hpp"
#include "Utility/XamlMacros.hpp"

namespace Arcusical{ namespace Model { class Album; } }

namespace Arcusical {
namespace ViewModel{

	[Windows::UI::Xaml::Data::Bindable]
	public ref class AlbumVM sealed : INotifyPropertyChanged_t
	{
	public:
		NOTIFY_PROPERTY_CHANGED_IMPL;

		PROP_SET_AND_GET_WINRT(Platform::String^, Title);
		PROP_SET_AND_GET_WINRT(Platform::String^, Artist);
		PROP_SET_AND_GET_WINRT(SongListVM^, Songs);
		PROP_SET_AND_GET_WINRT(Platform::String^, ImagePath);

		void Play();
		
	internal:
		AlbumVM(const Model::Album& album);

	private:
		Model::Album m_album;
	};

}
}

#endif