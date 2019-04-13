#ifndef __INIT_SOCKET_HPP___
#define __INIT_SOCKET_HPP___
#pragma once

//#include <winsock.h>
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

/************************************************************************
���ƣ�Windows Socket �����ʼ����
�������Զ����غ�ж�� Windows Socket ���

�÷����� Ҫ�õ�winsock ����ʱ���� ��ͷ�ļ� �ŵ� stdafx.h �ļ��� ���� ���ɡ�
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