//
// SongListControl.xaml.h
// Declaration of the SongListControl class
//

#pragma once

#include "Controls/SongListControl.g.h"
#include "ViewModels/SongListControlVM.hpp"
#include "Utility/XamlMacros.hpp"

using namespace Windows::UI::Xaml;

namespace Arcusical
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class SongListControl sealed
	{
	public:
		SongListControl();

		VM(ViewModel::SongListControlVM^);
	private:

		void SongDoubleTapped(Platform::Object^ sender, Windows::UI::Xaml::Input::DoubleTappedRoutedEventArgs^ e);

		void Shuffle_Clicked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};
}
