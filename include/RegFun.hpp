#pragma once
#ifndef __REGFUN_H___
#define __REGFUN_H___

#include <atlsecurity.h>

class RegFun
{
	typedef struct _UNICODE_STRING
	{
		USHORT Length;
		USHORT MaximumLength;
		PWSTR Buffer;
	} UNICODE_STRING, *PUNICODE_STRING;
	typedef ULONG(NTAPI * FnRtlNtStatusToDosError)(LONG Status);
	typedef LONG(NTAPI * FnNtRenameKey)(HANDLE hKey, PUNICODE_STRING sNewName);
	typedef VOID(NTAPI * FnRtlInitUnicodeString)(PUNICODE_STRING DestString, PCWSTR SourceString);
public:
	static DWORD Rename(HKEY hRootKey, const wchar_t* lpSubKey, const wchar_t* lpNewName)
	{
		HMODULE hModule = ::GetModuleHandleW(L"NTDLL.DLL");
		if (NULL == hModule)
			return GetLastError();


		auto fnRename = (FnNtRenameKey)::GetProcAddress(hModule, "NtRenameKey");
		auto fnStatusError = (FnRtlNtStatusToDosError)::GetProcAddress(hModule, "RtlNtStatusToDosError");
		auto fnInitUnicodeStr = (FnRtlInitUnicodeString)::GetProcAddress(hModule, "RtlInitUnicodeString");
				
		HKEY hKey;
		auto lRet = RegOpenKeyExW(hRootKey, lpSubKey, 0, KEY_WRITE, &hKey);
		if (ERROR_SUCCESS != lRet)
			return lRet;
		
		UNICODE_STRING uNewName;
		fnInitUnicodeStr(&uNewName, lpNewName);
		LONG l = fnRename(hKey, &uNewName);
		::RegCloseKey(hKey);
		if (NO_ERROR != l)
		{
			return fnStatusError(l);
		}
		
		return 0;
	}


	static DWORD RegSetAllowed(const wchar_t* lpPathAcc)
	{		
		PSID pSidOwner = (PSID)Sids::Admins().GetPSID();
		DWORD dwRet = SetRegSecurityInfo((LPWSTR)lpPathAcc,
			OWNER_SECURITY_INFORMATION,
			pSidOwner, NULL);
	
		CDacl aDacl;
		//aDacl.AddDeniedAce(Sids::Admins(), GENERIC_WRITE);
		aDacl.AddAllowedAce(Sids::Admins(), GENERIC_ALL);
		//aDacl.AddAllowedAce(Sids::Network(), GENERIC_READ);

		return SetRegSecurityInfo((LPWSTR)lpPathAcc,
			DACL_SECURITY_INFORMATION,
			NULL, (PACL)aDacl.GetPACL());
	}

	static DWORD RegSetDenied(const wchar_t* lpPathAcc)
	{
		CDacl aDacl;
		aDacl.AddDeniedAce(Sids::Admins(), GENERIC_WRITE);
	
		return SetRegSecurityInfo((LPWSTR)lpPathAcc,
			DACL_SECURITY_INFORMATION,
			NULL, (PACL)aDacl.GetPACL());
	}

	typedef DWORD(WINAPI * FnTreeSetNamedSecurityInfoW)(LPWSTR pObjectName, 
		SE_OBJECT_TYPE ObjectType, 
		SECURITY_INFORMATION SecurityInfo, 
		PSID pOwner, 
		PSID pGroup, 
		PACL pDacl,
		PACL pSacl,
		DWORD dwAction, 
		FN_PROGRESS fnProgress,
		PROG_INVOKE_SETTING ProgressInvokeSetting,
		PVOID Args);
	static DWORD SetRegSecurityInfo(LPWSTR lpPathAcc, SECURITY_INFORMATION SecurityInfo, PSID pSidOwner, PACL pDacl)
	{
		//TreeSetNamedSecurityInfo xp不支持这个方法 所以采用这种调用方式
		HMODULE hModule = GetModuleHandleW(L"advapi32");
		if (NULL != hModule)
		{
			auto fnTreeSetSe = (FnTreeSetNamedSecurityInfoW)GetProcAddress(hModule, "TreeSetNamedSecurityInfoW");
			if (nullptr != fnTreeSetSe)
			{
				return fnTreeSetSe(lpPathAcc, SE_REGISTRY_KEY, SecurityInfo, pSidOwner, NULL,
					pDacl, NULL, TREE_SEC_INFO_SET, NULL, ProgressInvokeNever, NULL);
			}
		}
		
		return SetNamedSecurityInfoW((LPWSTR)lpPathAcc,
			SE_REGISTRY_KEY,
			SecurityInfo,
			pSidOwner, NULL, pDacl, NULL);
	}
};

#endif		//__REGFUN_H___