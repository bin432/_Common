#pragma once
#ifndef __CODINGCONV_H___
#define __CODINGCONV_H___

class CodingConv
{
public:
	template<class STRA>
	static STRA UnicodeToUtf8(const wchar_t* lpwBuf)
	{
		if ((nullptr == lpwBuf) || (0 == wcslen(lpwBuf))) return STRA();

		//得到转换后的字符串长度
		int nBufferLen = WideCharToMultiByte(CP_UTF8, 0, lpwBuf, -1, NULL, 0, NULL, NULL);

		LPSTR lpszUtf8 = new(std::nothrow) char[nBufferLen];
		if (nullptr == lpszUtf8)
		{
			return lpszUtf8;
		}

		//memset(lpszUtf8, 0, nBufferLen);

		nBufferLen = WideCharToMultiByte(CP_UTF8, 0, lpwBuf, -1, lpszUtf8, nBufferLen, NULL, NULL);

		if (nullptr != lpszUtf8)
		{
			STRA sResult(lpszUtf8, nBufferLen);
			delete[] lpszUtf8;
			lpszUtf8 = nullptr;
			return sResult;
		}

		return STRA();
	}

	template<class STRW>
	static STRW Utf8ToUnicode(const char* lpszUtf8)
	{
		if ((nullptr == lpszUtf8) || (0 == strlen(lpszUtf8)))
			return STRW();

		//得到转换后的字符串长度
		int nBufferLen = MultiByteToWideChar(CP_UTF8, 0, lpszUtf8, -1, NULL, NULL);

		//new buffer
		LPWSTR lpwBuf = new(std::nothrow) wchar_t[nBufferLen];
		if (nullptr == lpwBuf)
		{
			return lpwBuf;
		}

		//memset(lpwBuf, 0, nBufferLen * sizeof(wchar_t));

		nBufferLen = MultiByteToWideChar(CP_UTF8, 0, lpszUtf8, -1, lpwBuf, nBufferLen);


		if (nullptr != lpwBuf)
		{
			STRW sResult(lpwBuf, nBufferLen);
			delete[] lpwBuf;
			lpwBuf = nullptr;
			return sResult;
		}

		return STRW();
	}

	inline static std::string UToA(const wchar_t* lpszW, UINT CodePage= CP_ACP)
	{
		if ((nullptr == lpszW) || (0 == wcslen(lpszW))) return std::string();

		//得到转换后的字符串长度
		int nLen = WideCharToMultiByte(CodePage, 0, lpszW, -1, NULL, 0, NULL, NULL);

		std::string sOut;
		sOut.resize(nLen);

		nLen = WideCharToMultiByte(CodePage, 0, lpszW, -1, (char*)sOut.data(), nLen, NULL, NULL);

		return sOut;
	}

	// 大数据  建议  使用 这两个 转换  比上面 少 一次 内存 拷贝。
	static std::string UToUtf8(const wchar_t* lpszW)
	{
		return UToA(lpszW, CP_UTF8);
	}

	inline static std::wstring AToU(const char* lpszA, UINT CodePage = CP_ACP)
	{
		if ((nullptr == lpszA) || (0 == strlen(lpszA)))
			return std::wstring();

		//得到转换后的字符串长度
		int nBufferLen = MultiByteToWideChar(CodePage, 0, lpszA, -1, NULL, NULL);

		std::wstring sOut;
		sOut.resize(nBufferLen);

		nBufferLen = MultiByteToWideChar(CodePage, 0, lpszA, -1, (wchar_t*)sOut.data(), nBufferLen);

		return sOut;
	}

	static std::wstring Utf8ToU(const char* lpszUtf8)
	{
		return AToU(lpszUtf8, CP_UTF8);
	}
};

// 用于 atl 的 CString
#define ATL_UA2W CodingConv::Utf8ToUnicode<CStringW>
#define ATL_W2UA CodingConv::UnicodeToUtf8<CStringA>



#endif