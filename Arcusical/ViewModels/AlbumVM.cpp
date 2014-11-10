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
		// TODO::JT
		// this->Songs
	}

} /*ViewModel*/
} /*Arcusical*/