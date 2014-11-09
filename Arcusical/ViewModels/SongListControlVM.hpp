#ifndef SONG_LIST_CONTROL_VM_HPP
#define SONG_LIST_CONTROL_VM_HPP

#include "IMusicProvider.hpp"
#include "ViewModels/SongListVM.hpp"
#include "Utility/XamlMacros.hpp"

namespace Arcusical{
namespace ViewModel{

	[Windows::UI::Xaml::Data::Bindable]
	public ref class SongListControlViewModel sealed : INotifyPropertyChanged_t
	{
	public:
		NOTIFY_PROPERTY_CHANGED_IMPL;

		PROP_SET_AND_GET_WINRT(SongListVM^, SongList);

	internal:
		SongListControlViewModel(MusicProvider::MusicProviderLocator::ServiceRef providerService);
	private:

		void MusicCallback(Model::SongCollection& localSongs);

		MusicProvider::MusicProviderLocator::ServiceRef m_providerService;
		MusicProvider::MusicProviderSubscription m_subscription;
	};

}
}

#endif