#pragma once
#ifndef LOCKED_PTR_HELPER_HPP
#define LOCKED_PTR_HELPER_HPP

#include <functional>
#include <memory>

template<typename T>
std::unique_ptr<T, std::function<void(T*)>> CreateLockedPointer(std::recursive_mutex& mutex, T* ptr)
{
	auto currentThreadId = std::this_thread::get_id();
	mutex.lock();
	std::function<void(T*)> deleter = [&mutex, currentThreadId](T* p)
	{
		ARC_ASSERT(currentThreadId == std::this_thread::get_id())
			mutex.unlock();
	};
	return std::unique_ptr<T, decltype(deleter)>(ptr, deleter);
}

#endif