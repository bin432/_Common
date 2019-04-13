#ifndef __WIN_SERVICE_T__H_
#define __WIN_SERVICE_T__H_
#pragma once

#include <assert.h>
//#include <functional>
//#include "OutputDebugOp.hpp"


// 封装了 windows 服务的 基本方法
class WinService
{
public:

	// 创建 服务
	static bool Create(LPCTSTR lpServiceName, LPCTSTR lpDisplayName, LPCTSTR lpServiceDesc, LPCTSTR lpBinaryPath = nullptr)
	{
		bool bRet = false;
		TCHAR* lpFilePath = nullptr;
		SC_HANDLE hSCM = NULL;
		SC_HANDLE hService = NULL;
		try
		{
			hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
			if (NULL == hSCM)
			{
				return false;
			}

			// Get the executable file path			
			if (nullptr == lpBinaryPath)
			{
				lpFilePath = new TCHAR[MAX_PATH]();
				::GetModuleFileName(NULL, lpFilePath, MAX_PATH);
				lpBinaryPath = lpFilePath;
			}

			hService = ::CreateService(hSCM,
				lpServiceName,
				lpDisplayName,
				SERVICE_ALL_ACCESS,
				SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS,
				SERVICE_AUTO_START,
				SERVICE_ERROR_NORMAL,
				lpBinaryPath,
				NULL,
				NULL,
				NULL,
				NULL,
				NULL);
			if (NULL == hService)
			{
				DWORD dwErr = GetLastError();
				if (ERROR_SERVICE_EXISTS == dwErr)
					bRet = true;

				throw 0;
			}

			SERVICE_DESCRIPTION sd;
			sd.lpDescription = (LPWSTR)lpServiceDesc;

			ChangeServiceConfig2(hService, SERVICE_CONFIG_DESCRIPTION, &sd);
			bRet = true;
		}
		catch (...)
		{

		}

		if (nullptr != lpFilePath)
		{
			delete[] lpFilePath;
			lpFilePath = nullptr;
		}

		if (NULL != hService)
		{
			CloseServiceHandle(hService);
		}

		if (NULL != hSCM)
		{
			CloseServiceHandle(hSCM);
		}

		return bRet;
	}
	// 启动 服务
	static bool Start(LPCTSTR lpServiceName)
	{
		bool bRet = false;
		SC_HANDLE hSCM = NULL;
		SC_HANDLE hService = NULL;
		try
		{
			hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
			if (NULL == hSCM)
			{
				return false;
			}
			hService = ::OpenService(hSCM, lpServiceName, SERVICE_START | SERVICE_QUERY_STATUS);
			if (NULL == hService)
			{
				throw 0;
			}

			SERVICE_STATUS sc_Status;
			if (FALSE == ::QueryServiceStatus(hService, &sc_Status))
			{
				throw 0;
			}

			if (SERVICE_RUNNING == sc_Status.dwCurrentState ||
				SERVICE_START_PENDING == sc_Status.dwCurrentState ||
				SERVICE_CONTINUE_PENDING == sc_Status.dwCurrentState)
			{
				bRet = true;
				throw 1;
			}

			bRet = (TRUE == ::StartService(hService, NULL, NULL));
		}
		catch (...)
		{

		}

		if (NULL != hService)
		{
			CloseServiceHandle(hService);
		}

		if (NULL != hSCM)
		{
			CloseServiceHandle(hSCM);
		}

		return bRet;
	}
	// 停止 服务
	static bool Stop(LPCTSTR lpServiceName)
	{
		bool bRet = false;
		SC_HANDLE hSCM = NULL;
		SC_HANDLE hService = NULL;
		try
		{
			hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
			if (NULL == hSCM)
			{
				return false;
			}
			hService = ::OpenService(hSCM, lpServiceName, SERVICE_STOP | SERVICE_QUERY_STATUS);
			if (NULL == hService)
			{
				throw 0;
			}

			SERVICE_STATUS sc_Status;
			if (FALSE == ::QueryServiceStatus(hService, &sc_Status))
			{
				throw 0;
			}

			if (SERVICE_STOPPED == sc_Status.dwCurrentState ||
				SERVICE_STOP_PENDING == sc_Status.dwCurrentState)
			{
				bRet = true;
				throw 0;
			}


			bRet = (TRUE == ::ControlService(hService, SERVICE_CONTROL_STOP, &sc_Status));
		}
		catch (...)
		{

		}

		if (NULL != hService)
		{
			CloseServiceHandle(hService);
		}

		if (NULL != hSCM)
		{
			CloseServiceHandle(hSCM);
		}

		return bRet;
	}
	// 删除 服务
	static bool Delete(LPCTSTR lpServiceName)
	{
		bool bRet = false;
		SC_HANDLE hSCM = NULL;
		SC_HANDLE hService = NULL;
		try
		{
			hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
			if (NULL == hSCM)
			{
				return false;
			}
			hService = ::OpenService(hSCM, lpServiceName, SERVICE_STOP | DELETE | SERVICE_QUERY_STATUS);
			if (NULL == hService)
			{
				throw 0;
			}

			SERVICE_STATUS sc_Status;
			if (FALSE == ::QueryServiceStatus(hService, &sc_Status))
			{
				throw 1;
			}
			if (SERVICE_RUNNING == sc_Status.dwCurrentState)
			{
				::ControlService(hService, SERVICE_CONTROL_STOP, &sc_Status);
			}

			bRet = (TRUE == ::DeleteService(hService));
		}
		catch (...)
		{

		}

		if (NULL != hService)
		{
			CloseServiceHandle(hService);
		}

		if (NULL != hSCM)
		{
			CloseServiceHandle(hSCM);
		}

		return bRet;
	}

};


