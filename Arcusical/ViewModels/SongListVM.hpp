#ifndef SONG_LIST_VM_HPP
#define SONG_LIST_VM_HPP

#include <collection.h>
#include "boost\uuid\uuid.hpp"
#include <unordered_map>

#include "IMusicProvider.hpp"
#include "ViewModels/SongVM.hpp"
#include "Utility/XamlMacros.hpp"

namespace Arcusical {
namespace Model
{
	class Song;
}
namespace ViewModel
{
	[Windows::UI::Xaml::Data::Bindable]
	public ref class SongListVM sealed : INotifyPropertyChanged_t
	{
	public:
		NOTIFY_PROPERTY_CHANGED_IMPL;

		PROP_SET_AND_GET_WINRT(Windows::Foundation::Collections::IVector<ViewModel::SongVM^>^, List);

	internal:
		SongListVM(Model::SongCollection& songs);
	private:
		
	};

}
}

#endif