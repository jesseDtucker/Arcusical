//
// Search.xaml.h
// Declaration of the Search class
//

#pragma once

#include "Controls\Search.g.h"
#include "ViewModels\SearchVM.hpp"

namespace Arcusical
{
namespace MusicProvider
{
	class MusicSearcher;
}
}

namespace Arcusical
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class Search sealed
	{
	public:
		Search();
	internal:
		void SetSearchProvider(MusicProvider::MusicSearcher* musicSearcher);
	private:
		MusicProvider::MusicSearcher* m_searcher;
		ViewModel::SearchVM^ m_vm;
		void TextBox_KeyDown(Platform::Object^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs^ e);
	};
}
