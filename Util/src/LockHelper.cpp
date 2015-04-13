#include "LockHelper.hpp"

#include "Arc_Assert.hpp"

using namespace std;

Util::SlimRWLock::SlimRWLock()
{
	InitializeSRWLock(&m_lock);
}

void Util::SlimRWLock::LockShared()
{
	bool shouldLock = true;

	{
		lock_guard<mutex> guard(m_syncLock);
		auto id = this_thread::get_id();
		shouldLock = (m_readIds.find(id) == end(m_readIds)) && (m_writeId == nullptr || id != *m_writeId);
		if (shouldLock)
		{
			m_readIds.insert(id);
		}
	}
	
	if (shouldLock)
	{
		AcquireSRWLockShared(&m_lock);
	}
}

void Util::SlimRWLock::LockExclusive()
{
	bool shouldLock = true;
	bool shouldUnlockRead = false;

	{
		lock_guard<mutex> guard(m_syncLock);
		auto id = this_thread::get_id();
		if (m_writeId != nullptr && *m_writeId == id)
		{
			// current thread already has a lock write
			shouldLock = false;
		}
		else if (m_readIds.find(id) != end(m_readIds))
		{
			// current thread has a read lock but no write lock
			// first release the read lock, before trying to acquire the write lock
			shouldLock = true;
			shouldUnlockRead = true;
		}

		if (shouldLock)
		{
			m_writeId = make_unique<thread::id>(id);
		}
	}

	if (shouldUnlockRead)
	{
		this->UnlockShared();
	}
	if (shouldLock)
	{
		AcquireSRWLockExclusive(&m_lock);
	}
}

void Util::SlimRWLock::UnlockShared()
{
	bool shouldUnlock = true;
	{
		lock_guard<mutex> guard(m_syncLock);
		auto id = this_thread::get_id();
		shouldUnlock = (m_readIds.find(id) != end(m_readIds));
		m_readIds.erase(id);
	}

	if (shouldUnlock)
	{
		ReleaseSRWLockShared(&m_lock);
	}
}

void Util::SlimRWLock::UnlockExclusive()
{
	{
		lock_guard<mutex> guard(m_syncLock);
		auto id = this_thread::get_id();
		ARC_ASSERT(m_writeId != nullptr);
		ARC_ASSERT_MSG(id == *m_writeId, "Remember to unlock from the same thead that performed the lock!");
		m_writeId = nullptr; // assuming this is never a valid thread id
	}

	ReleaseSRWLockExclusive(&m_lock);
}

Util::WriteLock::WriteLock(SlimRWLock* lock)
	: ScopeGuard([lock](){ lock->UnlockExclusive(); })
{
	lock->LockExclusive();
}

Util::WriteLock::WriteLock(SRWLOCK* lock)
	: ScopeGuard([lock](){ ReleaseSRWLockExclusive(lock); })
{
	AcquireSRWLockExclusive(lock);
}

Util::WriteLock::~WriteLock()
{ }
