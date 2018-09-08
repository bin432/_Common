#ifndef __TIME_DATE_H___
#define __TIME_DATE_H___
#pragma once

#include <string>

class TimeDate
{
public:
	//************************************
	// Method:    获取 当前用户的用户目录  
	//************************************
	static std::wstring GetDateTimeFormat(SYSTEMTIME* pTime=nullptr, wchar_t firstChar='-', wchar_t secondChar=':')
	{	
		TCHAR lpTimeBuf[1024] = {0};
		SYSTEMTIME sysTime;
		if(nullptr == pTime)
		{			
			GetLocalTime(&sysTime);
			pTime = &sysTime;
		}
		

		int nLen = swprintf_s(lpTimeBuf, 1024, L"%04d%c%02d%c%02d %02d%c%02d%c%02d", 
			pTime->wYear,
			firstChar,
			pTime->wMonth,
			firstChar,
			pTime->wDay,
			
			pTime->wHour,
			secondChar,
			pTime->wMinute,
			secondChar,
			pTime->wSecond);
	
		return std::wstring(lpTimeBuf, nLen);
	}

	static std::wstring GetDateFormat(SYSTEMTIME* pTime=NULL, wchar_t cChar='-')
	{
		TCHAR lpTimeBuf[1024] = {0};
		SYSTEMTIME sysTime;
		if(nullptr == pTime)
		{			
			GetLocalTime(&sysTime);
			pTime = &sysTime;
		}
		
		int nLen = swprintf_s(lpTimeBuf, 1024, L"%04d%c%02d%c%02d", 
			pTime->wYear,
			cChar,
			pTime->wMonth,
			cChar,
			pTime->wDay);

		return std::wstring(lpTimeBuf, nLen);
	}

	static std::wstring GetTimeFormat(SYSTEMTIME* pTime=nullptr, wchar_t cChar=':')
	{
		TCHAR lpTimeBuf[1024] = {0};
		SYSTEMTIME sysTime;
		if(NULL == pTime)
		{			
			GetLocalTime(&sysTime);
			pTime = &sysTime;
		}

		int nLen = swprintf_s(lpTimeBuf, 1024, L"%02d%c%02d%c%02d", 
			pTime->wHour,
			cChar,
			pTime->wMinute,
			cChar,
			pTime->wSecond);

		return std::wstring(lpTimeBuf, nLen);
	}

	static std::wstring FormatTimeSecond(long lSecond, wchar_t cChar=':')
	{
		if (lSecond <= 0)
			return L"0";
		int nHour = lSecond / 3600;
		int nMinute = nHour ? lSecond % 3600 / 60 : lSecond / 60;
		int nSecond = lSecond % 60;

		TCHAR lpTimeBuf[1024] = { 0 };
		int nLen = swprintf_s(lpTimeBuf, 1024, L"%d%c%02d%c%02d",
			nHour,
			cChar,
			nMinute,
			cChar,
			nSecond);

		return std::wstring(lpTimeBuf, nLen);
	}
};




#endif