//
// SearchResultsControl.xaml.cpp
// Implementation of the SearchResultsControl class
//

#include "pch.h"
#include "SearchResultsControl.xaml.h"

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Media::Animation;
using namespace Windows::UI::Xaml::Navigation;

using namespace Arcusical;
using namespace Arcusical::ViewModel;

// The User Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234236

SearchResultsControl::SearchResultsControl() {
	InitializeComponent();
	m_animIn = safe_cast<Storyboard^>(v_root->Resources->Lookup("slideInAnim"));
	m_animOut = safe_cast<Storyboard^>(v_root->Resources->Lookup("slideOutAnim"));
}

SearchResultsVM^ SearchResultsControl::VM::get() {
	if(m_viewModel != this->DataContext) {
		m_viewModel = dynamic_cast<SearchResultsVM^>(this->DataContext);
	}
	return m_viewModel;
}
void SearchResultsControl::VM::set(SearchResultsVM^ vm) {
	ARC_ASSERT(Arcusical::HasThreadAccess());
	m_viewModel = vm;
	this->DataContext = m_viewModel;

	m_hasResultsSub = m_viewModel->OnHasResultsChanged += [this](auto hasResults) {
		if (hasResults) {
			ShowIfNeeded();
		}
		else {
			HideResults();
		}
	};
}

void Arcusical::SearchResultsControl::ShowResults() {
	if (!m_isShown) {
		m_isShown = true;
		m_animOut->Stop();
		m_animIn->Begin();
	}
}
void Arcusical::SearchResultsControl::HideResults() {
	if (m_isShown) {
		m_isShown = false;
		m_animIn->Stop();
		m_animOut->Begin();
	}
}

void Arcusical::SearchResultsControl::ShowIfNeeded() {
	auto albumCount = m_viewModel->Albums->Albums->Size;
	auto songCount = (m_viewModel->Songs->SongList != nullptr)
		? m_viewModel->Songs->SongList->List->Size
		: 0;
	if (albumCount > 0 || songCount > 0) {
		ShowResults();
	}
}

void Arcusical::SearchResultsControl::BackClicked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e) {
	HideResults();
}
