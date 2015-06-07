#pragma once
#ifndef ASYNC_PROCESSOR_HPP
#define ASYNC_PROCESSOR_HPP

#include <algorithm>
#include <functional>

#include "WorkBuffer.hpp"

namespace Util
{
	template<typename Input, typename Output>
	class AsyncProcessor final
	{
	public:

		typedef std::function<Output(const Input&)> ProcessSingle;
		typedef std::function<std::vector<Output>(const std::vector<Input>&)> ProcessBatch;

		// Processes input one by one
		AsyncProcessor(ProcessSingle f,
			           bool autoStart = true,
			           boost::optional<std::chrono::milliseconds> timeout = boost::none);

		// processes input in batches
		AsyncProcessor(ProcessBatch f,
			           bool autoStart = true,
			           int minBatchSize = 20,
			           int maxBatchSize = 100,
			           boost::optional<std::chrono::milliseconds> batchTimeout = boost::none);

		AsyncProcessor(const AsyncProcessor&) = delete;
		AsyncProcessor(const AsyncProcessor&&) = delete;
		
		void Start();

		InputBuffer<Input>* InputBuffer();
		OutputBuffer<Output>* OutputBuffer();
	private:
		Util::WorkBuffer<Input> m_inputBuffer;
		Util::WorkBuffer<Output> m_outputBuffer;
		std::function<void()> m_processorFunction;

		bool m_isRunning;
		int m_minBatchSize = 0;
		int m_maxBatchSize = 0;
		boost::optional<std::chrono::milliseconds> m_timeout = boost::none;
	};

	template<typename Input, typename Output>
	AsyncProcessor<Input, Output>::AsyncProcessor(ProcessSingle func,
		                                          bool autoStart /*= true*/,
		                                          boost::optional<std::chrono::milliseconds> timeout)
		: m_timeout(timeout)
		, m_isRunning(false)
	{
		m_processorFunction = [this, func]()
		{
			while (m_inputBuffer.ResultsPending())
			{
				auto input = m_inputBuffer.GetNext(m_timeout);
				if (input)
				{
					auto output = func(*input);
					m_outputBuffer.Append(output);
				}
			}

			m_outputBuffer.Complete();
		};

		if (autoStart)
		{
			Start();
		}
	}

	template<typename Input, typename Output>
	AsyncProcessor<Input, Output>::AsyncProcessor(ProcessBatch func,
		                                          bool autoStart /*= true*/,
		                                          int minBatchSize /*= 20*/,
		                                          int maxBatchSize /*= 100*/,
		                                          boost::optional<std::chrono::milliseconds> batchTimeout /*= boost::none*/)
		: m_minBatchSize(minBatchSize)
		, m_maxBatchSize(maxBatchSize)
		, m_timeout(batchTimeout)
		, m_isRunning(false)
	{
		ARC_ASSERT(minBatchSize <= maxBatchSize);
		ARC_ASSERT(minBatchSize > 0 && maxBatchSize > 0);

		m_processorFunction = [this, func]()
		{
			while (m_inputBuffer.ResultsPending())
			{
				auto input = m_inputBuffer.GetMultiple(m_minBatchSize, m_maxBatchSize, m_timeout);
				auto output = func(input);

				m_outputBuffer.Append(std::move(output));
			}

			m_outputBuffer.Complete();
		};

		if (autoStart)
		{
			Start();
		}
	}

	template<typename Input, typename Output>
	void AsyncProcessor<Input, Output>::Start()
	{
		ARC_ASSERT(!m_isRunning);
		m_isRunning = true;
		std::async(m_processorFunction);
	}

	template<typename Input, typename Output>
	InputBuffer<Input>* AsyncProcessor<Input, Output>::InputBuffer()
	{
		return &m_inputBuffer;
	}

	template<typename Input, typename Output>
	OutputBuffer<Output>* AsyncProcessor<Input, Output>::OutputBuffer()
	{
		return &m_outputBuffer;
	}
}

#endif