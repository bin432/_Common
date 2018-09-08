#ifndef __COMMON_FUN_H___
#define __COMMON_FUN_H___
#pragma once

#include <string>

class CommonFun
{
public:
	static std::string NewGuid()
	{	
		
		GUID guid;
		if(S_OK != ::CoCreateGuid(&guid))		
			return "";

		char lpBuf[1024] = {0};
		int nLen = sprintf_s(lpBuf, 1024, "%08X%04X%04x%02X%02X%02X%02X%02X%02X%02X%02X"
			, guid.Data1, guid.Data2, guid.Data3
			, guid.Data4[0], guid.Data4[1], guid.Data4[2]	, guid.Data4[3]
			, guid.Data4[4], guid.Data4[5], guid.Data4[6]	, guid.Data4[7]);

		return std::string(lpBuf, nLen);
	}

	static std::wstring NewGuidW()
	{	

		GUID guid;
		if(S_OK != ::CoCreateGuid(&guid))		
			return L"";

		wchar_t lpBuf[1024] = {0};
		int nLen = swprintf_s(lpBuf, 1024, L"%08X%04X%04x%02X%02X%02X%02X%02X%02X%02X%02X"
			, guid.Data1, guid.Data2, guid.Data3
			, guid.Data4[0], guid.Data4[1], guid.Data4[2]	, guid.Data4[3]
		, guid.Data4[4], guid.Data4[5], guid.Data4[6]	, guid.Data4[7]);

		return std::wstring(lpBuf, nLen);
	}
	
	static int MatchUrl(const wchar_t* lpUrl, std::wstring& sIp, int nDefPort=80)
	{
		std::wstring sUrl(lpUrl);
		int nPos = sUrl.find(L"://");
		if(-1 != nPos)
		{
			sUrl = sUrl.substr(nPos + 3);
		}

		nPos = sUrl.find(':');
		if(-1 == nPos)
		{
			sIp = sUrl;
			return nDefPort;
		}

		sIp = sUrl.substr(0, nPos);
		return _wtoi(sUrl.substr(nPos+1).c_str());
	}
};




#endif