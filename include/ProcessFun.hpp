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
	// Method:    ���� �������� ��ȡ ���� id
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

	// ��� ��Ҫ Ȩ��
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

	// ���� ���� �� �ȴ� ���� ����
	static bool CreateAndWaitProcess(LPCTSTR lpCmd, DWORD& dwExitCode)
	{
		//�½�����
		STARTUPINFO startupInfo;							//����������Ϣ
		ZeroMemory(&startupInfo, sizeof(startupInfo));
		startupInfo.cb = sizeof(startupInfo);

		PROCESS_INFORMATION processInfo;			//������Ϣ
		ZeroMemory(&processInfo, sizeof(processInfo));

		if (!CreateProcessW(NULL,
			(LPTSTR)lpCmd,		//
			NULL,
			NULL,
			FALSE,
			CREATE_NO_WINDOW,//Ϊ�½��̴���һ���µĿ���̨����  
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

	// �첽 ���� ����
	// lpCmd ���� ���� + arg 
	// dwFlags ��CreateProcessW �еĲ���	REALTIME_PRIORITY_CLASS ������ȼ�	CREATE_NO_WINDOW 
	// bAsFather �Ƿ� �̳� ������ ������
	static bool RunProcessAsync(LPCTSTR lpCmd, DWORD dwFlags=0, BOOL bAsFather=FALSE)
	{
		//�½�����
		STARTUPINFO startupInfo;							//����������Ϣ
		ZeroMemory(&startupInfo, sizeof(startupInfo));
		startupInfo.cb = sizeof(startupInfo);

		PROCESS_INFORMATION processInfo;			//������Ϣ
		ZeroMemory(&processInfo, sizeof(processInfo));

		if (!CreateProcessW(NULL,
			(LPTSTR)lpCmd,		//
			NULL,
			NULL,
			bAsFather,
			dwFlags,// CREATE_NO_WINDOW,//Ϊ�½��̴���һ���µĿ���̨����  
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

	// ���ݽ���Id ��ȡ ���̵����� �û� 
	// return ����0 ��ʾ������롣
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

	// �ж� ���� �Ƿ� �� ϵͳ ���� 
	static bool IsSystemProcess(DWORD dwProcId)
	{
		std::wstring sName;
		DWORD dwError = GetProcUserName(dwProcId, sName);

		// �û����̲���û��Ȩ�޵�, Ӧ����ϵͳ����, PASS
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

	// ���ݽ���Id ��ȡ ����·��
	static DWORD GetProcPathById(DWORD dwProcId, std::wstring& sProcPath)
	{
		HANDLE hProc = ::OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ, FALSE, dwProcId);
		if (NULL == hProc)
		{
			return GetLastError();
		}
		try
		{
			// EnumProcessModules 32 λ ֻ�ܻ�ȡ 32λ���� 64ֻ��64
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

	// �жϽ��� �Ƿ� �� 64λ 
	static bool IsProc64Bit(DWORD dwProcId)
	{
		bool b64System = Is64System();
		if (!b64System)			// ��� ���� 64λϵͳ  �Ǿ� �������� 64λ ����
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

	// �ж� ϵͳ �Ƿ��� 64λ
	static bool Is64System()
	{
		// ��������ʱ  ������ �� �� ��� ϵͳλ����  ���� ���� ֱ��ȡֵ
		static int gs_nSystemBit = 0;		// ϵͳ λ��  0 ��ʼ��		32, 64		
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