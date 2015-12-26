#pragma once
#ifndef PARALLEL_ORDERED_PROCESSOR_HPP
#define PARALLEL_ORDERED_PROCESSOR_HPP

#include <future>
#include <queue>
#include <utility>
#include <vector>

#include "Arc_Assert.hpp"
#include "AsyncProcessor.hpp"

namespace Util {
template <typename Input, typename Output>
class ParallelOrderedProcessor final {
  typedef std::pair<int, Input> InputUnit;
  typedef std::pair<int, Output> OutputUnit;

  typedef AsyncProcessor<InputUnit, OutputUnit> Worker;

 public:
  typedef std::function<Output(const Input&)> WorkFunc;

  ParallelOrderedProcessor(int numWorkers = 2);
  ~ParallelOrderedProcessor();

  ParallelOrderedProcessor(const ParallelOrderedProcessor&) = delete;
  ParallelOrderedProcessor(ParallelOrderedProcessor&&) = delete;
  ParallelOrderedProcessor& operator=(const ParallelOrderedProcessor&) = delete;
  ParallelOrderedProcessor& operator=(ParallelOrderedProcessor&&) = delete;

  void SetWorkFunc(WorkFunc func);
  void AddItem(Input&& input);
  void AddItems(std::vector<Input>&& inputs);

  void StopWork();
  Output GetNextResult();
  bool HasResult();

  Util::MulticastDelegate<void()> OnResultReady;

 private:
  OutputUnit WorkWrapper(const InputUnit& input);
  void PullInWorkResults(bool blockForAll = false);

  WorkFunc m_workFunc;
  std::vector<Worker> m_workers;
  std::queue<Output> m_orderedOutput;
  std::vector<OutputUnit> m_unorderedOutput;
  std::mutex m_syncLock;
  std::atomic<int> m_itemCounter;
  std::atomic<int> m_doneItemCounter;
  std::vector<Util::Subscription> m_processorSubscriptions;
  bool m_isStoppingWork = false;
};

template <typename Input, typename Output>
ParallelOrderedProcessor<Input, Output>::ParallelOrderedProcessor(int numWorkers /* = 4 */)
    : m_itemCounter(0), m_doneItemCounter(0) {
  ARC_ASSERT(numWorkers > 0);
  m_workers.resize(numWorkers);
}

template <typename Input, typename Output>
ParallelOrderedProcessor<Input, Output>::~ParallelOrderedProcessor() {
  StopWork();
}

template <typename Input, typename Output>
void ParallelOrderedProcessor<Input, Output>::SetWorkFunc(WorkFunc func) {
  auto workListener = [this]() {
    std::lock_guard<std::mutex> lock(m_syncLock);
    if (!m_isStoppingWork) {
      PullInWorkResults();
    }
  };
  m_workFunc = func;
  for (auto& worker : m_workers) {
    auto sub = worker.OutputBuffer()->ItemAvailable += workListener;
    m_processorSubscriptions.push_back(std::move(sub));
    worker.SetProcessor([this](const InputUnit& input) { return this->WorkWrapper(input); });
    worker.Start();
  }
}

template <typename Input, typename Output>
typename ParallelOrderedProcessor<Input, Output>::OutputUnit ParallelOrderedProcessor<Input, Output>::WorkWrapper(
    const InputUnit& input) {
  auto id = input.first;
  auto res = m_workFunc(input.second);
  return std::make_pair<OutputUnit::first_type, OutputUnit::second_type>(std::move(id), std::move(res));
}

template <typename Input, typename Output>
void ParallelOrderedProcessor<Input, Output>::AddItem(Input&& input) {
  // work is distributed evenly amongst the workers
  // Note: the itemCounter is incremented on access!
  int itemNumber = m_itemCounter++;
  auto& worker = m_workers[itemNumber % m_workers.size()];
  auto inputUnit =
      std::make_pair<InputUnit::first_type, InputUnit::second_type>(std::move(itemNumber), std::move(input));
  worker.InputBuffer()->Append(std::move(inputUnit));
}

template <typename Input, typename Output>
void ParallelOrderedProcessor<Input, Output>::AddItems(std::vector<Input>&& inputs) {
  // work is distributed evenly amongst the workers
  for (auto& input : inputs) {
    AddItem(input);
  }
}

template <typename Input, typename Output>
void ParallelOrderedProcessor<Input, Output>::StopWork() {
  {
    std::lock_guard<std::mutex> lock(m_syncLock);
    m_isStoppingWork = true;
  }

  for (auto& worker : m_workers) {
    worker.DropAllWork();  // drop any work that is not yet in processing
  }
  for (auto& worker : m_workers) {
    worker.OutputBuffer()->WaitForCompletion();
    // there may have been work in flight at the time. Dropping again after completion to ensure no pending work
    // remains in the queue.
    worker.DropAllWork();
  }

  std::lock_guard<std::mutex> lock(m_syncLock);
  m_orderedOutput = {};
  m_unorderedOutput = {};
  m_doneItemCounter = 0;
  m_itemCounter = 0;

  m_isStoppingWork = false;
}

template <typename Input, typename Output>
typename Output ParallelOrderedProcessor<Input, Output>::GetNextResult() {
  std::lock_guard<std::mutex> lock(m_syncLock);
  if (m_orderedOutput.size() == 0) {
    PullInWorkResults();
  }
  ARC_ASSERT(m_orderedOutput.size() > 0);
  auto next = std::move(m_orderedOutput.front());
  m_orderedOutput.pop();
  return next;
}

template <typename Input, typename Output>
bool ParallelOrderedProcessor<Input, Output>::HasResult() {
  std::lock_guard<std::mutex> lock(m_syncLock);
  bool hasResults = m_orderedOutput.size() > 0;
  if (!hasResults) {
    // it's possible the work is ready but hasn't been pulled in yet
    // pull it in then try again.
    PullInWorkResults();
    hasResults = m_orderedOutput.size() > 0;
  }
  return hasResults;
}

template <typename Input, typename Output>
void ParallelOrderedProcessor<Input, Output>::PullInWorkResults(bool blockForAll /*= false*/) {
  using namespace std;

  if (blockForAll) {
    for (auto& worker : m_workers) {
      auto results = worker.OutputBuffer()->GetAll();
      std::move(begin(results), end(results), back_inserter(m_unorderedOutput));
    }
  } else {
    for (auto& worker : m_workers) {
      auto results = worker.OutputBuffer()->GetAtLeast(0);
      std::move(begin(results), end(results), back_inserter(m_unorderedOutput));
    }
  }

  // take each item that is done and try to put them in order on the queue. Anything that is left
  // will be processed later once the work that comes before it is ready.
  auto searchPred = [this](const OutputUnit& out) { return out.first == this->m_doneItemCounter; };

  auto outputAvailableBefore = m_orderedOutput.size();
  bool done = false;
  while (!done) {
    auto nextInOrder = find_if(begin(m_unorderedOutput), end(m_unorderedOutput), searchPred);
    done = (nextInOrder == end(m_unorderedOutput));
    if (!done) {
      m_orderedOutput.push(std::move(nextInOrder->second));
      m_unorderedOutput.erase(nextInOrder);
      ++m_doneItemCounter;
    }
  }

  if (blockForAll) {
    ARC_ASSERT(m_doneItemCounter == m_itemCounter);
    ARC_ASSERT(m_unorderedOutput.size() == 0);
  }

  auto newOutputCount = m_orderedOutput.size() - outputAvailableBefore;
  // notify for each new result we have available
  for (decltype(newOutputCount) i = 0; i < newOutputCount; ++i) {
    OnResultReady();
  }
}
}

#endif