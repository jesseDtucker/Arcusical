//
// AlbumListControl.xaml.h
// Declaration of the AlbumListControl class
//
#pragma once

#include "Controls/AlbumListControl.g.h"
#include "ViewModels/AlbumListControlVM.hpp"
#include "Utility\XamlMacros.hpp"

namespace Arcusical
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class AlbumListControl sealed
	{
	public:
		AlbumListControl();

		VM_DECLARATION(ViewModel::AlbumListControlVM^);
	private:
		void AlbumClicked(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e);
		void Album_DoubleTapped(Platform::Object^ sender, Windows::UI::Xaml::Input::DoubleTappedRoutedEventArgs^ e);
	};
}
