#ifndef __PROCESS_H___
#define __PROCESS_H___
#pragma once
#include <Tlhelp32.h>

class ProcessFun
{
public:

	//************************************
	// Method:    根据 进程名称 获取 进程 id
	//************************************
	static unsigned long GetProcessIdByName(const wchar_t* lpProcessName)
	{	
		unsigned long dwProcessId = 0;
		PROCESSENTRY32 pe;
		pe.dwSize = sizeof(PROCESSENTRY32);

		HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (Process32First(hSnapshot, &pe))
		{
			do
			{
				pe.dwSize = sizeof(PROCESSENTRY32);
				if (FALSE == Process32Next(hSnapshot, &pe))
					break;

				if (0 == lstrcmpi(lpProcessName, pe.szExeFile))
				{
					dwProcessId = pe.th32ProcessID;
					break;
				}
			} while (NULL != hSnapshot);
		}

		CloseHandle(hSnapshot);
		return dwProcessId;
	}

	static bool CreateAndWaitProcess(LPCTSTR lpCmd, DWORD& dwExitCode)
	{
		//新建进程
		STARTUPINFO startupInfo;							//进程启动信息
		ZeroMemory(&startupInfo, sizeof(startupInfo));
		startupInfo.cb = sizeof(startupInfo);

		PROCESS_INFORMATION processInfo;			//进程信息
		ZeroMemory(&processInfo, sizeof(processInfo));

		if (!CreateProcessW(NULL,
			(LPTSTR)lpCmd,		//
			NULL,
			NULL,
			FALSE,
			CREATE_NO_WINDOW,//为新进程创建一个新的控制台窗口  
			NULL,
			NULL,
			&startupInfo,
			&processInfo))
		{
			dwExitCode = GetLastError();
			return false;
		}

		CloseHandle(processInfo.hThread);
		WaitForSingleObject(processInfo.hProcess, INFINITE);

		if (!GetExitCodeProcess(processInfo.hProcess, &dwExitCode))
		{
			dwExitCode = GetLastError();
			CloseHandle(processInfo.hProcess);
			return false;
		}
		CloseHandle(processInfo.hProcess);

		return true;
	}
};




#endif