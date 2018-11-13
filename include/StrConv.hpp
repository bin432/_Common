#ifndef __STRCONV_H___
#define __STRCONV_H___
#pragma once

#include <tchar.h>
#include <map>



namespace StrConv
{
	static bool ParseBool(const wchar_t* lpValue, bool def=false)
	{
		if (nullptr == lpValue || 0 == wcslen(lpValue)) return def;

		// only look at first char
		wchar_t first = *lpValue;

		// 1*, t* (true), T* (True), y* (yes), Y* (YES)
		return (first == '1' || first == 't' || first == 'T' || first == 'y' || first == 'Y');
	}

	static bool ParseBool(const char* lpValue, bool def = false)
	{
		if (nullptr == lpValue || 0 == strlen(lpValue)) return def;

		// only look at first char
		char first = *lpValue;

		// 1*, t* (true), T* (True), y* (yes), Y* (YES)
		return (first == '1' || first == 't' || first == 'T' || first == 'y' || first == 'Y');
	}
	
	static long ParseLong(const wchar_t* lpValue, long def=0, int bitsize=10)
	{
		if (nullptr == lpValue || 0 == wcslen(lpValue)) return def;

		return wcstol(lpValue, nullptr, bitsize);
	}

	static long ParseLong(const char* lpValue, long def = 0, int bitsize = 10)
	{
		if (nullptr == lpValue || 0 == strlen(lpValue)) return def;

		return strtol(lpValue, nullptr, bitsize);
	}

	static unsigned long ParseUlong(const wchar_t* lpValue, unsigned int def=0, int bitsize = 10)
	{
		if (nullptr == lpValue || 0 == wcslen(lpValue)) return def;

		return wcstoul(lpValue, nullptr, bitsize);
	}

	static unsigned long ParseUlong(const char* lpValue, unsigned int def = 0, int bitsize = 10)
	{
		if (nullptr == lpValue || 0 == strlen(lpValue)) return def;

		return strtoul(lpValue, nullptr, bitsize);
	}

	static __int64 ParseInt64(const wchar_t* lpValue, __int64 def=0, int bitsize = 10)
	{
		if (nullptr == lpValue || 0 == wcslen(lpValue)) return def;

		return _wcstoi64(lpValue, nullptr, bitsize);
	}

	static __int64 ParseInt64(const char* lpValue, __int64 def = 0, int bitsize = 10)
	{
		if (nullptr == lpValue || 0 == strlen(lpValue)) return def;

		return _strtoi64(lpValue, nullptr, bitsize);
	}

	static unsigned __int64 ParseUint64(const wchar_t* lpValue, unsigned __int64 def=0,int bitsize = 10)
	{
		if (nullptr == lpValue || 0 == wcslen(lpValue)) return def;

		return _wcstoui64(lpValue, nullptr, bitsize);
	}

	static unsigned long long ParseUint64(const char* lpValue, unsigned __int64 def=0, int bitsize = 10)
	{
		if (nullptr == lpValue || 0 == strlen(lpValue)) return def;

		return _strtoui64(lpValue, nullptr, bitsize);
	}


	/////////字符串 转 double float 后面会有一些 0 1 2 3 之类的 还不知道怎么处理
	static double ParseDouble(const wchar_t* lpValue, double def=0.0)
	{
		if (nullptr == lpValue || 0 == wcslen(lpValue)) return def;

		return wcstod(lpValue, nullptr);
	}

	static double ParseDouble(const char* lpValue, double def=0.0)
	{
		if (nullptr == lpValue || 0 == strlen(lpValue)) return def;

		return strtod(lpValue, nullptr);
	}

	static float ParseFloat(const wchar_t* lpValue, float def=0.0)
	{
		if (nullptr == lpValue || 0 == wcslen(lpValue)) return def;

		return static_cast<float>(wcstod(lpValue, nullptr));
	}

	static float ParseFloat(const char* lpValue, float def=0.0)
	{
		if (nullptr == lpValue || 0 == strlen(lpValue)) return def;
		
		return static_cast<float>(strtod(lpValue, nullptr));
	}
}


#endif