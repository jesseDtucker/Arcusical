#pragma once
#ifndef LOCKED_PTR_HELPER_HPP
#define LOCKED_PTR_HELPER_HPP

#include <memory>

template<typename T>
std::shared_ptr<T> CreateLockedPointer(std::recursive_mutex& mutex, T* ptr)
{
	auto currentThreadId = std::this_thread::get_id();
	mutex.lock();
	return std::shared_ptr<T>(ptr, [&mutex, currentThreadId](T* p)
	{
		ARC_ASSERT(currentThreadId == std::this_thread::get_id())
		mutex.unlock();
	});
}

#endif