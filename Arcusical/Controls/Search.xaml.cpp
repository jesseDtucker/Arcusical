//
// Search.xaml.cpp
// Implementation of the Search class
//

#include "pch.h"
#include "Search.xaml.h"

//#include "ViewModels/SearchVM.hpp"

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

using namespace Arcusical;
using namespace Arcusical::ViewModel;
using namespace Arcusical::MusicProvider;


Arcusical::Search::Search()
	: m_searcher(nullptr)
{
	InitializeComponent();
}

void Search::SetSearchProvider(MusicSearcher* musicSearcher)
{
	ARC_ASSERT(musicSearcher != nullptr);
	m_searcher = musicSearcher;
	m_vm = ref new SearchVM(*musicSearcher);
	this->DataContext = m_vm;
}

void Arcusical::Search::TextBox_KeyDown(Platform::Object^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs^ e)
{
	if (e->Key == Windows::System::VirtualKey::Enter)
	{
		m_vm->SelectCurrent();
	}
}
