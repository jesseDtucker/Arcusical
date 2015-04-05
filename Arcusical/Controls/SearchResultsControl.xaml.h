//
// SearchResultsControl.xaml.h
// Declaration of the SearchResultsControl class
//

#pragma once

#include "Controls\SearchResultsControl.g.h"
#include "ViewModels\SearchResultsVM.hpp"
#include "Utility/XamlMacros.hpp"

namespace Arcusical
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class SearchResultsControl sealed
	{
	public:
		SearchResultsControl();

		VM_DECLARATION(ViewModel::SearchResultsVM^);
	};
}
