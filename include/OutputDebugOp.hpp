#pragma once
#ifndef __OUTPUTDEBUG_H___
#define __OUTPUTDEBUG_H___

class OutputDebug
{
public:
	static void Show_0(LPCWSTR text)
	{
		OutputDebugStringW(text);
	}
	static void Show(LPCWSTR format, ...)
	{
		va_list v;
		va_start(v, format);
		int nlen = _vscwprintf(format, v) + 1;
		wchar_t* pBuf = new wchar_t[nlen];
		vswprintf_s(pBuf, nlen, format, v);
		va_end(v);
		OutputDebugStringW(pBuf);
		delete[] pBuf;
	}

	static void Show_0(LPCSTR text)
	{
		OutputDebugStringA(text);
	}
	static void Show(LPCSTR format, ...)
	{
		va_list v;
		va_start(v, format);
		int nlen = _vscprintf(format, v) + 1;
		char* pBuf = new char[nlen];
		vsprintf_s(pBuf, nlen, format, v);
		va_end(v);
		OutputDebugStringA(pBuf);
		delete[] pBuf;
	}
};


#endif		