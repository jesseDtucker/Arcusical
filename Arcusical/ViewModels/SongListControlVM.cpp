#include "pch.h"

#include "SongListControlVM.hpp"

#include "Arc_Assert.hpp"
#include "Events/AlbumSelectedEvent.hpp"
#include "Events/EventService.hpp"
#include "SongListVM.hpp"

using namespace Windows::UI::Xaml::Data;
using namespace Arcusical::Events;

namespace Arcusical {
namespace ViewModel{

	SongListControlVM::SongListControlVM()
	{
		std::function<void(const Events::AlbumSelectedEvent&)> callback = [this](const Events::AlbumSelectedEvent& event)
		{
			this->OnAlbumSelected(event);
		};
		m_albumSelectedSubscription = EventService<AlbumSelectedEvent>::RegisterListener(callback);
	}

	void SongListControlVM::OnAlbumSelected(const Events::AlbumSelectedEvent& event)
	{
		auto album = event.GetSelectedAlbum();
		this->SongList = album->Songs;
	}
}
}