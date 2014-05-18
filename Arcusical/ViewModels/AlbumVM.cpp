#include "pch.h"

#include "Album.hpp"
#include "AlbumVM.hpp"

using namespace Windows::UI::Xaml::Data;

namespace Arcusical{
namespace ViewModel{
	
	AlbumVM::AlbumVM(std::shared_ptr<Model::Album> album)
		: m_album(album)
	{
		this->Artist = ref new Platform::String(album->GetArtist().c_str());
		this->Title = ref new Platform::String(album->GetTitle().c_str());
		// TODO::JT 
		// this->Songs
	}

}
}