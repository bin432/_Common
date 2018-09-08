#pragma once
#ifndef __STR_OP_H___
#define __STR_OP_H___
#include <vector>

class StrOp
{
public:
	static bool IsNullOrEmpty(LPCTSTR lpStr)
	{
		return nullptr == lpStr || 0 == lstrlen(lpStr);
	}
	static std::wstring Format(LPCWSTR format, ...)
	{
		va_list v;
		va_start(v, format);
		int nlen = _vscwprintf(format, v) + 1;
		std::wstring s(nlen, nlen);
		vswprintf_s((LPWSTR)s.data(), nlen, format, v);
		va_end(v);
		return s;
	}
	static const wchar_t* Format(std::wstring& s, LPCWSTR format, ...)
	{
		va_list v;
		va_start(v, format);
		int nlen = _vscwprintf(format, v) + 1;
		s.resize(nlen);
		vswprintf_s((LPWSTR)s.data(), nlen, format, v);
		va_end(v);
		return s.c_str();
	}

	static const char* Format(std::string& s, LPCSTR format, ...)
	{
		va_list v;
		va_start(v, format);
		int nlen = _vscprintf(format, v) + 1;
		s.resize(nlen);
		vsprintf_s((LPSTR)s.data(), nlen, format, v);
		va_end(v);
		return s.c_str();
	}

	static int Split(const wchar_t* lpStr, char cSep, std::vector<std::wstring>& arr)
	{
		int nLen = lstrlenW(lpStr);

		int nValueBegin = 0;

		for (int i = 1; i <= nLen; ++i)
		{
			if (cSep == lpStr[i] || '\0' == lpStr[i])
			{
				arr.push_back(std::move(std::wstring(lpStr + nValueBegin, i - nValueBegin)));

				nValueBegin = i + 1;
			}
		}
		if (nValueBegin < nLen)
		{
			arr.push_back(std::move(std::wstring(lpStr + nValueBegin, nLen - nValueBegin)));
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