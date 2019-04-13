/* at 2018/09/28				v2.1
	这里 是 对 std::string wstring 方法的 一些扩展 
	如果 后续C++ 添加了 这些方法  
	
	//  %I64d
*/

#pragma once
#ifndef __STR_OP_H___
#define __STR_OP_H___
#include <tchar.h>
#include <vector>
#include <stdarg.h>

class StrOp
{
public:
	static bool IsNullOrEmpty(LPCTSTR lpStr)
	{
		return nullptr == lpStr || 0 == _tcslen(lpStr);
	}
	static void MakeUpper(std::wstring& s)
	{
		_wcsupr_s((wchar_t*)s.data(), s.length()+1);
	}
	static void MakeLower(std::wstring& s)
	{
		_wcslwr_s((wchar_t*)s.data(), s.length()+1);
	}
	static int CompareNoCase(std::string& s, LPCSTR lpsz)
	{
		return _stricmp(s.c_str(), lpsz);
	}
	static int CompareNoCase(std::wstring& s, LPCWSTR lpsz)
	{
		return _wcsicmp(s.c_str(), lpsz);
	}
	static std::wstring Format(LPCWSTR format, ...)
	{
		va_list v;
		va_start(v, format);
		int nlen = _vscwprintf(format, v);			// 这里不 +1  因为string 是 不需要 \0 结尾的
		std::wstring s(nlen, nlen);
		int n = vswprintf_s((LPWSTR)s.data(), nlen+1, format, v);
		va_end(v);
		return s;
	}
	static const wchar_t* Format(std::wstring& s, LPCWSTR format, ...)
	{
		va_list v;
		va_start(v, format);
		int nlen = _vscwprintf(format, v);
		s.resize(nlen);
		int n = vswprintf_s((LPWSTR)s.data(), nlen+1, format, v);
		va_end(v);
		return s.c_str();
	}

	static const char* Format(std::string& s, LPCSTR format, ...)
	{
		va_list v;
		va_start(v, format);
		int nlen = _vscprintf(format, v);
		s.resize(nlen);
		int n = vsprintf_s((LPSTR)s.data(), nlen+1, format, v);
		va_end(v);
		return s.c_str();
	}
	static std::string Format(LPCSTR format, ...)
	{
		va_list v;
		va_start(v, format);
		int nlen = _vscprintf(format, v);
		std::string s(nlen, nlen);
		int n = vsprintf_s((LPSTR)s.data(), nlen+1, format, v);
		va_end(v);
		return s;
	}

	static int Split(const wchar_t* lpStr, char cSep, std::vector<std::wstring>& arr, size_t uStrLen=0)
	{
		if(0 == uStrLen)
			uStrLen = wcslen(lpStr);

		size_t nValueBegin = 0;

		for (size_t i = 1; i <= uStrLen; ++i)
		{
			if (cSep == lpStr[i] && nValueBegin < i)
			{
				arr.push_back(std::move(std::wstring(lpStr + nValueBegin, i - nValueBegin)));

				nValueBegin = i + 1;
			}
		}
		if (nValueBegin < uStrLen)
		{
			arr.push_back(std::move(std::wstring(lpStr + nValueBegin, uStrLen - nValueBegin)));
		}
		return 0;
	}

	static void TrimBlank(std::wstring& s)
	{
		if (s.empty()) return;

		auto p = s.begin();
		//look for start
		while (p._Ptr)
		{
			if (*p._Ptr != ' ') break;
			p++;
		}
									//look for end
		auto p2 = s.end();
		while (p2._Ptr >= p._Ptr)
		{
			if (*(p2._Ptr-1) != ' ')
				break;
		
			--p2;
		}

		if (p2._Ptr < p._Ptr)
			s.clear();
		else if(p != s.begin() || p2 != s.end())
		{
			s = std::wstring(p._Ptr, p2._Ptr);
		}
	}
};


#endif		