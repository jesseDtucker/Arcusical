//
// SongListControl.xaml.h
// Declaration of the SongListControl class
//

#pragma once

#include "Controls/SongListControl.g.h"

#include "ViewModels/SongListVM.hpp"

using namespace Windows::UI::Xaml;

namespace Arcusical
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class SongListControl sealed
	{
	public:
		SongListControl();
	private:
		static DependencyProperty^ d_songs;

		void SongClicked(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e);
	};
}
