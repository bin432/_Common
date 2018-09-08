#ifndef __BEGINTHREAD_H_INCLUDE_20160727__
#define __BEGINTHREAD_H_INCLUDE_20160727__
#pragma once
#include <winnt.h>
#include <process.h>
#include <functional>			//C++11  ��֧�� vs2008 
/*//////////////////////////////////////////////////////////////////////////
time 2016 07 27 
�߳� ��װ��  ��ֱ��ʹ����ĳ�Ա���� �� �߳� ������
 ����ʹ�� ģ��ķ�ʽ����  
	BeginThread thread;
	thread.Start(&Class::Fun, this);

Ҳ������ C++11��function ����
	BeginThread thread;
	thread.Start(bind(&Class::Fun, this));


���� �� ����  �����Ӱ�  �ȹ���
//////////////////////////////////////////////////////////////////////////*/
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
			m_sErrorText = L"�߳��ѿ�����";
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
			m_sErrorText = L"�߳��ѿ�����";
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

	//�ر��߳�   
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
	//��̬ �߳� ���� 
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
	bool					m_bAutoRelease;					//�Զ��ͷ�
	std::function<UINT()> m_threadFun;

	std::wstring		m_sErrorText;
};



class BeginThreadEx
{
public:
	BeginThreadEx()
		: m_hThread(NULL)
		, m_dwThreadId(0)
		//, m_threadFun(nullptr)
		, m_funInfo(NULL)
	{
	}
	~BeginThreadEx()
	{
		Release();
		if (NULL  != m_funInfo)
		{
			delete m_funInfo;
			m_funInfo = NULL;
		}
	}
public:
	bool Start(std::function<void(LPARAM)> threadFun, LPARAM lParam)
	{
		if(IsWorking())
		{
			m_sErrorText = L"�߳��ѿ�����";
			return false;
		}

		m_funInfo = new FunInfo;
		m_funInfo->threadFun = threadFun;
		m_funInfo->lParam = lParam;

		//m_threadFun = threadFun;
		//m_lParam = lParam;
		m_hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadFun, this, NULL, &m_dwThreadId);
		return NULL != m_hThread;
	}

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

	//�ر��߳�   
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
	unsigned int Do()
	{
		m_funInfo->threadFun(m_funInfo->lParam);
		
		Release();

		return 0;
	}
public:
	//��̬ �߳� ���� 
	static unsigned int __stdcall ThreadFun(void* lp)
	{
		BeginThreadEx* pThread = (BeginThreadEx*)lp;
		if(NULL == pThread)
			return 0;
		
		return pThread->Do();
	}

	struct FunInfo
	{
		std::function<void(LPARAM)>			threadFun;
		LPARAM lParam;
	};
private:
	HANDLE				m_hThread;
	UINT					m_dwThreadId;
	std::wstring		m_sErrorText;

	FunInfo*			m_funInfo;
};

//////////////////////////////////////////////////////////////////////////
#endif // __BEGINTHREAD_H_INCLUDE_20160727__




//  vs2008 д��  
#if 0

//������ģ���д��   ���� ���� ��ģ��  
//	���ӣ�
//	CBeginThread<CMainDlg> beginThread;
//	beginThead.Start(this, &CMainDlg::Fun); 

template<class Type>
class CBeginThread
{
public:
	CBeginThread()
		: m_hThread(NULL)
		, m_pT(NULL)
		, m_pFun(NULL)
	{
	}

	virtual ~CBeginThread()
	{
		Release();
	}
	
	typedef unsigned int (Type::*pFun)();

	//���������߳�    ���ָ�� �� ����ָ��
	bool Start(Type* pT, pFun p)
	{
		if(NULL != m_hThread)
			return false;

		if(NULL == pT || NULL == p)
			return false;

		m_pT = pT;
		m_pFun = p;

		m_hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadFun, this, NULL, 0);
		if(NULL == m_hThread)
			return false;

		return true;
	}

	//�ر��߳�   
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

	//�ж��߳� 
	bool IsWorking()
	{
		if(NULL == m_hThread)
			return false;

		DWORD dwWait = ::WaitForSingleObject(m_hThread, 0);
		if(WAIT_OBJECT_0 == dwWait)
			return false;
		else if(WAIT_TIMEOUT == dwWait)
			return true;

		return false;
	}

	//�ر� ��� 
	void Release()
	{
		if(NULL != m_hThread)
		{
			CloseHandle(m_hThread);
			m_hThread = NULL;
		}
	}


	//�߳� ����
	unsigned int OnFun()
	{
		if(NULL != m_pT)
		{
			return (m_pT->*m_pFun)();
		}
		return 0;
	}
public:
	//��̬ �߳� ���� 
	static unsigned int __stdcall ThreadFun(void* lp)
	{
		CBeginThread* pThread = (CBeginThread*)lp;
		if(NULL == pThread)
			return 0;

		unsigned int nRet = pThread->OnFun();
		pThread->Release();
		return nRet;
	}
private:
	HANDLE m_hThread;
	Type*				m_pT;
	pFun					m_pFun;
};
#endif