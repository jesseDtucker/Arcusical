#include "pch.h"

#include "Album.hpp"
#include "AlbumVM.hpp"

using namespace Windows::UI::Xaml::Data;

namespace Arcusical{
namespace ViewModel{
	
	AlbumVM::AlbumVM(const Model::Album& album)
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

} /*ViewModel*/
} /*Arcusical*/