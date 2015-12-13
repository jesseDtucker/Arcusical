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
using namespace Windows::UI::Xaml::Data;

using namespace Arcusical::Events;
using namespace Arcusical::Model;
using namespace Arcusical::Player;
using namespace Util;

namespace Arcusical {
namespace ViewModel{

	SongListControlVM::SongListControlVM(Playlist& playlist, BackgroundWorker& worker)
		: m_playlist(playlist)
		, m_worker(worker)
	{
		
	}

	void SongListControlVM::PlaySongsAfterAndIncluding(SongVM^ song)
	{
		m_worker.Append([this, song]()
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

			if (distance(begin(songsToPlay), end(songsToPlay)) > 0)
			{
				m_playlist.Clear();
				m_playlist.Enqueue(songsToPlay);
			}
		});
	}

	void SongListControlVM::Shuffle()
	{
		if (SongList == nullptr)
		{
			return;
		}

		m_worker.Append([this]()
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

				m_playlist.Clear();
				m_playlist.Enqueue(songsToPlay);
			}
		});
	}
}
}