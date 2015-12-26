#pragma once
#ifndef FILTER_PROCESSOR_HPP
#define FILTER_PROCESSOR_HPP

#include <algorithm>

#include "WorkBuffer.hpp"

namespace Util {
template <typename T>
class FilterProcessor final : public InputBuffer<T> {
 public:
  FilterProcessor(std::function<bool(const T)> pred);
  void ConnectMatchingBuffer(InputBuffer<T>* matchingBuffer);
  void ConnectRejectBuffer(InputBuffer<T>* rejectBuffer);

  // InputBuffer
  void Append(const T& value) override;
  void Append(T&& value) override;
  void Append(std::vector<T>&& values) override;
  void Complete() override;
  void Reset() override;

 private:
  InputBuffer<T>* m_matchingBuffer = nullptr;
  InputBuffer<T>* m_rejectBuffer = nullptr;
  std::function<bool(const T)> m_pred;
};

template <typename T>
Util::FilterProcessor<T>::FilterProcessor(std::function<bool(const T)> pred)
    : m_pred(pred) {}

template <typename T>
void Util::FilterProcessor<T>::Reset() {
  if (m_matchingBuffer != nullptr) {
    m_matchingBuffer->Reset();
  }
  if (m_rejectBuffer) {
    m_rejectBuffer->Reset();
  }
}

template <typename T>
void Util::FilterProcessor<T>::Complete() {
  if (m_matchingBuffer != nullptr) {
    m_matchingBuffer->Complete();
  }
  if (m_rejectBuffer) {
    m_rejectBuffer->Complete();
  }
}

template <typename T>
void Util::FilterProcessor<T>::Append(std::vector<T>&& values) {
  std::vector<T> matching;
  std::vector<T> rejected;

  for (auto&& value : values) {
    if (m_pred(value)) {
      matching.push_back(std::move(value));
    } else {
      rejected.push_back(std::move(value));
    }
  }

  ARC_ASSERT(m_matchingBuffer != nullptr || m_rejectBuffer != nullptr);
  if (m_matchingBuffer != nullptr) {
    m_matchingBuffer->Append(std::move(matching));
  }
  if (m_rejectBuffer != nullptr) {
    m_rejectBuffer->Append(std::move(rejected));
  }
}

template <typename T>
void Util::FilterProcessor<T>::Append(T&& value) {
  ARC_ASSERT(m_matchingBuffer != nullptr || m_rejectBuffer != nullptr);
  if (m_pred(value)) {
    if (m_matchingBuffer != nullptr) {
      m_matchingBuffer->Append(std::move(value));
    }
  } else {
    if (m_rejectBuffer != nullptr) {
      m_rejectBuffer->Append(std::move(value));
    }
  }
}

template <typename T>
void Util::FilterProcessor<T>::Append(const T& value) {
  ARC_ASSERT(m_matchingBuffer != nullptr || m_rejectBuffer != nullptr);
  if (m_pred(value)) {
    if (m_matchingBuffer != nullptr) {
      m_matchingBuffer->Append(value);
    }
  } else {
    if (m_rejectBuffer != nullptr) {
      m_rejectBuffer->Append(value);
    }
  }
}

template <typename T>
void Util::FilterProcessor<T>::ConnectRejectBuffer(InputBuffer<T>* rejectBuffer) {
  ARC_ASSERT(rejectBuffer != nullptr);
  m_rejectBuffer = rejectBuffer;
}

template <typename T>
void Util::FilterProcessor<T>::ConnectMatchingBuffer(InputBuffer<T>* matchingBuffer) {
  ARC_ASSERT(matchingBuffer != nullptr);
  m_matchingBuffer = matchingBuffer;
}
}

#endif