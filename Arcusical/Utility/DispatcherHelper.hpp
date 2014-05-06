#ifndef DISPATCHER_HELPER_HPP
#define DISPATCHER_HELPER_HPP

#include <functional>

namespace Arcusical 
{
	void DispatchToUI(std::function<void()> func);
}

#endif