#ifndef __ADJUST_PRIVILEGES_H___
#define __ADJUST_PRIVILEGES_H___
#pragma once
#include <unordered_set>


class AdjustPrivilegesOp
{
public:
	static bool Enable(const wchar_t* lpPrivi, bool bEnable)
	{
		TOKEN_PRIVILEGES tokenPrivilege;
		HANDLE hToken = NULL;
		if (!::OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES, &hToken))
		{
			return false;
		}

		tokenPrivilege.PrivilegeCount = 1;
		tokenPrivilege.Privileges[0].Attributes = bEnable ? SE_PRIVILEGE_ENABLED : 0;
		if (!::LookupPrivilegeValue(NULL, lpPrivi, &tokenPrivilege.Privileges[0].Luid))
		{
			CloseHandle(hToken);
			return false;
		}

		if (!::AdjustTokenPrivileges(hToken, FALSE, &tokenPrivilege, sizeof(TOKEN_PRIVILEGES), NULL, NULL))
		{
			CloseHandle(hToken);
			return false;
		}

		if (ERROR_NOT_ALL_ASSIGNED == GetLastError())
		{
			CloseHandle(hToken);
			return false;
		}

		CloseHandle(hToken);
		return true;
	}
	
	static bool Enable(const std::unordered_set<std::wstring>& setName, bool bEnable)
	{
		int nSize = setName.size();
		if (0 == nSize) return false;

		HANDLE hToken = NULL;
		if (!::OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES, &hToken))
		{
			return false;
		}

		auto fnSet = [](HANDLE hToken, LPCTSTR lpPrivilege, bool bEnable) ->bool
		{
			TOKEN_PRIVILEGES tp;
			LUID luid;

			if (!::LookupPrivilegeValue(NULL, lpPrivilege, &luid))
			{
				return false;
			}

			tp.PrivilegeCount = 1;
			tp.Privileges[0].Luid = luid;
			tp.Privileges[0].Attributes = bEnable ? SE_PRIVILEGE_ENABLED : 0;

			// Enable the privilege or disable all privileges.
			if (!::AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), (TOKEN_PRIVILEGES*)NULL, NULL))
			{
				return false;
			}

			return ERROR_NOT_ALL_ASSIGNED != GetLastError();
		};

		for each(auto& nam in setName)
		{
			fnSet(hToken, nam.c_str(), true);
		}
		
		CloseHandle(hToken);
		

		return true;
	}
};


#endif		