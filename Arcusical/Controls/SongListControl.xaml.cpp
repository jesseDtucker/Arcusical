//
// SongListControl.xaml.cpp
// Implementation of the SongListControl class
//

#include "pch.h"

#include "SongListControl.xaml.h"
#include "ViewModels/SongVM.hpp"

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
// The User Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234236

using namespace ViewModel;

SongListControl::SongListControl()
	:m_vm(ref new SongListControlVM())
{
	InitializeComponent();
	DataContext = m_vm;
}

void Arcusical::SongListControl::SongDoubleTapped(Platform::Object^ sender, Windows::UI::Xaml::Input::DoubleTappedRoutedEventArgs^ e)
{
	auto uiElement = dynamic_cast<FrameworkElement^>(sender);
	ARC_ASSERT(uiElement != nullptr);
	if (uiElement != nullptr)
	{
		auto song = dynamic_cast<SongVM^>(uiElement->DataContext);
		ARC_ASSERT(song != nullptr);
		if (song != nullptr)
		{
			m_vm->PlaySongsAfterAndIncluding(song);
		}
	}
}
