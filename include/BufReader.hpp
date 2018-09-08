#ifndef __BUF_READER_H___
#define __BUF_READER_H___
#pragma once

template<class T>
class BufReader
{
public:
	BufReader(const T* pData, int nLen, int nOffset=0)
		: m_pData((T*)pData)
		, m_nLen(nLen)
		, m_nOffset(nOffset)
	{

	}

	int Read(T*& pBuf, int nRead)
	{
		int nRemain = m_nLen - m_nOffset;
		if (0 == nRemain)
			return 0;

		if (nRead > nRemain)
			nRead = nRemain;

		pBuf = m_pData + m_nOffset;
		m_nOffset += nRead;

		return nRead;
	}
protected:
	T* m_pData;
	int m_nLen;
	int m_nOffset;
};


#endif