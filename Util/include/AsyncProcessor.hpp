#pragma once
#ifndef ASYNC_PROCESSOR_HPP
#define ASYNC_PROCESSOR_HPP

#include <algorithm>
#include <functional>
#include <future>

#include "WorkBuffer.hpp"

namespace Util
{
	template<typename Input, typename Output>
	class AsyncProcessor final
	{
	public:

		typedef std::function<Output(const Input&)> ProcessSingle;
		typedef std::function<std::vector<Output>(const std::vector<Input>&)> ProcessBatch;

		// processes input in batches
		AsyncProcessor(int minBatchSize = 20,
			           int maxBatchSize = 100,
			           boost::optional<std::chrono::milliseconds> batchTimeout = boost::none);

		AsyncProcessor(const AsyncProcessor&) = delete;
		AsyncProcessor(const AsyncProcessor&&) = delete;

		~AsyncProcessor();
		
		void Start();
		void SetProcessor(ProcessSingle func);
		void SetBatchProcessor(ProcessBatch func);

		InputBuffer<Input>* InputBuffer();
		OutputBuffer<Output>* OutputBuffer();
	private:
		Util::WorkBuffer<Input> m_inputBuffer;
		Util::WorkBuffer<Output> m_outputBuffer;
		std::function<void()> m_processorFunction;
		std::future<void> m_workFuture;

		std::atomic<bool> m_isRunning;
		int m_minBatchSize = 0;
		int m_maxBatchSize = 0;
		boost::optional<std::chrono::milliseconds> m_timeout = boost::none;
	};

	template<typename Input, typename Output>
	AsyncProcessor<Input, Output>::AsyncProcessor(int minBatchSize /*= 20*/,
		                                          int maxBatchSize /*= 100*/,
		                                          boost::optional<std::chrono::milliseconds> batchTimeout /*= boost::none*/)
		: m_minBatchSize(minBatchSize)
		, m_maxBatchSize(maxBatchSize)
		, m_timeout(batchTimeout)
		, m_isRunning(false)
	{
		ARC_ASSERT(minBatchSize <= maxBatchSize);
		ARC_ASSERT(minBatchSize > 0 && maxBatchSize > 0);
	}

	template<typename Input, typename Output>
	Util::AsyncProcessor<Input, Output>::~AsyncProcessor()
	{
		ARC_ASSERT_MSG(!m_outputBuffer.ResultsPending(), "The Processor should not be deleted while it still has outstanding work!");

		// nonetheless we will wait as this is the safest choice to make at this point. However this may cause some deadlock...
		m_inputBuffer.Complete(); // force the input buffer to close
		// Note: there is a small race condition with m_workFuture. If the object is destroyed and the processing
		// is started at the same time this will break. Not fixing because the object should never be allowed into
		// this state so I'm relying upon the assert identify and help to correct errors like this.
		if (m_workFuture.valid())
		{
			try
			{
				m_workFuture.wait();
			}
			catch (Concurrency::invalid_operation& ex)
			{
				OutputDebugStringA(ex.what());
				ARC_FAIL("Likely attempted to destroy an async processor on the UI thread, behaviour beyond this point is unpredictable");
			}
		}
	}


	template<typename Input, typename Output>
	void Util::AsyncProcessor<Input, Output>::SetProcessor(ProcessSingle func)
	{
		ARC_ASSERT(!m_isRunning);

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
	}

	template<typename Input, typename Output>
	void Util::AsyncProcessor<Input, Output>::SetBatchProcessor(ProcessBatch func)
	{
		ARC_ASSERT(!m_isRunning);

		m_processorFunction = [this, func]()
		{
			while (m_inputBuffer.ResultsPending())
			{
				auto input = m_inputBuffer.GetMultiple(m_minBatchSize, m_maxBatchSize, m_timeout);
				if (input.size() > 0)
				{
					auto output = func(input);
					m_outputBuffer.Append(std::move(output));
				}
			}

			m_outputBuffer.Complete();
		};
	}

	template<typename Input, typename Output>
	void AsyncProcessor<Input, Output>::Start()
	{
		ARC_ASSERT(!m_isRunning);
		m_isRunning = true;
		m_workFuture = std::async(m_processorFunction);
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