#ifndef DISPATCHER_HELPER_HPP
#define DISPATCHER_HELPER_HPP

#include <functional>
#include <future>

namespace Arcusical 
{
	std::future<void> DispatchToUI(std::function<void()> func);
	bool HasThreadAccess();
}

#endif