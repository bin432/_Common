#ifndef __IO_H___
#define __IO_H___
#pragma once

namespace IO
{
	class IReader
	{
	public:
		virtual int Read(void* pBuf, unsigned int uLen) = 0;
	};

	class IWriter
	{
	public:
		virtual int Write(void* pBuf, unsigned int uLen) = 0;
	};

	class ICloser
	{
	public:
		virtual int Close() = 0;
	};

	class ISeeker
	{
	public:
		enum SeekPosition
		{
			begin = 0x0,
			current = 0x1,
			end = 0x2
		};

		virtual __int64 Seek(__int64 nOffset, SeekPosition e) = 0;
	};

	class IReadWriter : public IReader, public IWriter
	{
	};

	class IReadCloser : public IReader, public ICloser
	{
	};

	class IWriteCloser : public IWriter, public ICloser
	{
	};

	class IReadWriteCloser : public IReader, public IWriter, public ICloser
	{
	};



	inline int Copy(IReader* reader, IWriter* writer)
	{
		const UINT cBufLen = 1024 * 64;
		unsigned char* pBuffer = (unsigned char*)malloc(cBufLen);

		int nReadBytes = 0;
		int nRet = 0;
		while (true)
		{
			nReadBytes = reader->Read(pBuffer, cBufLen);
			if (nReadBytes < 0)
			{
				nRet = nReadBytes;
				break;
			}

			if (0 == nReadBytes)
				break;

			nReadBytes = writer->Write(pBuffer, nReadBytes);
			if (nReadBytes < 0)
			{
				nRet = nReadBytes;
				break;
			}
		}

		free(pBuffer);
		pBuffer = nullptr;

		return nRet;
	}
}
#endif