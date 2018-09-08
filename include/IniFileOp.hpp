#pragma once 
#include <string>

class IniFileOp
{
public:
	static std::wstring GetValue(LPCTSTR lpSec, LPCTSTR lpKey, LPCTSTR lpPath, LPCTSTR lpDefault=nullptr)
	{
		std::wstring sValue;
		sValue.resize(1024);
		
		DWORD uSize = GetPrivateProfileStringW(lpSec, lpKey, lpDefault, (wchar_t*)sValue.data(), 1024, lpPath);
		if (uSize > 0)
		{
			sValue[uSize] = '\0';
		}

		return sValue;
	}
	static int GetInt(LPCTSTR lpSec, LPCTSTR lpKey, LPCTSTR lpPath, int nDefault=0)
	{
		return (int)GetPrivateProfileInt(lpSec, lpKey, nDefault, lpPath);
	}
public:
	IniFileOp()
	{

	}
	virtual void SetPath(LPCTSTR lpPath)
	{
		m_sPath = lpPath;
	}
public:
	bool SetStr(LPCTSTR lpSec, LPCTSTR lpKey, LPCTSTR lpValue)
	{
		return TRUE == WritePrivateProfileString(lpSec, lpKey, lpValue, m_sPath.c_str());
	}
	std::wstring GetStr(LPCTSTR lpSec, LPCTSTR lpKey, LPCTSTR lpDefault=nullptr)
	{
		return IniFileOp::GetValue(lpSec, lpKey, m_sPath.c_str(), lpDefault);
	}

	bool SetInt(LPCTSTR lpSec, LPCTSTR lpKey, int nValue)
	{
		wchar_t lpValue[1024];
		_itow_s(nValue, lpValue, 10);
		return TRUE == WritePrivateProfileString(lpSec, lpKey, lpValue, m_sPath.c_str());
	}
	int GetInt(LPCTSTR lpSec, LPCTSTR lpKey, int nDefault = 0)
	{
		return IniFileOp::GetInt(lpSec, lpKey, m_sPath.c_str(), nDefault);
	}
protected:
	std::wstring m_sPath;
};