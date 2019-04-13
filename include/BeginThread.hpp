#ifndef __BEGINTHREAD_H_INCLUDE_20160727__
#define __BEGINTHREAD_H_INCLUDE_20160727__
#pragma once
#include <winnt.h>
#include <process.h>
#include <functional>			//C++11  不支持 vs2008 
#include <tuple>
/*//////////////////////////////////////////////////////////////////////////
time 2016 07 27 
线程 封装类  可直接使用类的成员函数 做 线程 函数。
 可以使用 模版的方式调用  
	BeginThread thread;
	thread.Start(&Class::Fun, this);

也可以用 C++11的function 调用
	BeginThread thread;
	thread.Start(bind(&Class::Fun, this));

后续 的 功能  慢慢加吧  先够用*/
//////////////////////////////////////////////////////////////////////////
class BeginThread
{
public:
	BeginThread(bool bAutoRelease=true)
		: m_hThread(NULL)
		, m_dwThreadId(0)
		, m_threadFun(nullptr)
		, m_bAutoRelease(bAutoRelease)
	{
	}
	~BeginThread()
	{
		Release();
	}
public:
	template<class Fun, class T>
	bool Start(Fun fun, T t)
	{
		if(IsWorking())
		{
			m_sErrorText = L"线程已开启！";
			return false;
		}

		m_threadFun = std::bind(fun, t);
		m_hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadFun, this, NULL, &m_dwThreadId);
		return NULL != m_hThread;
	}

	bool Start(std::function<UINT()> threadFun)
	{
		if(IsWorking())
		{
			m_sErrorText = L"线程已开启！";
			return false;
		}

		m_threadFun = threadFun;
		m_hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadFun, this, NULL, &m_dwThreadId);
		return NULL != m_hThread;
	}
	
	/*bool IsValid() const
	{
	return NULL != m_hThread;
	}*/

	bool IsWorking()
	{
		if(NULL == m_hThread)
			return false;

		DWORD dwWait = ::WaitForSingleObject(m_hThread, 0);
		if(WAIT_TIMEOUT == dwWait)
			return true;

		return false;
	}

	UINT GetThreadId()
	{
		return m_dwThreadId;
	}

	//关闭线程   
	bool Terminate(DWORD dwWaitTime=200)
	{
		if (NULL != m_hThread)
		{
			DWORD dwEixtCode = 0;
			if (GetExitCodeThread(m_hThread, &dwEixtCode))
			{
				if(WAIT_OBJECT_0 != ::WaitForSingleObject(m_hThread, dwWaitTime))
				{
					::TerminateThread(m_hThread, 0);
				}
			}

			Release();
		}

		return true;
	}

	void Release()
	{
		if(NULL != m_hThread)
		{
			CloseHandle(m_hThread);
			m_hThread = NULL;
		}
	}

	std::wstring GetErrorText()
	{
		std::wstring s = m_sErrorText;
		m_sErrorText.clear();
		return s;
	}
private:
	//静态 线程 函数 
	static unsigned int __stdcall ThreadFun(void* lp)
	{
		BeginThread* pThread = (BeginThread*)lp;
		if(NULL == pThread || nullptr == pThread->m_threadFun)
			return 0;
		
		unsigned int nRet = pThread->m_threadFun();
		if(pThread->m_bAutoRelease)
			pThread->Release();

		return nRet;
	}

	
private:
	HANDLE				m_hThread;
	UINT					m_dwThreadId;
	bool					m_bAutoRelease;					//自动释放
	std::function<UINT()> m_threadFun;

	std::wstring		m_sErrorText;
};



// 快速 线程 
class ThreadOp
{
public:
	template<class Fun, class T>
	static HANDLE Create(Fun fun, T t)
	{
		auto pFn = new std::function<UINT(void)>(std::move(std::bind(fun, t)));
		HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, _ThreadFun, pFn, NULL, NULL);
		return hThread;
	}
	static HANDLE Create(std::function<UINT(void)> fn)
	{
		auto pFn = new std::function<UINT(void)>(std::move(fn));
		HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, _ThreadFun, pFn, NULL, NULL);
		return hThread;
	}	
protected:
	//静态 线程 函数 
	static unsigned int __stdcall _ThreadFun(void* lp)
	{
		std::function<UINT(void)>* pFn = (std::function<UINT(void)>*)lp;
		if (nullptr != pFn)
		{
			UINT uRet = (*pFn)();

			delete pFn;
			return uRet;
		}
		
		return 0;
	}	
};

// 带参数 的 线程 创建类  vs2010 使用 超过2010 就直接使用 std 的thread 
template<class P>
class ThreadFun
{
	struct TParam
	{
		std::function<UINT(P)> fn;
		P p;
	};
public:
	// 带 一个 tuple  参数 的  线程函数
	template<class Fun, class T>
	static HANDLE Create(Fun fun, T t, P p)
	{
		auto pTP = new TParam;
		pTP->fn = std::move(std::bind(fun, t, std::placeholders::_1));
		pTP->p = p;

		HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, _ThreadFun, pTP, NULL, NULL);
		return hThread;
	}

	static HANDLE Create(std::function<UINT(void)> fn, P p)
	{
		auto pTP = new TParam;
		pTP->fn = std::move(fn);
		pTP->p = p;
		HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, _ThreadFun, pTP, NULL, NULL);
		return hThread;
	}
protected:
	static unsigned int __stdcall _ThreadFun(void* lp)
	{
		auto pV = (TParam*)lp;
		if (nullptr != pV)
		{
			UINT uRet = pV->fn(pV->p);

			delete pV;
			return uRet;
		}

		return 0;
	}
};



#endif // end