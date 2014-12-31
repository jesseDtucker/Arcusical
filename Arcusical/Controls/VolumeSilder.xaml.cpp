//
// VolumeSilder.xaml.cpp
// Implementation of the VolumeSilder class
//

#include "pch.h"
#include "VolumeSilder.xaml.h"

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

using namespace Arcusical::ViewModel;

VolumeSilder::VolumeSilder()
	: m_vm(ref new VolumeSliderVM())
{
	InitializeComponent();
	this->DataContext = m_vm;
}