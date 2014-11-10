#include "pch.h"

#include <future>

#include "DispatcherHelper.hpp"

namespace Arcusical
{
	std::future<void> DispatchToUI(std::function<void()> func)
	{
		auto promise = std::make_shared<std::promise<void>>();

		auto wrappedFunc = [func, promise]()
		{
			func();
			promise->set_value();
		};

		auto wnd = Windows::ApplicationModel::Core::CoreApplication::MainView->CoreWindow;
		ARC_ASSERT(wnd != nullptr);

		if (wnd->Dispatcher->HasThreadAccess)
		{
			wrappedFunc();
		}
		else
		{
			wnd->Dispatcher->RunAsync(
				Windows::UI::Core::CoreDispatcherPriority::Low,
				ref new Windows::UI::Core::DispatchedHandler(wrappedFunc));
		}

		return promise->get_future();
	}

	bool HasThreadAccess()
	{
		auto wnd = Windows::ApplicationModel::Core::CoreApplication::MainView->CoreWindow;
		ARC_ASSERT(wnd != nullptr);

		return wnd->Dispatcher->HasThreadAccess;
	}
}