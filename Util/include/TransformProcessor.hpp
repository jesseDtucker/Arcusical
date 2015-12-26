#pragma once
#ifndef TRANSFORM_PROCESSOR_HPP
#define TRANSFORM_PROCESSOR_HPP

#include <algorithm>

#include "WorkBuffer.hpp"

namespace Util {
template <typename Input, typename Output>
class TransformProcessor final : public InputBuffer<Input> {
 public:
  typedef std::function<Output(const Input&)> ProcessSingle;
  typedef std::function<std::vector<Output>(const std::vector<Input>&)> ProcessBatch;

  // Don't need these for now so not supporting
  TransformProcessor(const TransformProcessor&) = delete;
  TransformProcessor(TransformProcessor&&) = delete;

  TransformProcessor() = default;
  TransformProcessor(ProcessSingle operation);

  void ConnectBuffer(InputBuffer<Output>* outputBuffer);
  void SetBatchProcessor(ProcessBatch operation);

  // InputBuffer
  virtual void Append(const Input& value);
  virtual void Append(Input&& value);
  virtual void Append(std::vector<Input>&& values);
  virtual void Complete();
  virtual void Reset();

 private:
  InputBuffer<Output>* m_outputBuffer = nullptr;
  ProcessSingle m_singleOperation;
  ProcessBatch m_batchOperation;
};

template <typename Input, typename Output>
Util::TransformProcessor<Input, Output>::TransformProcessor(ProcessSingle operation)
    : m_singleOperation(operation) {}

template <typename Input, typename Output>
void Util::TransformProcessor<Input, Output>::SetBatchProcessor(ProcessBatch operation) {
  m_batchOperation = operation;
}

template <typename Input, typename Output>
void Util::TransformProcessor<Input, Output>::Reset() {
  if (nullptr != m_outputBuffer) {
    m_outputBuffer->Reset();
  }
}

template <typename Input, typename Output>
void Util::TransformProcessor<Input, Output>::Complete() {
  if (nullptr != m_outputBuffer) {
    m_outputBuffer->Complete();
  }
}

template <typename Input, typename Output>
void Util::TransformProcessor<Input, Output>::Append(std::vector<Input>&& values) {
  if (nullptr != m_batchOperation) {
    auto results = m_batchOperation(std::move(values));
  } else {
    auto input = std::move(values);
    for (auto&& item : input) {
      Append(item);
    }
  }
}

template <typename Input, typename Output>
void Util::TransformProcessor<Input, Output>::Append(Input&& value) {
  ARC_ASSERT(nullptr != m_singleOperation);
  auto result = m_singleOperation(std::move(value));
  if (nullptr != m_outputBuffer) {
    m_outputBuffer->Append(std::move(result));
  }
}

template <typename Input, typename Output>
void Util::TransformProcessor<Input, Output>::Append(const Input& value) {
  ARC_ASSERT(nullptr != m_singleOperation);
  auto result = m_singleOperation(value);
  if (nullptr != m_outputBuffer) {
    m_outputBuffer->Append(result);
  }
}

template <typename Input, typename Output>
void Util::TransformProcessor<Input, Output>::ConnectBuffer(InputBuffer<Output>* outputBuffer) {
  ARC_ASSERT(nullptr != outputBuffer);
  m_outputBuffer = outputBuffer;
}
}

#endif