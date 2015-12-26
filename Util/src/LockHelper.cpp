#include "LockHelper.hpp"

#include "Arc_Assert.hpp"

using namespace std;

Util::SlimRWLock::SlimRWLock() { InitializeSRWLock(&m_lock); }

void Util::SlimRWLock::LockShared() {
#if _DEBUG
  {
    // ensure we don't already hold this lock
    lock_guard<mutex> guard(m_syncLock);
    auto id = this_thread::get_id();

    auto readId = m_readIds.find(id);
    ARC_ASSERT_MSG(readId == end(m_readIds),
                   "Attempted to recursively hold a read lock! This operation is not supported!");
    ARC_ASSERT_MSG(!m_writeId || m_writeId != id,
                   "Attempted to recursively hold a read lock while holding a write lock! This is not supported!");

    m_readIds.insert(id);
  }
#endif

  AcquireSRWLockShared(&m_lock);
}

void Util::SlimRWLock::UnlockShared() {
#if _DEBUG
  {
    lock_guard<mutex> guard(m_syncLock);
    auto id = this_thread::get_id();

    auto readId = m_readIds.find(id);
    ARC_ASSERT_MSG(readId != end(m_readIds),
                   "Attempted to release a read lock from a thread that does not hold a read lock!");
    m_readIds.erase(readId);
  }
#endif

  ReleaseSRWLockShared(&m_lock);
}

void Util::SlimRWLock::LockExclusive() {
#if _DEBUG
  {
    lock_guard<mutex> guard(m_syncLock);
    auto id = this_thread::get_id();

    auto readId = m_readIds.find(id);
    ARC_ASSERT_MSG(
        readId == end(m_readIds),
        "Attempted to recursively hold a write lock while holding a read lock! This operation is not supported!");
    ARC_ASSERT_MSG(!m_writeId || m_writeId != id, "Attempted to recursively hold a write lock! This is not supported!");
  }
#endif
  AcquireSRWLockExclusive(&m_lock);
#if _DEBUG
  {
    lock_guard<mutex> guard(m_syncLock);
    auto id = this_thread::get_id();
    ARC_ASSERT_MSG(!m_writeId, "Cannot acquire a lock while someone else has it.");

    m_writeId = id;
  }
#endif
}

void Util::SlimRWLock::UnlockExclusive() {
#if _DEBUG
  {
    lock_guard<mutex> guard(m_syncLock);
    auto id = this_thread::get_id();

    ARC_ASSERT(m_writeId);
    ARC_ASSERT_MSG(m_writeId == id, "Attempted to release a write lock from a thread that does not hold a write lock!");
    m_writeId = boost::none;
  }
#endif

  ReleaseSRWLockExclusive(&m_lock);
}

Util::WriteLock::WriteLock(SlimRWLock* lock)
    : ScopeGuard([lock]() { lock->UnlockExclusive(); }) {
  lock->LockExclusive();
}

Util::WriteLock::WriteLock(SRWLOCK* lock)
    : ScopeGuard([lock]() { ReleaseSRWLockExclusive(lock); }) {
  AcquireSRWLockExclusive(lock);
}

Util::WriteLock::~WriteLock() {}
