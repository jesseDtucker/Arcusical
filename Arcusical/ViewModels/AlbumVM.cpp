#include "pch.h"

#include <future>

#include "Album.hpp"
#include "AlbumVM.hpp"
#include "Playlist.hpp"

using namespace Windows::UI::Xaml::Data;

using namespace std;
using namespace::Arcusical::Player;

namespace Arcusical{
namespace ViewModel{
	
	AlbumVM::AlbumVM(const Model::Album& album)
		: m_album(album)
	{
		this->Artist = ref new Platform::String(album.GetArtist().c_str());
		this->Title = ref new Platform::String(album.GetTitle().c_str());
		this->ImagePath = ref new Platform::String(album.GetImageFilePath().c_str());

		// getting the songs could block, but this ctor must return immediately, so get songs will be async
		std::async([this, album]()
		{
			auto songs = *album.GetSongs();
			DispatchToUI([this, songs]()
			{
				this->Songs = ref new SongListVM(songs);
			});
		});
	}

	void AlbumVM::Play()
	{
		std::async([this]()
		{
			auto playlist = PlaylistLocator::ResolveService().lock();
			ARC_ASSERT(playlist != nullptr);
			if (playlist != nullptr)
			{
				playlist->Clear();
				playlist->Enqueue(*m_album.GetSongs());
			}
		});
	}

} /*ViewModel*/
} /*Arcusical*/