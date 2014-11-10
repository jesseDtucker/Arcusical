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

			MusicProvider::AlbumsChangedCallback callback = [this](Model::AlbumCollection& albums){ this->AlbumCallback(albums); };
			m_albumSubscription = service->SubscribeAlbums(callback);
		}

		void AlbumListControlVM::AlbumCallback(Model::AlbumCollection& albums)
		{
			auto promise = Arcusical::DispatchToUI([this, &albums]()
			{
				AlbumList = ref new AlbumListVM(albums);
			});
			promise.get();
		}
	}
}