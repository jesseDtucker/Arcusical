#include "pch.h"

#include <algorithm>
#include <random>
#include <vector>

#include "SongListControlVM.hpp"

#include "Arc_Assert.hpp"
#include "Events/AlbumSelectedEvent.hpp"
#include "Events/EventService.hpp"
#include "Playlist.hpp"
#include "SongListVM.hpp"

using namespace std;
using namespace Arcusical::Model;
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

	void SongListControlVM::PlaySongsAfterAndIncluding(SongVM^ song)
	{
		async([this, song]()
		{
			auto itr = SongList->List->First();
			vector<Song> songsToPlay;
			bool hasFoundCurrent = false;
			while (itr->HasCurrent)
			{
				if (!hasFoundCurrent && song->Title->Equals(itr->Current->Title))
				{
					hasFoundCurrent = true;
				}
				if (hasFoundCurrent)
				{
					songsToPlay.push_back(*itr->Current->GetModel());
				}
				itr->MoveNext();
			}

			auto playlist = Player::PlaylistLocator::ResolveService().lock();
			ARC_ASSERT(playlist != nullptr);

			if (distance(begin(songsToPlay), end(songsToPlay)) > 0)
			{
				playlist->Clear();
				playlist->Enqueue(songsToPlay);
			}
		});
	}

	void SongListControlVM::Shuffle()
	{
		if (SongList == nullptr)
		{
			return;
		}

		std::async([this]()
		{
			auto itr = SongList->List->First();
			vector<Song> songsToPlay;
			while (itr->HasCurrent)
			{
				songsToPlay.push_back(*itr->Current->GetModel());
				itr->MoveNext();
			}

			if (songsToPlay.size() > 0)
			{
				random_device rd;
				shuffle(begin(songsToPlay), end(songsToPlay), default_random_engine(rd()));

				auto playlist = Player::PlaylistLocator::ResolveService().lock();
				ARC_ASSERT(playlist != nullptr);

				playlist->Clear();
				playlist->Enqueue(songsToPlay);
			}
		});
		
	}

}
}