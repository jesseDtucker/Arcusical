#pragma once
#ifndef WORK_BUFFER_HPP

#include "boost/optional.hpp"
#include <chrono>
#include <deque>
#include <functional>
#include <future>
#include <iterator>

#include "Arc_Assert.hpp"
#include "MulticastDelegate.hpp"
#include "ScopeGuard.hpp"

#undef max
#undef min

#define WORK_BUFFER_LOCK                     \
  m_pending++;                               \
  std::unique_lock<std::mutex> lock(m_lock); \
  Util::ScopeGuard<std::function<void()>> counter([this]() { m_pending--; });

namespace Util {
template <typename T>
class InputBuffer {
 public:
  virtual void Append(const T& value) = 0;
  virtual void Append(T&& value) = 0;
  virtual void Append(std::vector<T>&& values) = 0;
  virtual void Complete() = 0;
  virtual void Reset() = 0;
};

template <typename T>
class OutputBuffer {
 public:
  virtual bool ResultsPending() const = 0;
  virtual bool ResultsAvailable() const = 0;
  virtual void WaitForCompletion() const = 0;
  virtual std::vector<T> GetAll() = 0;
  virtual std::vector<T> GetMultiple(size_t minimum, size_t maximum,
                                     boost::optional<std::chrono::milliseconds> = boost::none) = 0;
  virtual std::vector<T> GetN(size_t count, boost::optional<std::chrono::milliseconds> = boost::none) = 0;
  virtual std::vector<T> GetAtLeast(size_t count, boost::optional<std::chrono::milliseconds> = boost::none) = 0;
  virtual std::vector<T> GetAtMost(size_t count, boost::optional<std::chrono::milliseconds> = boost::none) = 0;
  virtual boost::optional<T> GetNext(boost::optional<std::chrono::milliseconds> = boost::none) = 0;
  virtual boost::optional<T> GetNextNoWait() = 0;

  Util::MulticastDelegate<void()> ItemAvailable;
};

/************************************************************************/
/* Simple produce-consumer queue                                        */
/************************************************************************/
template <typename T>
class WorkBuffer final : public InputBuffer<T>, public OutputBuffer<T> {
 public:
  WorkBuffer();
  WorkBuffer(const WorkBuffer& rhs) = delete;
  WorkBuffer(WorkBuffer&& rhs);
  WorkBuffer& operator=(const WorkBuffer&) = delete;
  WorkBuffer& operator=(WorkBuffer&&);
  ~WorkBuffer();

  void DropAll();
  void RemoveIf(std::function<bool(const T&)>);

  // producer functions
  void Append(const T& value) override;
  void Append(T&& value) override;
  void Append(std::vector<T>&& values) override;
  void Complete() override;
  void Reset() override;

  // consumer functions
  // Note: can return less than asked for if there isn't enough available and no more
  // work is expected
  bool ResultsPending() const override;
  bool ResultsAvailable() const override;
  void WaitForCompletion() const override;
  std::vector<T> GetAll() override;
  std::vector<T> GetMultiple(size_t minimum, size_t maximum,
                             boost::optional<std::chrono::milliseconds> = boost::none) override;
  std::vector<T> GetN(size_t count, boost::optional<std::chrono::milliseconds> = boost::none) override;
  std::vector<T> GetAtLeast(size_t count, boost::optional<std::chrono::milliseconds> = boost::none) override;
  std::vector<T> GetAtMost(size_t count, boost::optional<std::chrono::milliseconds> = boost::none) override;
  boost::optional<T> GetNext(boost::optional<std::chrono::milliseconds> timeout = boost::none) override;
  boost::optional<T> GetNextNoWait() override;

