#pragma once
#ifndef ASYNC_PROCESSOR_HPP
#define ASYNC_PROCESSOR_HPP

#include <algorithm>
#include <functional>
#include <future>

#include "WorkBuffer.hpp"

namespace Util {
template <typename Input, typename Output>
class AsyncProcessor final : public InputBuffer<Input> {
 public:
  typedef std::function<Output(const Input&)> ProcessSingle;
  typedef std::function<std::vector<Output>(const std::vector<Input>&)> ProcessBatch;

  // processes input in batches
  AsyncProcessor(int minBatchSize = 20, int maxBatchSize = 100,
                 boost::optional<std::chrono::milliseconds> batchTimeout = boost::none);

  AsyncProcessor(const AsyncProcessor&) = delete;
  AsyncProcessor(AsyncProcessor&&);
  AsyncProcessor& operator=(const AsyncProcessor&) = delete;
  AsyncProcessor& operator=(AsyncProcessor&&) = delete;
  ~AsyncProcessor();

  void Start();
  void Stop();
  void DropAllWork();
  void AwaitCompletion();

  void Connect(Util::InputBuffer<Output>* outputBuffer);
  void SetProcessor(ProcessSingle func);
  void SetBatchProcessor(ProcessBatch func);

  bool IsRunning();

  void Append(const Input& value) override;
  void Append(Input&& value) override;
  void Append(std::vector<Input>&& values) override;
  void Complete() override;
  void Reset() override;

 private:
  Util::WorkBuffer<Input> m_inputBuffer;
  Util::InputBuffer<Output>* m_outputBuffer;
  std::function<void()> m_processorFunction;
  std::future<void> m_workFuture;

