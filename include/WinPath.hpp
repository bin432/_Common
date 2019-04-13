#ifndef __WIN_PATH_H___
#define __WIN_PATH_H___
#pragma once
#include <Shlwapi.h>
#include <ShlObj.h>
#pragma comment(lib, "Shlwapi.lib")

class WinPath
{
public:
	static std::wstring GetAppPath(HMODULE h = NULL, bool hadBackslash = true)
	{
		wchar_t lpPath[MAX_PATH] = {0};
		GetModuleFileNameW(h, lpPath, MAX_PATH);
		PathRemoveFileSpec(lpPath);
		if (hadBackslash)
			PathAddBackslash(lpPath);
		else
			PathRemoveBackslash(lpPath);

		return std::wstring(lpPath);
	}

	// 获取 windows 系统 的 一些特别的 文件夹 路径
	static std::wstring GetSpecialPath(int csidl, bool hadBackslash=true)
	{
		wchar_t lpPath[MAX_PATH] = {0};
		::SHGetSpecialFolderPathW(NULL, lpPath, csidl, FALSE);

		if (hadBackslash)
			PathAddBackslash(lpPath);
		else
			PathRemoveBackslash(lpPath);

		return std::wstring(lpPath);
	}
	
	// Method:    获取 当前用户的用户目录  
	static std::wstring GetCUADPath(bool hadBackslash = true)
	{	
		return GetSpecialPath(CSIDL_LOCAL_APPDATA, hadBackslash);
	}

	// 获取 桌面 路径
	static std::wstring GetDeskTopPath(bool hadBackslash = true)
	{
		return GetSpecialPath(CSIDL_DESKTOP, hadBackslash);
	}
	
	static std::wstring GetSystem32Path(bool hadBackslash = true)
	{
		wchar_t lpPath[MAX_PATH] = {0};
		::GetSystemDirectoryW(lpPath, MAX_PATH);

		if (hadBackslash)
			PathAddBackslash(lpPath);
		else
			PathRemoveBackslash(lpPath);

		return std::wstring(lpPath);
	}
	static std::wstring GetSystemWowPath(bool hadBackslash = true)
	{
		wchar_t lpPath[MAX_PATH] = { 0 };
		::GetSystemWow64DirectoryW(lpPath, MAX_PATH);

		if (hadBackslash)
			PathAddBackslash(lpPath);
		else
			PathRemoveBackslash(lpPath);

		return std::wstring(lpPath);
	}
	// 
	static std::wstring GetDriversPath(bool hadBackslash = true)
	{
		std::wstring sDrivers = GetSystem32Path(true);
		
		sDrivers.append(L"drivers");
		if(hadBackslash)
			sDrivers.append(L"\\");

		return sDrivers;
	}

	// 将 一个 相对路径 或 其他 转换 成一个 合格 的路径
	static std::wstring PathToResolve(const wchar_t* lpPath, bool hadBackslash = true)
	{
		wchar_t buf[MAX_PATH] = { 0 };
		wcscpy_s(buf, MAX_PATH, lpPath);
		if (FALSE == PathResolve(buf, NULL, 0))	//PRF_VERIFYEXISTS
		{
			return L"";
		}

		if (hadBackslash)
			PathAddBackslash(buf);
		else
			PathRemoveBackslash(buf);

		return std::wstring(buf);
	}
};




#endif			//__WIN_PATH_H___