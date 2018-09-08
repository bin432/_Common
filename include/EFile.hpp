#ifndef __ETIMES_FILE_H___
#define __ETIMES_FILE_H___
#pragma once
#include <assert.h>
#include "io/io.hpp"

class EFile : public IReadWriteCloser
{
public:
	// Flag values
	enum OpenFlags
	{
		modeRead =         (int) 0x00000,
		modeWrite =        (int) 0x00001,
		modeReadWrite =    (int) 0x00002,
		shareCompat =      (int) 0x00000,
		shareExclusive =   (int) 0x00010,
		shareDenyWrite =   (int) 0x00020,
		shareDenyRead =    (int) 0x00030,
		shareDenyNone =    (int) 0x00040,
		modeNoInherit =    (int) 0x00080,
		modeCreate =       (int) 0x01000,
		modeNoTruncate =   (int) 0x02000,
		typeText =         (int) 0x04000, // typeText and typeBinary are
		typeBinary =       (int) 0x08000, // used in derived classes only
		osNoBuffer =       (int) 0x10000,
		osWriteThrough =   (int) 0x20000,
		osRandomAccess =   (int) 0x40000,
		osSequentialScan = (int) 0x80000,
	};

	enum Attribute
	{
		normal =    0x00,
		readOnly =  0x01,
		hidden =    0x02,
		system =    0x04,
		volume =    0x08,
		directory = 0x10,
		archive =   0x20
	};
	
	EFile()
	{
		m_hFile = INVALID_HANDLE_VALUE;
	}
	bool Open(LPCTSTR lpszFileName, UINT nOpenFlags= modeRead| shareDenyRead)
	{
		if(INVALID_HANDLE_VALUE != m_hFile)
			return false;

		// EFile objects are always binary and CreateFile does not need flag
		nOpenFlags &= ~(UINT)typeBinary;

		
		DWORD dwAccess = 0;
		switch (nOpenFlags & 3)
		{
		case modeRead:
			dwAccess = GENERIC_READ;
			break;
		case modeWrite:
			dwAccess = GENERIC_WRITE;
			break;
		case modeReadWrite:
			dwAccess = GENERIC_READ | GENERIC_WRITE;
			break;
		default:
			return false;
		}

		// map share mode
		DWORD dwShareMode = 0;
		switch (nOpenFlags & 0x70)    // map compatibility mode to exclusive
		{
		default:
			assert(FALSE);  // invalid share mode?
		case shareCompat:
		case shareExclusive:
			dwShareMode = 0;
			break;
		case shareDenyWrite:
			dwShareMode = FILE_SHARE_READ;
			break;
		case shareDenyRead:
			dwShareMode = FILE_SHARE_WRITE;
			break;
		case shareDenyNone:
			dwShareMode = FILE_SHARE_WRITE | FILE_SHARE_READ;
			break;
		}


		// map modeNoInherit flag
		SECURITY_ATTRIBUTES sa;
		sa.nLength = sizeof(sa);
		sa.lpSecurityDescriptor = NULL;
		sa.bInheritHandle = (nOpenFlags & modeNoInherit) == 0;

		// map creation flags
		DWORD dwCreateFlag;
		if (nOpenFlags & modeCreate)
		{
			if (nOpenFlags & modeNoTruncate)
				dwCreateFlag = OPEN_ALWAYS;
			else
				dwCreateFlag = CREATE_ALWAYS;
		}
		else
			dwCreateFlag = OPEN_EXISTING;


		// Random access and sequential scan should be mutually exclusive
		assert((nOpenFlags&(osRandomAccess|osSequentialScan)) != (osRandomAccess|
			osSequentialScan) );

		DWORD dwFlags = FILE_ATTRIBUTE_NORMAL;
		if (nOpenFlags & osNoBuffer)
			dwFlags |= FILE_FLAG_NO_BUFFERING;
		if (nOpenFlags & osWriteThrough)
			dwFlags |= FILE_FLAG_WRITE_THROUGH;
		if (nOpenFlags & osRandomAccess)
			dwFlags |= FILE_FLAG_RANDOM_ACCESS;
		if (nOpenFlags & osSequentialScan)
			dwFlags |= FILE_FLAG_SEQUENTIAL_SCAN;


		m_hFile = ::CreateFile(lpszFileName, dwAccess, dwShareMode, &sa, dwCreateFlag, dwFlags, NULL);
		if (INVALID_HANDLE_VALUE == m_hFile)
		{
			return false;
		}
		
		return true;
	}
	int Read(void* lpBuf, unsigned int uLen)
	{
		if(0 == uLen)
			return 0;

		DWORD dwRead;
		if (!::ReadFile(m_hFile, lpBuf, uLen, &dwRead, NULL))
		{
			return -1;
		}

		return static_cast<int>(dwRead);
	}
	int Write(void* lpBuf, unsigned int uLen)
	{
		DWORD dwWritten;
		if (!::WriteFile(m_hFile, lpBuf, uLen, &dwWritten, NULL))
		{
			return -1;
		}
		return dwWritten;
	}
	__int64 Seek(__int64 lOff, ISeeker::SeekPosition e=ISeeker::begin)
	{
		LARGE_INTEGER liOff;

		liOff.QuadPart = lOff;
		liOff.LowPart = ::SetFilePointer(m_hFile, liOff.LowPart, &liOff.HighPart, (DWORD)e);

		return liOff.QuadPart;
	}
	ULONGLONG GetLength() const
	{

		ULARGE_INTEGER liSize;
		liSize.LowPart = ::GetFileSize(m_hFile, &liSize.HighPart);
		/*if (liSize.LowPart == INVALID_FILE_SIZE)
			if (::GetLastError() != NO_ERROR)
				CFileException::ThrowOsError((LONG)::GetLastError(), m_strFileName);*/

		return liSize.QuadPart;
	}
	int Close()
	{
		if (m_hFile != INVALID_HANDLE_VALUE)
			::CloseHandle(m_hFile);

		m_hFile = INVALID_HANDLE_VALUE;
		return 0;
	}
private:
	HANDLE m_hFile;


	//////////////////////////////////////////////////////////////////////////
public:
	static bool ReadAllString(IReader* reader, std::string& sText)
	{
		if (nullptr == reader) return false;

		char buf[4096];
		while (true)
		{
			int nReaded = reader->Read(buf, 4096);
			if(0 == nReaded)
				break;

			sText.append(buf, nReaded);
		}
		
		return true;
	}

	//
	static void ReadBuf(IReader* reader, unsigned char* pBuf, unsigned __int64 ulBufSize)
	{
		if (nullptr == reader) return ;

		unsigned __int64 ulReaded = 0;
		while (true)
		{
			int n = ((ulBufSize - ulReaded) >= 65536) ? 65536 : ulBufSize - ulReaded;
			int nReaded = reader->Read(pBuf+ulReaded, n);
			if (0 == nReaded)
				break;

			ulReaded += nReaded;
		}
	}
};




#endif