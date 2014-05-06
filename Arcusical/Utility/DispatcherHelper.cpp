#include "pch.h"

#include "DispatcherHelper.hpp"

namespace Arcusical
{
	void DispatchToUI(std::function<void()> func)
	{
		auto wnd = Windows::ApplicationModel::Core::CoreApplication::MainView->CoreWindow;

		ARC_ASSERT(wnd != nullptr);

		wnd->Dispatcher->RunAsync(
			Windows::UI::Core::CoreDispatcherPriority::Low, 
			ref new Windows::UI::Core::DispatchedHandler(func));
	}
}