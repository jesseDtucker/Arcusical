#pragma once
#ifndef WORK_BUFFER_HPP

#define NOMINMAX

#include <chrono>
#include <future>
#include <vector>

#include "Arc_Assert.hpp"

namespace Util
{
	/************************************************************************/
	/* Simple produce-consumer queue                                        */
	/************************************************************************/
	template<typename T>
	class WorkBuffer final
	{
	public:
		WorkBuffer() = default;
		WorkBuffer(const WorkBuffer&) = delete;
		WorkBuffer(const WorkBuffer&&) = delete;
		~WorkBuffer();
		
		bool ResultsPending() const;

		// producer functions
		void Append(T&& value);
		void Append(std::vector<T>&& values);
		void Complete();

		// consumer functions
		std::vector<T> GetAll();
		std::vector<T> GetN(int count);
		std::vector<T> WaitAndGet(std::chrono::milliseconds waitTime, size_t numToGet = 0);
	private:
		mutable std::condition_variable m_consumerCP;
		mutable std::mutex m_lock;
		std::vector<T> m_buffer;
		bool m_isDone = false;
	};

	/************************************************************************/
	/* Implementation                                                       */
	/************************************************************************/

	template<typename T>
	WorkBuffer<T>::~WorkBuffer()
	{
		if (!m_isDone)
		{
			Complete();
		}
	}

	template<typename T>
	bool WorkBuffer<T>::ResultsPending() const
	{
		std::unique_lock<std::mutex> lock(m_lock);
		return !m_isDone || m_buffer.size() > 0;
	}

	template<typename T>
	void WorkBuffer<T>::Append(T&& value)
	{
		std::unique_lock<std::mutex> lock(m_lock);
		ARC_ASSERT(!m_isDone);
		m_buffer.push_back(std::move(value));
		m_consumerCP.notify_one();
	}

	template<typename T>
	void WorkBuffer<T>::Append(std::vector<T>&& values)
	{
		std::unique_lock<std::mutex> lock(m_lock);
		ARC_ASSERT(!m_isDone);
		std::move(begin(values), end(values), back_inserter(m_buffer));
		m_consumerCP.notify_one();
	}

	template<typename T>
	void WorkBuffer<T>::Complete()
	{
		std::unique_lock<std::mutex> lock(m_lock);
		ARC_ASSERT(!m_isDone);

		m_isDone = true;
		m_consumerCP.notify_all();
	}

	template<typename T>
	std::vector<T> WorkBuffer<T>::GetAll()
	{
		std::unique_lock<std::mutex> lock(m_lock);

		if (!m_isDone)
		{
			m_consumerCP.wait(lock, [this]()
			{
				return m_isDone;
			});
		}

		auto result = std::move(m_buffer);
		m_buffer = {};
		return std::move(result);
	}

	template<typename T>
	std::vector<T> WorkBuffer<T>::GetN(int count)
	{
		std::unique_lock<std::mutex> lock(m_lock);
		ARC_ASSERT(!m_isDone);

		if (count > m_buffer.size())
		{
			// there isn't enough work available to satisfy the request
			// wait until there is
			m_consumerCP.wait(lock, [this, count]()
			{
				return m_buffer.size() >= count || m_isDone;
			});
		}

		auto n = std::min(count, m_buffer.size());
		std::vector<T> results;
		results.reserve(n);
		
		auto last = std::next(begin(m_buffer), n);
		std::move(begin(m_buffer), last, back_inserter(results));
		m_buffer.erase(begin(m_buffer), last);

		return results;
	}

	template<typename T>
	std::vector<T> WorkBuffer<T>::WaitAndGet(std::chrono::milliseconds waitTime, size_t numToGet/* = 0 */)
	{
		using namespace std::chrono;

		std::unique_lock<std::mutex> lock(m_lock);
		auto start = system_clock::now();
		// what this means is we wait until either the timeout, all work is done or we have enough results to fill the request
		while (start + waitTime >= system_clock::now() && !m_isDone && (m_buffer.size() < numToGet || numToGet == 0))
		{
			m_consumerCP.wait(lock);
		}

		// zero means they want everything, after all getting zero doesn't make much sense
		numToGet = (numToGet == 0) ? m_buffer.size() : numToGet;
		numToGet = std::min(numToGet, m_buffer.size());
		std::vector<T> results;
		results.reserve(numToGet);

		auto last = std::next(begin(m_buffer), numToGet);
		std::move(begin(m_buffer), last, back_inserter(results));
		m_buffer.erase(begin(m_buffer), last);

		return results;
	}
}

#endif