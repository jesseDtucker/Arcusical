#include "pch.h"

#include "AlbumVM.hpp"

using namespace Windows::UI::Xaml::Data;

namespace Arcusical{
namespace ViewModel{
	
	AlbumVM::AlbumVM(std::shared_ptr<Model::Album> album)
		: m_album(album)
	{

	}

}
}