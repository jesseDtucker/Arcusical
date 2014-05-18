#include "pch.h"

#include "AlbumListControlVM.hpp"

namespace Arcusical
{
	namespace ViewModel
	{
		AlbumListControlVM::AlbumListControlVM(MusicProvider::MusicProviderLocator::ServiceRef providerService)
			: m_providerService(providerService)
		{
			AlbumList = nullptr;
			auto service = m_providerService.lock();

			std::function<void(MusicProvider::AlbumListPtr)> callback = [this](MusicProvider::AlbumListPtr albums){ this->AlbumCallback(albums); };
			m_albumSubscription = service->SubscribeAlbums(callback);
		}

		void AlbumListControlVM::AlbumCallback(MusicProvider::AlbumListPtr albums)
		{
			AlbumList = ref new AlbumListVM(albums);
		}
	}
}