  std::atomic<bool> m_isRunning;
  std::atomic<bool> m_isStopping;
  int m_minBatchSize = 0;
  int m_maxBatchSize = 0;
  boost::optional<std::chrono::milliseconds> m_timeout = boost::none;
};

template <typename Input, typename Output>
AsyncProcessor<Input, Output>::AsyncProcessor(int minBatchSize /*= 20*/, int maxBatchSize /*= 100*/,
                                              boost::optional<std::chrono::milliseconds> batchTimeout /*= boost::none*/)
    : m_minBatchSize(minBatchSize),
      m_maxBatchSize(maxBatchSize),
      m_timeout(batchTimeout),
      m_isRunning(false),
      m_outputBuffer(nullptr) {
  ARC_ASSERT(minBatchSize <= maxBatchSize);
  ARC_ASSERT(minBatchSize > 0 && maxBatchSize > 0);
}

template <typename Input, typename Output>
Util::AsyncProcessor<Input, Output>::AsyncProcessor(AsyncProcessor<Input, Output>&& rhs) {
  ARC_ASSERT_MSG(!rhs.m_isRunning, "Moving is untested with active processors!");

  m_inputBuffer = std::move(rhs.m_inputBuffer);
  m_outputBuffer = std::move(rhs.m_outputBuffer);

  // just copy this, it's cheap enough
  m_processorFunction = rhs.m_processorFunction;
  m_isRunning = false;

  m_minBatchSize = rhs.m_minBatchSize;
  m_maxBatchSize = rhs.m_maxBatchSize;
  m_timeout = rhs.m_timeout;
}

template <typename Input, typename Output>
Util::AsyncProcessor<Input, Output>::~AsyncProcessor() {
  try {
    Stop();
  }
  catch (Concurrency::invalid_operation& ex) {
    OutputDebugStringA(ex.what());
    ARC_FAIL(
        "Likely attempted to destroy an async processor on the UI thread, behaviour beyond this point is "
        "unpredictable");
  }
}

template <typename Input, typename Output>
void Util::AsyncProcessor<Input, Output>::SetProcessor(ProcessSingle func) {
  ARC_ASSERT(!m_isRunning);

  m_processorFunction = [this, func]() {
    while (m_inputBuffer.ResultsPending() && !m_isStopping) {
      auto input = m_inputBuffer.GetNext(m_timeout);
      if (input) {
        auto output = func(*input);
        m_outputBuffer->Append(output);
      }
    }

    if (!m_inputBuffer.ResultsPending()) {
      m_outputBuffer->Complete();
    }
  };
}

template <typename Input, typename Output>
void Util::AsyncProcessor<Input, Output>::SetBatchProcessor(ProcessBatch func) {
  ARC_ASSERT(!m_isRunning);

  m_processorFunction = [this, func]() {
    while (m_inputBuffer.ResultsPending() && !m_isStopping) {
      auto input = m_inputBuffer.GetMultiple(m_minBatchSize, m_maxBatchSize, m_timeout);
      if (input.size() > 0) {
        auto output = func(input);
        m_outputBuffer->Append(std::move(output));
      }
    }

    if (!m_inputBuffer.ResultsPending()) {
      m_outputBuffer->Complete();
    }
  };
}

template <typename Input, typename Output>
void Util::AsyncProcessor<Input, Output>::Connect(Util::InputBuffer<Output>* outputBuffer) {
  ARC_ASSERT(outputBuffer != nullptr);
  m_outputBuffer = outputBuffer;
}

template <>
inline void AsyncProcessor<std::function<void()>, void>::Start() {
  ARC_ASSERT(!m_isRunning);

  m_processorFunction = [this]() {
    while (m_inputBuffer.ResultsPending() && !m_isStopping) {
      auto input = m_inputBuffer.GetNext(m_timeout);
      if (input) {
        (*input)();
      }
    }
  };

  m_inputBuffer.Reset();
  m_isRunning = true;
  m_workFuture = std::async(std::launch::async, m_processorFunction);
}

template <typename Input, typename Output>
void AsyncProcessor<Input, Output>::Start() {
  ARC_ASSERT(!m_isRunning);
  ARC_ASSERT(m_processorFunction != nullptr);
  m_inputBuffer.Reset();
  m_outputBuffer->Reset();
  m_isRunning = true;
  m_workFuture = std::async(std::launch::async, m_processorFunction);
}

template <typename Input, typename Output>
void Util::AsyncProcessor<Input, Output>::Stop() {
  m_isStopping = true;
  m_inputBuffer.Complete();
  if (m_workFuture.valid()) {
    m_workFuture.wait();
  }
  m_isStopping = false;
  m_isRunning = false;
}

template <typename Input, typename Output>
void AsyncProcessor<Input, Output>::DropAllWork() {
  auto wasRunning = m_isRunning.load();
  if (wasRunning) {
    Stop();
  }
  m_inputBuffer.DropAll();
  if (wasRunning) {
    Start();
  }
}

template <typename Input, typename Output>
void AsyncProcessor<Input, Output>::AwaitCompletion() {
  m_inputBuffer.WaitForCompletion();
}

template <typename Input, typename Output>
bool Util::AsyncProcessor<Input, Output>::IsRunning() {
  return m_isRunning.load();
}

template <typename Input, typename Output>
void Util::AsyncProcessor<Input, Output>::Complete() {
  m_inputBuffer.Complete();
}

template <typename Input, typename Output>
void Util::AsyncProcessor<Input, Output>::Reset() {
  m_inputBuffer.Reset();
}

template <typename Input, typename Output>
void Util::AsyncProcessor<Input, Output>::Append(std::vector<Input>&& values) {
  m_inputBuffer.Append(std::forward<std::vector<Input>>(values));
}

template <typename Input, typename Output>
void Util::AsyncProcessor<Input, Output>::Append(Input&& value) {
  m_inputBuffer.Append(std::forward<Input>(value));
}

template <typename Input, typename Output>
void Util::AsyncProcessor<Input, Output>::Append(const Input& value) {
  m_inputBuffer.Append(value);
}

typedef Util::AsyncProcessor<std::function<void()>, void> BackgroundWorker;
}

#endif