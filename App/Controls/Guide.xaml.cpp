//
// Guide.xaml.cpp
// Implementation of the Guide class
//

#include "pch.h"
#include "Guide.xaml.h"

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

// The User Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234236

VM_IMPL(GuideVM^, Guide);

Guide::Guide()
{
	InitializeComponent();
}
