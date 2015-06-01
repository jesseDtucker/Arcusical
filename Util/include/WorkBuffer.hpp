#pragma once
#ifndef WORK_BUFFER_HPP

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
		
		// producer functions
		void Append(T&& value);
		void Append(std::vector<T>&& values);
		void Complete();

		// consumer functions
		std::vector<T> GetAll();
		std::vector<T> GetN(int count);
		std::vector<T> WaitAndGet(std::chrono::milliseconds waitTime);
	private:
		std::condition_variable m_consumerCP;
		std::mutex m_lock;
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
	std::vector<T> WorkBuffer<T>::WaitAndGet(std::chrono::milliseconds waitTime)
	{
		{
			std::unique_lock<std::mutex> lock(m_lock);
			ARC_ASSERT(!m_isDone);
		}

		std::this_thread::sleep_for(waitTime);

		std::unique_lock<std::mutex> lock(m_lock);
		auto result = std::move(m_buffer);
		m_buffer = {};
		return std::move(result);
	}
}

#endif