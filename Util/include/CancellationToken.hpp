#pragma once

#include <atomic>
#include <memory>

namespace Util {
class CancellationToken final {
 public:
  CancellationToken() = default;
  ~CancellationToken() = default;

  void Cancel();
  bool IsCanceled();

 private:
  std::atomic<bool> m_isCanceled = false;
};

typedef std::shared_ptr<CancellationToken> CancellationTokenRef;
}