// windows 服务 模版 
// 单例 服务模式 需要 重载 控制方法 主程序方法。 还要 静态 方法 T::GetSvcName()
// 这个单 服务  多服务 还没 研究 也没这个需求。
template<class T>
class ServiceFramework
{
public:
	ServiceFramework()
	{
		assert(nullptr == ms_Singleton);

		ms_Singleton = static_cast<T*>(this);

		m_hServiceStatus = NULL;
		m_status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
		m_status.dwCurrentState = SERVICE_STOPPED;
		m_status.dwControlsAccepted = SERVICE_ACCEPT_STOP;
		m_status.dwWin32ExitCode = 0;
		m_status.dwServiceSpecificExitCode = 0;
		m_status.dwCheckPoint = 0;		// 用于报告冗长操作期间的进度
		m_status.dwWaitHint = 0;	// 估计待处理操作所需的时间，以毫秒为单位
	}
	~ServiceFramework()
	{
		ms_Singleton = nullptr;
	}
private:
	ServiceFramework& operator=(const ServiceFramework&);
	ServiceFramework(const ServiceFramework&);
public:
	//服务 控制接口
	virtual DWORD SvcStop() 
	{
		SetStatus(SERVICE_STOP);
		return NO_ERROR;
	}
	virtual DWORD SvcPause()
	{ 
		SetStatus(SERVICE_PAUSE_CONTINUE); 
		return NO_ERROR;
	}
	virtual DWORD SvcContinue() 
	{
		SetStatus(SERVICE_PAUSE_CONTINUE); 
		return NO_ERROR;
	}
	virtual DWORD SvcInterrogate()
	{
		SetStatus(SERVICE_INTERROGATE); 
		return NO_ERROR;
	}
	virtual DWORD SvcShutdown() { return NO_ERROR; };
	virtual DWORD SvcOtherEx(DWORD dwControl, DWORD dwEventType, LPVOID lpEventData) { return NO_ERROR; };
	virtual DWORD SvcDoRun() = 0;			// 主程序 return exitcode
public:
	bool Serve()				// 启动
	{
		wchar_t* lpSvcName = (wchar_t*)T::GetSvcName();
		SERVICE_TABLE_ENTRY st[] =
		{
			{lpSvcName, _ServiceProc},
			{ NULL, NULL }
		};

		return TRUE == ::StartServiceCtrlDispatcherW(st);
	}
protected:
	bool SetStatus(DWORD dwState)
	{
		m_status.dwCurrentState = dwState;
		return TRUE == ::SetServiceStatus(m_hServiceStatus, &m_status);
	}
	//服务器主程序
	void SvcRun(DWORD dwArgc, LPTSTR* lpszArgv)
	{
		m_hServiceStatus = ::RegisterServiceCtrlHandlerEx(T::GetSvcName(), _Handler, nullptr);
		if (NULL == m_hServiceStatus)
		{
			return ;
		}

		SetStatus(SERVICE_START_PENDING);

		m_status.dwWin32ExitCode = S_OK;
		m_status.dwCheckPoint = 0;
		m_status.dwWaitHint = 0;

		SetStatus(SERVICE_RUNNING);
		
		m_status.dwWin32ExitCode = SvcDoRun();

		SetStatus(SERVICE_STOPPED);
	}
protected:	
	static void WINAPI _ServiceProc(DWORD dwArgc, LPTSTR* lpszArgv)
	{
		ms_Singleton->SvcRun(dwArgc, lpszArgv);
	}
	static DWORD WINAPI _Handler(DWORD dwControl,DWORD dwEventType,LPVOID lpEventData,LPVOID lpContext)
	{
		if (nullptr == ms_Singleton) return ERROR_CALL_NOT_IMPLEMENTED;

		if (dwControl == SERVICE_CONTROL_STOP)
			return ms_Singleton->SvcStop();
		else if (dwControl == SERVICE_CONTROL_PAUSE)
			return ms_Singleton->SvcPause();
		else if (dwControl == SERVICE_CONTROL_CONTINUE)
			return ms_Singleton->SvcContinue();
		else if (dwControl == SERVICE_CONTROL_INTERROGATE)
			return ms_Singleton->SvcInterrogate();
		else if (dwControl == SERVICE_CONTROL_SHUTDOWN)
			return ms_Singleton->SvcShutdown();
		else
			return ms_Singleton->SvcOtherEx(dwControl, dwEventType, lpEventData);
	}
	static T* ms_Singleton;
public:
protected:
	SERVICE_STATUS_HANDLE m_hServiceStatus;
	SERVICE_STATUS					m_status;
};

//WinService* WinService::m_pSingleton = nullptr;





#endif				//__WIN_SERVICE_T__H_