/*
异步 任务 处理

*/
#pragma once
#include <thread>
#include <atomic>
#include <list>

template<class Task>
class AsynTask
{
public:
	AsynTask(int nThreadCount=1)
		: m_bQuit(false)
		, m_nMaxThread(nThreadCount)
		, m_nRunThread(0)
	{
		InitializeCriticalSection(&m_csLock);
	}
	virtual ~AsynTask()
	{		
		DeleteCriticalSection(&m_csLock);
	}

	void SetFun(std::function<void(Task)> fun)
	{
		m_TaskFun = fun;
	}

	bool IsDone()
	{
		return m_nRunThread == 0;
	}
public:
	void Add(Task pTask)
	{
		EnterCriticalSection(&m_csLock);
		m_TaskList.push_back(pTask);		
		LeaveCriticalSection(&m_csLock);

		if (m_nRunThread < m_nMaxThread)
		{
			++m_nRunThread;
			std::thread th(&AsynTask::ThreadFun, this);
			th.detach();
		}
	}
	
	void Close(DWORD dwWait=1000)
	{
		m_bQuit = true;
	}
protected:
	void ThreadFun()
	{
		while (!m_bQuit)
		{
			EnterCriticalSection(&m_csLock);
			if (m_TaskList.size() == 0)
			{
				LeaveCriticalSection(&m_csLock);
				break;
			}

			Task task = m_TaskList.front();
			m_TaskList.pop_front();

			LeaveCriticalSection(&m_csLock);

			if (nullptr != m_TaskFun)
				m_TaskFun(task);
		}
		
		--m_nRunThread;
	}

protected:
	volatile bool							m_bQuit;
	CRITICAL_SECTION					m_csLock;					//维持队列同步
	int											m_nMaxThread;
	std::function<void(Task)>		m_TaskFun;
	std::atomic<int> 					m_nRunThread;
	std::list<Task>						m_TaskList;
};