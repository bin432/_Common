#pragma once
#ifndef __LOGWRITER_HPP___
#define __LOGWRITER_HPP___

#include "CSLock.hpp"
#include "CodingConv.hpp"

namespace LogWriter
{
// 日志 标志 
#define LogFlag_Date					0x01				// 是否 记录 日期
#define LogFlag_Time					0x02				// 是否 记录 时间
#define LogFlag_ThId					0x04				// 是否 记录 线程 id

#define DefaultLogFlag		LogFlag_Date|LogFlag_Time|LogFlag_ThId				// 默认 标志

// 直接 写入 文件  没有处理 线程 安全 只是 
inline bool WriteLog_0(const wchar_t* lpLogFile, DWORD dwFlags, const char* lpText)//= DefaultLogFlag)
{
	FILE* fp = _wfsopen(lpLogFile, L"a+", 0x40/*_SH_DENYNO*/);
	if (!fp) return false;

	if ((LogFlag_Date == (LogFlag_Date & dwFlags)) || (LogFlag_Time == (LogFlag_Time&dwFlags)) )
	{
		SYSTEMTIME t;
		::GetLocalTime(&t);
		fwrite("[", 1, 1, fp);

		if (LogFlag_Date == (LogFlag_Date & dwFlags))
		{
			fprintf(fp, " %04d-%02d-%02d", t.wYear, t.wMonth, t.wDay);
		}

		if (LogFlag_Time == (LogFlag_Time & dwFlags))
		{
			fprintf(fp, " %02d:%02d:%02d.%04d", t.wHour, t.wMinute, t.wSecond, t.wMilliseconds);
		}
		fwrite(" ]", 2, 1, fp);
	}

	if (LogFlag_ThId == (LogFlag_ThId & dwFlags))
	{
		DWORD dwThreadId = ::GetCurrentThreadId();
		fprintf(fp, "[%d]", ::GetCurrentThreadId());
	}
		
	//fprintf(fp, "%s\r\n", lpText);
	fwrite(lpText, strlen(lpText), 1, fp);
	fwrite("\r\n", 2, 1, fp);

	fclose(fp);
	
	return true;
}

inline bool WriteLog_0(const wchar_t* lpLogFile, DWORD dwFlags, const wchar_t* lpText)
{
	return WriteLog_0(lpLogFile, dwFlags, CodingConv::UToUtf8(lpText).c_str());
}

inline bool WriteLog(const wchar_t* lpLogFile, DWORD dwFlags, const wchar_t* lpFormat, ...)
{
	va_list arg_ptr;
	va_start(arg_ptr, lpFormat);
	size_t numOfChar = _vscwprintf(lpFormat, arg_ptr) + 1;
	wchar_t* lpBuf = new wchar_t[numOfChar];
	vswprintf_s(lpBuf, numOfChar, lpFormat, arg_ptr);

	bool b = WriteLog_0(lpLogFile, dwFlags, lpBuf);
	delete[] lpBuf;
	return b;
}


class Loger
{
public:
	Loger()
		: m_bDebug(false)
	{
		memset(m_lpLogFile, 0, 256);
	}
	void Init(const wchar_t* lpLogFile, bool bDebug=false, DWORD dwFlags = DefaultLogFlag)
	{
		m_bDebug = bDebug;
		m_dwLogFlags = dwFlags;
		wcscpy_s(m_lpLogFile, lpLogFile);
	}

	void Write_0(const char* lpText)
	{
		m_Lock.Lock();
		WriteLog_0(m_lpLogFile, m_dwLogFlags, lpText);
		m_Lock.Unlock();
	}
	void Write(const char* lpFormat, ...)
	{
		va_list arg_ptr;
		va_start(arg_ptr, lpFormat);
		size_t numOfChar = _vscprintf(lpFormat, arg_ptr) + 1;
		char* lpBuf = new char[numOfChar];
		vsprintf_s(lpBuf, numOfChar, lpFormat, arg_ptr);

		m_Lock.Lock();
		bool b = WriteLog_0(m_lpLogFile, m_dwLogFlags, lpBuf);
		m_Lock.Unlock();
		delete[] lpBuf;
	}

	void Write_0(const wchar_t* lpText)
	{
		m_Lock.Lock();
		WriteLog_0(m_lpLogFile, m_dwLogFlags, lpText);
		m_Lock.Unlock();
	}
	void Write(const wchar_t* lpFormat, ...)
	{
		va_list arg_ptr;
		va_start(arg_ptr, lpFormat);
		size_t numOfChar = _vscwprintf(lpFormat, arg_ptr) + 1;
		wchar_t* lpBuf = new wchar_t[numOfChar];
		vswprintf_s(lpBuf, numOfChar, lpFormat, arg_ptr);

		m_Lock.Lock();
		bool b = WriteLog_0(m_lpLogFile, m_dwLogFlags, lpBuf);
		m_Lock.Unlock();
		delete[] lpBuf;
	}
	void Debug_0(const wchar_t* lpText)
	{
		if (!m_bDebug) return;

		m_Lock.Lock();
		WriteLog_0(m_lpLogFile, m_dwLogFlags, lpText);
		m_Lock.Unlock();
	}
	void Debug(const wchar_t* lpFormat, ...)
	{
		if (!m_bDebug) return;

		va_list arg_ptr;
		va_start(arg_ptr, lpFormat);
		size_t numOfChar = _vscwprintf(lpFormat, arg_ptr) + 1;
		wchar_t* lpBuf = new wchar_t[numOfChar];
		vswprintf_s(lpBuf, numOfChar, lpFormat, arg_ptr);

		m_Lock.Lock();
		bool b = WriteLog_0(m_lpLogFile, m_dwLogFlags, lpBuf);
		m_Lock.Unlock();
		delete[] lpBuf;
	}
protected:
	wchar_t m_lpLogFile[256];
	DWORD m_dwLogFlags;
	bool m_bDebug;
	CSLock m_Lock;
};

}// namespace

// 唯一 日志 对象 ，调用之前 先 初始化 Init(logfile, debug)
extern __declspec(selectany) LogWriter::Loger& theLog = LogWriter::Loger();
#endif	//__LOGWRITER_HPP___