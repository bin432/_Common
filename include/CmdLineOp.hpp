#ifndef __COMLINE_H___
#define __COMLINE_H___
#pragma once
/* 命令行 处理 
规则：
	-del    - 开头的 表示 flags 后面 都 不带参数				这个没有重复 信息

	每个参数都要要 一个字节 的头  如    i4032	->  i是标志  4032是信息


*/

#include <vector>
#include <map>
#include <set>
#include <string>
#include <ShellAPI.h>

class CmdLineOp
{
public:
	static int CmdLineToArg(LPCTSTR lpCmdLine, std::vector<std::wstring>& argList)
	{
		int nArg = 0;
		LPWSTR* lpArg = CommandLineToArgvW(lpCmdLine, &nArg);
		if (nullptr == lpArg) return 0;

		for (int i = 0; i < nArg; ++i)
		{
			argList.push_back(lpArg[i]);
		}
		LocalFree(lpArg);
		return nArg;
	}
public:
	CmdLineOp(LPCTSTR lpCmdLine)
	{
		if (nullptr == lpCmdLine || 0 == lstrlen(lpCmdLine))
			return ;

		int nArg = 0;
		LPWSTR* lpArg = CommandLineToArgvW(lpCmdLine, &nArg);
		if (nullptr == lpArg) return ;

		for (int i = 0; i < nArg; ++i)
		{
			wchar_t c = lpArg[i][0];
			if ('-' == c)
			{
				m_setFlag.insert(lpArg[i] + 1);
			}
			else
			{
				m_mapArg[c] = lpArg[i] + 1;
			}
		}

		LocalFree(lpArg);		
	}
public:
	bool GetFlag(const wchar_t* lpKey)
	{
		auto ite = m_setFlag.find(lpKey);

		return ite != m_setFlag.end();
	}
	const wchar_t* GetValue(wchar_t cKey, const wchar_t* lpDefault=L"")
	{
		auto ite = m_mapArg.find(cKey);
		if (ite == m_mapArg.end())
			return lpDefault;

		return ite->second.c_str();
	}
protected:
	std::map<wchar_t, std::wstring> m_mapArg;
	std::set<std::wstring> m_setFlag;
};

#endif