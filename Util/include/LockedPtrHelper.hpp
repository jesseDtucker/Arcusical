#pragma once
#ifndef LOCKED_PTR_HELPER_HPP
#define LOCKED_PTR_HELPER_HPP

#include <functional>
#include <memory>
#include <windows.h>

#include "LockHelper.hpp"

template <typename T>
std::unique_ptr<T, std::function<void(T*)>> CreateReadLockedPointer(Util::SlimRWLock* lock, T* ptr) {
  auto currentThreadId = std::this_thread::get_id();
  lock->LockShared();
  std::function<void(T*)> deleter = [lock, currentThreadId](T* p) {
    ARC_ASSERT(currentThreadId == std::this_thread::get_id());
    lock->UnlockShared();
  };
  return std::unique_ptr<T, decltype(deleter)>(ptr, deleter);

  return nullptr;
}

#endif