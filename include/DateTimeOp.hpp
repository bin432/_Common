#ifndef __DATE_TIME_H___
#define __DATE_TIME_H___
#pragma once

#include <string>

class DateTimeOp
{
public:
	//************************************
	// Method:    获取 时间 格式文本
	// pTime 空 就获取当前时间
	// dateChar 年月日 分割   timeChar 时分秒 分割
	//************************************
	static std::wstring GetDateTimeText(SYSTEMTIME* pTime=nullptr, wchar_t dateChar='-', wchar_t timeChar=':')
	{	
		wchar_t lpTimeBuf[1024] = {0};
		SYSTEMTIME sysTime;
		if(nullptr == pTime)
		{			
			GetLocalTime(&sysTime);
			pTime = &sysTime;
		}
		

		int nLen = swprintf_s(lpTimeBuf, 1024, L"%04d%c%02d%c%02d %02d%c%02d%c%02d", 
			pTime->wYear,
			dateChar,
			pTime->wMonth,
			dateChar,
			pTime->wDay,
			
			pTime->wHour,
			timeChar,
			pTime->wMinute,
			timeChar,
			pTime->wSecond);
	
		return std::wstring(lpTimeBuf, nLen);
	}

	static std::wstring GetDateText(SYSTEMTIME* pTime=nullptr, wchar_t cChar='-')
	{
		wchar_t lpTimeBuf[1024] = {0};
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

	static std::wstring GetTimeText(SYSTEMTIME* pTime=nullptr, wchar_t cChar=':')
	{
		wchar_t lpTimeBuf[1024] = {0};
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




#endif		//