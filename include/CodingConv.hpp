/* at 2018/09/28				v2.1

	修改 之前 string 和wstring 末尾 都会 有一个 '\0'结束符，这个不应该有
	模版函数 也 

*/


#pragma once
#ifndef __CODINGCONV_H___
#define __CODINGCONV_H___

#define _FUNC_W_		CodingConv::AToU(__func__).c_str()

class CodingConv
{
public:
	static std::string UToA(const wchar_t* lpszW, UINT CodePage= CP_ACP)
	{
		size_t size_w = wcslen(lpszW);
		if ((nullptr == lpszW) || (0 == size_w)) return std::string();

		// cchWideChar 如果传 -1  那么返回长度 包括 '\0'  也就是 比 wcslen 多 1;
		// 所以 还是 传 len 取好了 
		int nLen = WideCharToMultiByte(CodePage, 0, lpszW, static_cast<int>(size_w), NULL, 0, NULL, NULL);

		std::string sOut;			// string 不能包含 '\0' 会发生很多意外的事情
		sOut.resize(nLen);

		nLen = WideCharToMultiByte(CodePage, 0, lpszW, static_cast<int>(size_w), (char*)sOut.data(), nLen, NULL, NULL);

		return sOut;
	}

	// 大数据  建议  使用 这两个 转换  比上面 少 一次 内存 拷贝。
	static std::string UToUtf8(const wchar_t* lpszW)
	{
		return UToA(lpszW, CP_UTF8);
	}

	static std::wstring AToU(const char* lpszA, UINT CodePage = CP_ACP)
	{
		size_t size_a = strlen(lpszA);
		if ((nullptr == lpszA) || (0 == size_a))
			return std::wstring();

		// cbMultiChar 如果传 -1  那么返回长度 包括 '\0'  也就是 比 wcslen 多 1;
		// 所以 还是 传 len 取好了 
		int nBufferLen = MultiByteToWideChar(CodePage, 0, lpszA, static_cast<int>(size_a), NULL, 0);

		std::wstring sOut;
		sOut.resize(nBufferLen);

		nBufferLen = MultiByteToWideChar(CodePage, 0, lpszA, static_cast<int>(size_a), (wchar_t*)sOut.data(), nBufferLen);

		return sOut;
	}

	static std::wstring Utf8ToU(const char* lpszUtf8)
	{
		return AToU(lpszUtf8, CP_UTF8);
	}


	template<class STRA>
	static STRA UnicodeToUtf8(const wchar_t* lpszW)
	{
		size_t size_w = wcslen(lpszW);
		if ((nullptr == lpszW) || (0 == size_w)) return STRA();

		//得到转换后的字符串长度
		int nBufferLen = WideCharToMultiByte(CP_UTF8, 0, lpszW, static_cast<int>(size_w), NULL, 0, NULL, NULL);

		LPSTR lpszUtf8 = new(std::nothrow) char[nBufferLen];
		if (nullptr == lpszUtf8)
		{
			return lpszUtf8;
		}

		nBufferLen = WideCharToMultiByte(CP_UTF8, 0, lpszW, static_cast<int>(size_w), lpszUtf8, nBufferLen, NULL, NULL);

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
		size_t size_a = strlen(lpszUtf8);
		if ((nullptr == lpszUtf8) || (0 == size_a))
			return STRW();

		//得到转换后的字符串长度
		int nBufferLen = MultiByteToWideChar(CP_UTF8, 0, lpszUtf8, static_cast<int>(size_a), NULL, 0);

		//new buffer
		LPWSTR lpwBuf = new(std::nothrow) wchar_t[nBufferLen];
		if (nullptr == lpwBuf)
		{
			return lpwBuf;
		}

		nBufferLen = MultiByteToWideChar(CP_UTF8, 0, lpszUtf8, static_cast<int>(size_a), lpwBuf, nBufferLen);


		if (nullptr != lpwBuf)
		{
			STRW sResult(lpwBuf, nBufferLen);
			delete[] lpwBuf;
			lpwBuf = nullptr;
			return sResult;
		}

		return STRW();
	}
};

// 用于 atl 的 CString
#define ATL_UA2W CodingConv::Utf8ToUnicode<CStringW>
#define ATL_W2UA CodingConv::UnicodeToUtf8<CStringA>



#endif