#pragma once
#ifndef LOCK_HELPER_HPP
#define LOCK_HELPER_HPP

#include <functional>
#include <future>
#include <windows.h>
#include <unordered_set>

#include "ScopeGuard.hpp"

namespace Util
{
	// Wrapper for SRWLOCK that adds recursive capabilities, very simple and probably not performant
	// However, getting boost::thread compiling for winrt was proving difficult
	class SlimRWLock final
	{
	public:
		SlimRWLock();
		SlimRWLock(const SlimRWLock&) = delete;
		SlimRWLock& operator=(const SlimRWLock&) = delete;

		void LockShared();
		void LockExclusive();

		void UnlockShared();
		void UnlockExclusive();
	private:
		SRWLOCK m_lock;
		std::mutex m_syncLock;
		std::unordered_set<std::thread::id> m_readIds;
		std::unique_ptr<std::thread::id> m_writeId; // TODO::JT make this an optional, not a pointer
	};

	class WriteLock final : public ScopeGuard < std::function<void()> >
	{
	public:
		WriteLock(SRWLOCK* lock);
		WriteLock(SlimRWLock* lock);
		virtual ~WriteLock();
	};
}

#endif