 private:
  mutable std::condition_variable m_consumerCP;
  mutable std::mutex m_lock;
  std::deque<T> m_buffer;
  bool m_isDone = false;
  mutable std::atomic<int> m_pending;
};

/************************************************************************/
/* Implementation                                                       */
/************************************************************************/

template <typename T>
WorkBuffer<T>::WorkBuffer() {
  m_pending = 0;
}

template <typename T>
WorkBuffer<T>::WorkBuffer(WorkBuffer&& rhs) {
  // make sure to lock the other one first
  std::lock_guard<std::mutex> lock(rhs.m_lock);

  m_buffer = std::move(rhs.m_buffer);
  rhs.m_buffer = {};
  m_isDone = rhs.m_isDone;

  // we have no pending in the new buffer as nothing could have called it yet
  m_pending = 0;

  // mutex, and condition variable are not moved

  // we need to notify the other work buffer to make sure nothing gets stuck waiting for non-existent results
  rhs.m_consumerCP.notify_all();
}

template <typename T>
WorkBuffer<T>& WorkBuffer<T>::operator=(WorkBuffer&& rhs) {
  // make sure to lock both buffers!
  std::lock_guard<std::mutex> lock(m_lock);
  std::lock_guard<std::mutex> otherLock(rhs.m_lock);

  m_buffer = std::move(rhs.m_buffer);
  m_isDone = rhs.m_isDone;
  rhs.m_isDone = true;  // rhs is now garbage. It cannot be used

  // we have no pending in the new buffer as nothing could have called it yet
  m_pending = 0;

  // mutex, and condition variable are not moved

  // we need to notify both work buffers to make sure nothing gets stuck waiting for non-existent results
  rhs.m_consumerCP.notify_all();
  m_consumerCP.notify_all();

  return *this;
}

template <typename T>
WorkBuffer<T>::~WorkBuffer() {
  bool isWorkDone = false;
  {
    std::unique_lock<std::mutex> lock(m_lock);
    isWorkDone = m_isDone || m_buffer.size() == 0;
  }
  ARC_ASSERT_MSG(isWorkDone, "Attempted to destroy a work buffer that has pending work!");
  // the following code will try to correct against a premature deletion. However, this really shouldn't be allowed to
  // happen
  if (!isWorkDone) {
    Complete();
  }

  ARC_ASSERT_MSG(
      m_pending == 0,
      "Warning, there is very likely a lifetime error as a WorkBuffer is being destroyed while something is using it!");

  while (m_pending != 0) {
    // just spin, this should never happen but nonetheless we must be safe and detect the error, report it (loudly) and
    // then handle it gracefully
    // However, this will only protect against code that is currently using this object, not code that attempts access
    // after it has been destroyed
    // frankly there is little I can do to protect against a dangling pointer from in here...
  }

  ARC_ASSERT(m_pending == 0);
}

template <typename T>
void WorkBuffer<T>::DropAll() {
  WORK_BUFFER_LOCK;
  m_buffer.clear();
  m_consumerCP.notify_all();
}

template <typename T>
void Util::WorkBuffer<T>::RemoveIf(std::function<bool(const T&)> pred) {
  WORK_BUFFER_LOCK;
  auto newEnd = std::remove_if(std::begin(m_buffer), std::end(m_buffer), pred);
  m_buffer.erase(newEnd, std::end(m_buffer));
}

template <typename T>
bool WorkBuffer<T>::ResultsPending() const {
  WORK_BUFFER_LOCK;
  return !m_isDone || m_buffer.size() > 0;
}

template <typename T>
bool WorkBuffer<T>::ResultsAvailable() const {
  WORK_BUFFER_LOCK;
  return m_buffer.size() > 0;
}

template <typename T>
void Util::WorkBuffer<T>::WaitForCompletion() const {
  WORK_BUFFER_LOCK;

  if (!m_isDone) {
    m_consumerCP.wait(lock, [this]() { return m_isDone; });
  }
}

template <typename T>
void WorkBuffer<T>::Append(const T& value) {
  {
    WORK_BUFFER_LOCK;
    ARC_ASSERT(!m_isDone);
    m_buffer.push_back(value);
  }

  ItemAvailable();
  m_consumerCP.notify_all();
}

template <typename T>
void WorkBuffer<T>::Append(T&& value) {
  {
    WORK_BUFFER_LOCK;
    ARC_ASSERT(!m_isDone);
    m_buffer.push_back(std::move(value));
  }

  ItemAvailable();
  m_consumerCP.notify_all();
}

template <typename T>
void WorkBuffer<T>::Append(std::vector<T>&& values) {
  WORK_BUFFER_LOCK;
  ARC_ASSERT(!m_isDone);
  std::move(begin(values), end(values), std::back_inserter(m_buffer));
  m_consumerCP.notify_all();
}

template <typename T>
void WorkBuffer<T>::Complete() {
  WORK_BUFFER_LOCK;

  m_isDone = true;
  m_consumerCP.notify_all();
}

template <typename T>
void Util::WorkBuffer<T>::Reset() {
  WORK_BUFFER_LOCK;
  m_isDone = false;
}

template <typename T>
std::vector<T> WorkBuffer<T>::GetAll() {
  WaitForCompletion();

  WORK_BUFFER_LOCK;
  std::vector<T> results;
  results.reserve(m_buffer.size());
  std::move(begin(m_buffer), end(m_buffer), std::back_inserter(results));
  m_buffer = {};
  return results;
}

template <typename T>
std::vector<T> WorkBuffer<T>::GetMultiple(size_t minimum, size_t maximum,
                                          boost::optional<std::chrono::milliseconds> timeout) {
  using namespace std::chrono;

  ARC_ASSERT(minimum >= 0 && maximum > 0);
  ARC_ASSERT(minimum <= maximum);

  auto start = system_clock::now();
  WORK_BUFFER_LOCK;

  if (m_buffer.size() < minimum && !m_isDone) {
    if (timeout) {
      while (start + *timeout >= system_clock::now() && !m_isDone && (m_buffer.size() < minimum)) {
        m_consumerCP.wait(lock);
      }
    } else {
      m_consumerCP.wait(lock, [this, minimum]() { return m_buffer.size() >= minimum || m_isDone; });
    }
  }

  auto numToGet = m_buffer.size();
  numToGet = std::min(numToGet, maximum);
  if (numToGet > 0) {
    std::vector<T> results;
    results.reserve(numToGet);

    auto last = std::next(begin(m_buffer), numToGet);
    std::move(begin(m_buffer), last, std::back_inserter(results));
    m_buffer.erase(begin(m_buffer), last);

    return results;
  } else {
    return {};
  }
}

template <typename T>
std::vector<T> WorkBuffer<T>::GetN(size_t count, boost::optional<std::chrono::milliseconds> timeout) {
  ARC_ASSERT(count > 0);
  return GetMultiple(count, count, timeout);
}

template <typename T>
std::vector<T> WorkBuffer<T>::GetAtLeast(size_t count, boost::optional<std::chrono::milliseconds> timeout) {
  ARC_ASSERT(count >= 0);
  return GetMultiple(count, std::numeric_limits<size_t>::max(), timeout);
}

template <typename T>
std::vector<T> WorkBuffer<T>::GetAtMost(size_t count, boost::optional<std::chrono::milliseconds> timeout) {
  ARC_ASSERT(count > 0);
  return GetMultiple(1, count, timeout);
}

template <typename T>
boost::optional<T> WorkBuffer<T>::GetNext(boost::optional<std::chrono::milliseconds> timeout /*= boost::none*/) {
  using namespace std::chrono;

  boost::optional<T> result = boost::none;
  auto start = system_clock::now();
  WORK_BUFFER_LOCK;

  if (m_buffer.size() < 1 || !m_isDone) {
    if (timeout) {
      while (start + *timeout >= system_clock::now() && !m_isDone && (m_buffer.size() < 1)) {
        m_consumerCP.wait(lock);
      }
    } else {
      m_consumerCP.wait(lock, [this]() { return m_buffer.size() >= 1 || m_isDone; });
    }
  }

  if (m_buffer.size() > 0) {
    result = m_buffer.front();
    m_buffer.pop_front();
  }

  return result;
}

template <typename T>
boost::optional<T> WorkBuffer<T>::GetNextNoWait() {
  boost::optional<T> result = boost::none;
  WORK_BUFFER_LOCK;

  if (m_buffer.size() > 0) {
    result = m_buffer.front();
    m_buffer.pop_front();
  }

  return result;
}
}

#endif