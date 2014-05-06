//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"

#include "Controls/SongListControl.xaml.h"
#include "Controls/AlbumListControl.xaml.h"
#include "ViewModels/AlbumListControlVM.hpp"

#include "IMusicProvider.hpp"

using namespace Arcusical;

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

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

MainPage::MainPage()
{
	using namespace ViewModel;

	InitializeComponent();

	auto musicProviderService = MusicProvider::MusicProviderLocator::ResolveService();

	this->v_songListControl->DataContext = ref new ViewModel::SongListControlViewModel(musicProviderService);
}
