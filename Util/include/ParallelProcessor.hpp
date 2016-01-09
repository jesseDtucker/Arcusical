#pragma once
#ifndef PARALLEL_PROCESSOR_HPP
#define PARALLEL_PROCESSOR_HPP

#include <future>
#include <queue>
#include <utility>
#include <vector>

#include "Arc_Assert.hpp"
#include "AsyncProcessor.hpp"
#include "WorkBuffer.hpp"

namespace Util {

template <typename Output>
class AccumlatingWorkBuffer final : public WorkBuffer<Output> {
 public:
  AccumlatingWorkBuffer(int count);
  void Complete() override;

 private:
  std::atomic_int m_count;
};

template <typename Input, typename Output>
class ParallelProcessor final : public InputBuffer<Input> {
  typedef AsyncProcessor<Input, Output> Worker;

 public:
  typedef typename Worker::ProcessSingle WorkFunc;

  ParallelProcessor(WorkFunc workFunc, int numWorkers = 2);
  ~ParallelProcessor() = default;

  ParallelProcessor(const ParallelProcessor&) = delete;
  ParallelProcessor(ParallelProcessor&&) = delete;
  ParallelProcessor& operator=(const ParallelProcessor&) = delete;
  ParallelProcessor& operator=(ParallelProcessor&&) = delete;

  void Append(const Input& value) override;
  void Append(Input&& value) override;
  void Append(std::vector<Input>&& values) override;
  void Complete() override;
  void Reset() override;

  void Start();
  OutputBuffer<Output>& ResultsBuffer();

 private:
  Worker& GetNextWorker();

  std::vector<Worker> m_workers;
  std::atomic_int m_itemCounter;
  AccumlatingWorkBuffer<Output> m_workBuffer;
};

template <typename Input, typename Output>
ParallelProcessor<Input, Output>::ParallelProcessor(WorkFunc workFunc, int numWorkers /* = 2 */)
    : m_itemCounter(0), m_workBuffer(numWorkers) {
  ARC_ASSERT(numWorkers > 0);
  m_workers.resize(numWorkers);
  for (auto& worker : m_workers) {
    worker.SetProcessor(workFunc);
    worker.Connect(&m_workBuffer);
  }
}

template <typename Input, typename Output>
typename ParallelProcessor<Input, Output>::Worker& ParallelProcessor<Input, Output>::GetNextWorker() {
  // work is distributed evenly amongst the workers
  // Note: the itemCounter is incremented on access!
  int itemNumber = m_itemCounter++;
  return m_workers[itemNumber % m_workers.size()];
}

template <typename Input, typename Output>
void ParallelProcessor<Input, Output>::Append(const Input& input) {
  GetNextWorker().Append(input);
}

template <typename Input, typename Output>
void ParallelProcessor<Input, Output>::Append(Input&& input) {
  GetNextWorker().Append(std::move(input));
}

template <typename Input, typename Output>
void ParallelProcessor<Input, Output>::Append(std::vector<Input>&& inputs) {
  for (auto&& input : inputs) {
    Append(std::move(input));
  }
}

template <typename Input, typename Output>
void ParallelProcessor<Input, Output>::Complete() {
  for (auto& worker : m_workers) {
    worker.Complete();
  }
}

template <typename Input, typename Output>
void ParallelProcessor<Input, Output>::Reset() {
  for (auto& worker : m_workers) {
    worker.Reset();
  }
}

template <typename Input, typename Output>
void ParallelProcessor<Input, Output>::Start() {
  for (auto& worker : m_workers) {
    worker.Start();
  }
}

template <typename Input, typename Output>
OutputBuffer<Output>& ParallelProcessor<Input, Output>::ResultsBuffer() {
  return m_workBuffer;
}

template <typename Output>
AccumlatingWorkBuffer<Output>::AccumlatingWorkBuffer(int count) : m_count(count) {}

template <typename Output>
void AccumlatingWorkBuffer<Output>::Complete() {
  m_count--;
  if (m_count <= 0) {
    WorkBuffer<Output>::Complete();
  }
}
}

#endif