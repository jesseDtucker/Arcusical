//
// AlbumListControl.xaml.h
// Declaration of the AlbumListControl class
//
#pragma once

#include "Controls/AlbumListControl.g.h"

namespace Arcusical
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class AlbumListControl sealed
	{
	public:
		AlbumListControl();
		
	private:
		void AlbumClicked(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e);
		void Album_DoubleTapped(Platform::Object^ sender, Windows::UI::Xaml::Input::DoubleTappedRoutedEventArgs^ e);

		ViewModel::AlbumListControlVM^ m_vm;
	};
}
