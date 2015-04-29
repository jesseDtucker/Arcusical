//
// TopBar.xaml.cpp
// Implementation of the TopBar class
//

#include "pch.h"
#include "BottomBar.xaml.h"

using namespace Arcusical;
using namespace Arcusical::ViewModel;

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


VM_IMPL(SongPlayerVM^, BottomBar);

BottomBar::BottomBar()
{
	InitializeComponent();
}
