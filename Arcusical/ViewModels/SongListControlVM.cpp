#include "pch.h"

#include "SongListControlVM.hpp"

#include "Arc_Assert.hpp"
#include "SongListVM.hpp"

using namespace Windows::UI::Xaml::Data;

namespace Arcusical {
namespace ViewModel{

	SongListControlViewModel::SongListControlViewModel(
		MusicProvider::MusicProviderLocator::ServiceRef providerService)
		: m_providerService(providerService)
	{
		auto service = providerService.lock();
		ARC_ASSERT(service != nullptr);
		m_subscription = service->SubscribeSongs(MusicProvider::SongsChangedCallback([this](Model::SongCollection& localSongs)
		{
			this->MusicCallback(localSongs);
		}));
	}

	void SongListControlViewModel::MusicCallback(Model::SongCollection& localSongs)
	{
		this->SongList = ref new SongListVM(localSongs);
	}

}
}