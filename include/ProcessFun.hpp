#ifndef __PROCESS_H___
#define __PROCESS_H___
#pragma once
#include <Tlhelp32.h>
#include <Psapi.h>
#pragma comment(lib, "Psapi.lib")

#include <list>

class ProcessFun
{
public:

	//************************************
	// Method:    根据 进程名称 获取 进程 id
	//************************************
	static DWORD GetProcessIdByName(const wchar_t* lpProcessName)
	{	
		DWORD dwProcessId = 0;
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

	static std::list<DWORD> GetIdList(const wchar_t* lpProcName)
	{
		PROCESSENTRY32W pe;
		pe.dwSize = sizeof(PROCESSENTRY32W);

		std::list<DWORD> re;
		HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (TRUE == Process32FirstW(hSnapshot, &pe))
		{
			do
			{
				pe.dwSize = sizeof(PROCESSENTRY32);
				if (FALSE == Process32Next(hSnapshot, &pe))
					break;

				if (0 == lstrcmpi(lpProcName, pe.szExeFile))
				{
					re.push_back(pe.th32ProcessID);
				}
			} while (NULL != hSnapshot);
		}

		CloseHandle(hSnapshot);
		return re;
	}

	// 这个 需要 权限
	static bool HasModule(DWORD dwProcId, const wchar_t* lpModuleName)
	{
		if (0 == dwProcId) return false;

		HANDLE hSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwProcId);
		if (NULL == hSnap || INVALID_HANDLE_VALUE == hSnap) return false;

		MODULEENTRY32W m32;
		m32.dwSize = sizeof(m32);
		if (!::Module32FirstW(hSnap, &m32))
		{
			::CloseHandle(hSnap);
			return false;
		}

		return false;
	}

	// 创建 进程 且 等待 进程 结束
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

