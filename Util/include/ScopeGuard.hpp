#pragma once
#ifndef SCOPE_GUARD_HPP

namespace Util {
template <typename CallbackType>
class ScopeGuard {
 public:
  ScopeGuard(CallbackType callback);

  // cannot be copied or moved
  ScopeGuard(const ScopeGuard&) = delete;
  ScopeGuard(ScopeGuard&&) = delete;
  ScopeGuard& operator=(const ScopeGuard&) = delete;

  virtual ~ScopeGuard();

 private:
  CallbackType m_cleanupCall;
};

template <typename CallbackType>
ScopeGuard<CallbackType>::ScopeGuard(CallbackType callback)
    : m_cleanupCall(callback) {}

template <typename CallbackType>
ScopeGuard<CallbackType>::~ScopeGuard() {
  m_cleanupCall();
}
}

#endif