#pragma once
#ifndef WORK_BUFFER_HPP

#include "boost/optional.hpp"
#include <chrono>
#include <future>
#include <vector>

#include "Arc_Assert.hpp"

#undef max
#undef min

namespace Util
{
	template<typename T>
	class InputBuffer
	{
	public:
		virtual void Append(const T& value) = 0;
		virtual void Append(T&& value) = 0;
		virtual void Append(std::vector<T>&& values) = 0;
		virtual void Complete() = 0;
	};

	template<typename T>
	class OutputBuffer
	{
	public:
		virtual bool ResultsPending() const = 0;
		virtual std::vector<T> GetAll() = 0;
		virtual std::vector<T> GetMultiple(size_t minimum, size_t maximum, boost::optional<std::chrono::milliseconds> = boost::none) = 0;
		virtual std::vector<T> GetN(size_t count, boost::optional<std::chrono::milliseconds> = boost::none) = 0;
		virtual std::vector<T> GetAtLeast(size_t count, boost::optional<std::chrono::milliseconds> = boost::none) = 0;
		virtual std::vector<T> GetAtMost(size_t count, boost::optional<std::chrono::milliseconds> = boost::none) = 0;
		virtual boost::optional<T> GetNext(boost::optional<std::chrono::milliseconds> = boost::none) = 0;
	};

	/************************************************************************/
	/* Simple produce-consumer queue                                        */
	/************************************************************************/
	template<typename T>
	class WorkBuffer final : public InputBuffer<T>, public OutputBuffer<T>
	{
	public:
		WorkBuffer() = default;
		WorkBuffer(const WorkBuffer&) = delete;
		WorkBuffer(const WorkBuffer&&) = delete;
		~WorkBuffer();
		
		// producer functions
		virtual void Append(const T& value) override;
		virtual void Append(T&& value) override;
		virtual void Append(std::vector<T>&& values) override;
		virtual void Complete() override;

		// consumer functions
		// Note: can return less than asked for if there isn't enough available and no more
		// work is expected
		virtual bool ResultsPending() const override;
		virtual std::vector<T> GetAll() override;
		virtual std::vector<T> GetMultiple(size_t minimum, size_t maximum, boost::optional<std::chrono::milliseconds> = boost::none) override;
		virtual std::vector<T> GetN(size_t count, boost::optional<std::chrono::milliseconds> = boost::none) override;
		virtual std::vector<T> GetAtLeast(size_t count, boost::optional<std::chrono::milliseconds> = boost::none) override;
		virtual std::vector<T> GetAtMost(size_t count, boost::optional<std::chrono::milliseconds> = boost::none) override;
		virtual boost::optional<T> GetNext(boost::optional<std::chrono::milliseconds> timeout = boost::none) override;
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
	void WorkBuffer<T>::Append(const T& value)
	{
		std::unique_lock<std::mutex> lock(m_lock);
		ARC_ASSERT(!m_isDone);
		m_buffer.push_back(value);
		m_consumerCP.notify_one();
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

		auto results = std::move(m_buffer);
		m_buffer = {};
		return results;
	}

	template<typename T>
	std::vector<T> WorkBuffer<T>::GetMultiple(size_t minimum, size_t maximum, boost::optional<std::chrono::milliseconds> timeout)
	{
		using namespace std::chrono;

		ARC_ASSERT(minimum > 0 && maximum > 0);
		ARC_ASSERT(minimum <= maximum);

		auto start = system_clock::now();
		std::unique_lock<std::mutex> lock(m_lock);

		if (m_buffer.size() < minimum && !m_isDone)
		{
			if (timeout)
			{
				while (start + *timeout >= system_clock::now() && !m_isDone && (m_buffer.size() < minimum))
				{
					m_consumerCP.wait(lock);
				}
			}
			else
			{
				m_consumerCP.wait(lock, [this, minimum]()
				{
					return m_buffer.size() >= minimum || m_isDone;
				});
			}
		}

		auto numToGet = m_buffer.size();
		numToGet = std::min(numToGet, maximum);
		std::vector<T> results;
		results.reserve(numToGet);

		auto last = std::next(begin(m_buffer), numToGet);
		std::move(begin(m_buffer), last, back_inserter(results));
		m_buffer.erase(begin(m_buffer), last);

		return results;
	}

	template<typename T>
	std::vector<T> WorkBuffer<T>::GetN(size_t count, boost::optional<std::chrono::milliseconds> timeout)
	{
		ARC_ASSERT(count > 0);
		return GetMultiple(count, count, timeout);
	}

	template<typename T>
	std::vector<T> WorkBuffer<T>::GetAtLeast(size_t count, boost::optional<std::chrono::milliseconds> timeout)
	{
		ARC_ASSERT(count > 0);
		return GetMultiple(count, std::numeric_limits<size_t>::max(), timeout);
	}

	template<typename T>
	std::vector<T> WorkBuffer<T>::GetAtMost(size_t count, boost::optional<std::chrono::milliseconds> timeout)
	{
		ARC_ASSERT(count > 0);
		return GetMultiple(1, count, timeout);
	}


	template<typename T>
	boost::optional<T> WorkBuffer<T>::GetNext(boost::optional<std::chrono::milliseconds> timeout /*= boost::none*/)
	{
		using namespace std::chrono;

		boost::optional<T> result = boost::none;
		auto start = system_clock::now();
		std::unique_lock<std::mutex> lock(m_lock);

		if (m_buffer.size() < 1 || !m_isDone)
		{
			if (timeout)
			{
				while (start + *timeout >= system_clock::now() && !m_isDone && (m_buffer.size() < 1))
				{
					m_consumerCP.wait(lock);
				}
			}
			else
			{
				m_consumerCP.wait(lock, [this]()
				{
					return m_buffer.size() >= 1 || m_isDone;
				});
			}
		}

		if (m_buffer.size() > 0)
		{
			result = m_buffer.back();
			m_buffer.pop_back();
		}

		return result;
	}
}

#endif