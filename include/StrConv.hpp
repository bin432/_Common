#ifndef __STRCONV_H___
#define __STRCONV_H___
#pragma once

#include <string>
#include <map>



namespace StrConv
{
	static bool ParseBool(const wchar_t* lpValue, bool def=false)
	{
		if (nullptr == lpValue) return def;

		// only look at first char
		wchar_t first = *lpValue;

		// 1*, t* (true), T* (True), y* (yes), Y* (YES)
		return (first == '1' || first == 't' || first == 'T' || first == 'y' || first == 'Y');
	}

	static int ParseInt(const wchar_t* lpValue, int def=0, int bitsize=10)
	{
		if (nullptr == lpValue) return def;

		return static_cast<int>(_tcstol(lpValue, 0, bitsize));
	}

	static unsigned int ParseUint(const wchar_t* lpValue, unsigned int def=0, int bitsize = 10)
	{
		if (nullptr == lpValue) return def;

		return static_cast<unsigned int>(_tcstoul(lpValue, 0, bitsize));
	}

	static long long ParseInt64(const wchar_t* lpValue, long long def, int bitsize = 10)
	{
		if (nullptr == lpValue) return def;

		return static_cast<long long>(_tcstoi64(lpValue, 0, bitsize));
	}

	static unsigned long long ParseUint64(const wchar_t* lpValue, unsigned long long def,int bitsize = 10)
	{
		if (nullptr == lpValue) return def;

		return static_cast<unsigned long long>(_wcstoui64(lpValue, 0, bitsize));
	}

	static double ParseDouble(const wchar_t* lpValue, double def)
	{
		if (nullptr == lpValue) return def;

		return _tcstod(lpValue, 0);
	}

	static float ParseFloat(const wchar_t* lpValue, float def)
	{
		if (nullptr == lpValue) return def;

		return static_cast<float>(_tcstod(lpValue, 0));
	}
	


	// A °æ±¾
	static unsigned long long ParseUint64(const char* lpValue, unsigned long long def, int bitsize = 10)
	{
		if (nullptr == lpValue) return def;

		return static_cast<unsigned long long>(_strtoui64(lpValue, 0, bitsize));
	}
}


#endif