#pragma once
#ifndef __CSLOCK_H___
#define __CSLOCK_H___

class CSLock
{
public:
	CSLock(DWORD dwSpinCount = 4096)
	{
		::InitializeCriticalSectionAndSpinCount(&m_CS, dwSpinCount);
	}
	~CSLock()
	{
		::DeleteCriticalSection(&m_CS);
	}

	void Lock()
	{
		::EnterCriticalSection(&m_CS);
	}
	void Unlock()
	{
		::LeaveCriticalSection(&m_CS);
	}
	bool TryLock()
	{
		return TRUE == ::TryEnterCriticalSection(&m_CS);
	}
	DWORD SetSpinCount(DWORD dwSpinCount)
	{
		return ::SetCriticalSectionSpinCount(&m_CS, dwSpinCount);
	}
private:
	CSLock(const CSLock& cs);
	CSLock operator = (const CSLock& cs);
private:
	CRITICAL_SECTION m_CS;
};

class AutoCSLock
{
public:
	AutoCSLock(CSLock& cs)
		: m_cs(cs)
	{
		m_cs.Lock();
	}
	~AutoCSLock()
	{
		m_cs.Unlock();
	}
protected:
	CSLock& m_cs;
};

#endif //__CSLOCK_H___