#ifndef __INIT_SOCKET_HPP___
#define __INIT_SOCKET_HPP___
#pragma once

//#include <winsock.h>
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

/************************************************************************
名称：Windows Socket 组件初始化类
描述：自动加载和卸载 Windows Socket 组件

用法：在 要用到winsock 功能时，将 该头文件 放到 stdafx.h 文件里 声明 即可。
************************************************************************/
class InitSocket
{
public:
	InitSocket(LPWSADATA lpWSAData = nullptr, BYTE minorVersion = 2, BYTE majorVersion = 2)
	{
		LPWSADATA lpTemp = lpWSAData;
		if (!lpTemp)
			lpTemp = (LPWSADATA)_alloca(sizeof(WSADATA));

		m_iResult = ::WSAStartup(MAKEWORD(majorVersion, minorVersion), lpTemp);
	}

	~InitSocket()
	{
		if (IsValid())
			::WSACleanup();
	}

	int	 GetResult() const { return m_iResult; }
	bool IsValid()	 const { return m_iResult == 0; }

private:
	int m_iResult;
};


static InitSocket msInitSocket;

#endif		// __INIT_SOCKET_HPP___