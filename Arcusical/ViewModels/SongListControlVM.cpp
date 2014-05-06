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
		m_subscription = service->Subscribe(
			std::function<void(MusicProvider::SongListPtr, MusicProvider::SongListPtr)>(
		[this](MusicProvider::SongListPtr localSongs, MusicProvider::SongListPtr remoteSongs)
		{
			this->MusicCallback(localSongs, remoteSongs);
		}));
	}

	void SongListControlViewModel::MusicCallback(MusicProvider::SongListPtr localSongs, MusicProvider::SongListPtr remoteSongs)
	{
		auto songsPtr = localSongs.lock();
		ARC_ASSERT(songsPtr != nullptr);
		if (songsPtr != nullptr)
		{
			this->SongList = ref new SongListVM(songsPtr);
		}
	}

}
}