	// 异步 运行 进程
	// lpCmd 包含 进程 + arg 
	// dwFlags 是CreateProcessW 中的参数	REALTIME_PRIORITY_CLASS 最高优先级	CREATE_NO_WINDOW 
	// bAsFather 是否 继承 父进程 的属性
	static bool RunProcessAsync(LPCTSTR lpCmd, DWORD dwFlags=0, BOOL bAsFather=FALSE)
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
			bAsFather,
			dwFlags,// CREATE_NO_WINDOW,//为新进程创建一个新的控制台窗口  
			NULL,
			NULL,
			&startupInfo,
			&processInfo))
		{
			return false;
		}

		CloseHandle(processInfo.hThread);
		CloseHandle(processInfo.hProcess);

		return true;
	}

	// 根据进程Id 获取 进程的所属 用户 
	// return 大于0 表示错误代码。
	static DWORD GetProcUserName(DWORD dwProcId, std::wstring& sUserName)
	{
		HANDLE hProc = ::OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwProcId);
		try
		{			
			if (NULL == hProc)
			{
				DWORD dwError = GetLastError();
				if (ERROR_ACCESS_DENIED != dwError)
				{
					return dwError;
				}
				hProc = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcId);
				if (NULL == hProc)
					return GetLastError();
			}

			HANDLE hToken = NULL;
			::OpenProcessToken(hProc, TOKEN_QUERY, &hToken);
			if (NULL == hToken)
				throw 0;

			DWORD dwLen = 0;
			::GetTokenInformation(hToken, TokenUser, NULL, 0, &dwLen);
			if (0 == dwLen)
				throw 0;

			PTOKEN_USER pTokenUser = (PTOKEN_USER)malloc(dwLen);
			::GetTokenInformation(hToken, TokenUser, pTokenUser, dwLen, &dwLen);

			SID_NAME_USE sidNameUse;
			wchar_t cUser[MAX_PATH] = {0};
			wchar_t cDomain[MAX_PATH] = {0};
			DWORD dwUser = MAX_PATH;
			DWORD dwDomain = MAX_PATH;
			BOOL bRet = LookupAccountSidW(NULL, pTokenUser->User.Sid, cUser, &dwUser,
				cDomain, &dwDomain, &sidNameUse);
				
			free(pTokenUser); pTokenUser = nullptr;

			if(!bRet)
				throw 0;

			sUserName = cUser;

			CloseHandle(hProc);
			return ERROR_SUCCESS;
		}
		catch (...)
		{
			if (NULL != hProc)
			{
				CloseHandle(hProc);
				hProc = NULL;
			}
		}
		return GetLastError();
	}

	// 判断 进程 是否 是 系统 进程 
	static bool IsSystemProcess(DWORD dwProcId)
	{
		std::wstring sName;
		DWORD dwError = GetProcUserName(dwProcId, sName);

		// 用户进程不会没有权限的, 应该是系统进程, PASS
		if (ERROR_ACCESS_DENIED == dwError)
			return true;

		if (0 == sName.compare(L"SYSTEM") ||
			0 == sName.compare(L"LOCAL SERVICE") ||
			0 == sName.compare(L"NETWORK SERVICE"))
		{
			return true;
		}

		return false;
	}

	// 根据进程Id 获取 进程路径
	static DWORD GetProcPathById(DWORD dwProcId, std::wstring& sProcPath)
	{
		HANDLE hProc = ::OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ, FALSE, dwProcId);
		if (NULL == hProc)
		{
			return GetLastError();
		}
		try
		{
			// EnumProcessModules 32 位 只能获取 32位进程 64只能64
			HMODULE hModule = NULL;
			DWORD dwNeeded = 1024;
			if (::EnumProcessModules(hProc, &hModule, sizeof(hModule), &dwNeeded))
			{
				wchar_t cPath[MAX_PATH] = {0};
				if (GetModuleFileNameEx(hProc, hModule, cPath, MAX_PATH))
				{
					sProcPath = cPath;
				}
			}
			else 
			{
				wchar_t cImageFile[MAX_PATH] = {0};
				if (!::GetProcessImageFileNameW(hProc, cImageFile, MAX_PATH))
					throw 0;

				wchar_t cDriveArr[512] = {0};
				wchar_t cDevName[128];
				DWORD dwLen = GetLogicalDriveStringsW(512, cDriveArr);
				for (DWORD i = 0; i < dwLen; i += 4)
				{
					if ('A' == cDriveArr[i] ||
						'B' == cDriveArr[i])
						continue;

					wchar_t cDisk[3] = {cDriveArr[i], cDriveArr[i + 1], '\0'};
					cDevName[0] = '\0';
					QueryDosDeviceW(cDisk, cDevName, 128);

					size_t aLens = wcslen(cDevName);
					if (0 == _wcsnicmp(cImageFile, cDevName, aLens))
					{
						sProcPath = cDisk;
						sProcPath += (cImageFile + aLens);
						break;
					}
				}
			}
		
			CloseHandle(hProc);
			hProc = NULL;

			return ERROR_SUCCESS;
		}
		catch (...)
		{
			if (NULL != hProc)
			{
				CloseHandle(hProc);
				hProc = NULL;
			}
		}
		return GetLastError();		
	}

	// 判断进程 是否 是 64位 
	static bool IsProc64Bit(DWORD dwProcId)
	{
		bool b64System = Is64System();
		if (!b64System)			// 如果 不是 64位系统  那就 不可能有 64位 进程
			return false;

		HMODULE hModule = ::GetModuleHandle(L"Kernel32.dll");
		if (NULL == hModule) return false;

		typedef BOOL(WINAPI * FnIsWow64Process)(HANDLE hProcess, BOOL* pResult);

		FnIsWow64Process fnIsWow64 = (FnIsWow64Process)::GetProcAddress(hModule, "IsWow64Process");
		if (nullptr == fnIsWow64) return false;

		BOOL bWow64 = TRUE;
		HANDLE hProc = ::OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwProcId);
		if (NULL == hProc) return false;

		fnIsWow64(hProc, &bWow64);
		CloseHandle(hProc);

		return FALSE == bWow64;
	}

	// 判断 系统 是否是 64位
	static bool Is64System()
	{
		// 程序运行时  不可能 还 能 变更 系统位数吧  所以 这里 直接取值
		static int gs_nSystemBit = 0;		// 系统 位数  0 初始化		32, 64		
		if (gs_nSystemBit > 0)
		{
			return 64 == gs_nSystemBit;
		}
		
		typedef VOID(WINAPI* FN_GetNativeSystemInfo)(SYSTEM_INFO* lpSystemInfo);
		SYSTEM_INFO sysInfo;
		FN_GetNativeSystemInfo fGetNativeSysInfo =	(FN_GetNativeSystemInfo)GetProcAddress(GetModuleHandleW(L"kernel32"), "GetNativeSystemInfo");
		if (nullptr != fGetNativeSysInfo)
		{
			fGetNativeSysInfo(&sysInfo);
		}
		else
		{
			GetSystemInfo(&sysInfo);
		}

		if (sysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ||
			sysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64)
		{
			gs_nSystemBit = 64;
		}
		else
		{
			gs_nSystemBit = 32;
		}

		return 64 == gs_nSystemBit;
	}

	//static int ms_nSystemBit;				// 
};
//int ProcessFun::ms_nSystemBit = 0;




#endif