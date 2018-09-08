#ifndef __NETWORD_OP_H___
#define __NETWORD_OP_H___
#pragma once
/*******************
*  需要在 stdafx.h头文件里 添加 
*	#include <winsock2.h>
*	#pragma comment(lib, "ws2_32.lib")
*
*
*********************/
#include <IPHlpApi.h>
#pragma comment(lib,"IPHlpApi.lib")

#include <string>

class NetWorkOp
{
public:
	//************************************
	// Method:    获取本机 计算机 一些信息 
	// FullName:  GetLocalNetWorkInfo
	// Access:    public 
	// Returns:   std::string hostname 
	// Qualifier:
	// Parameter: std::string * pStrLocalIp
	// Parameter: std::string * pStrLocalMacAddr
	//************************************
	static std::string GetLocalNetWork(std::string* pStrLocalIp, std::string* pStrLocalMacAddr)
	{
		std::string szHostName;

		WORD wVersionRequested;
		WSADATA wsaData;
		int err;
		wVersionRequested = MAKEWORD(2, 2);

		err = WSAStartup(wVersionRequested, &wsaData);
		if (err != 0)
		{
			return "";
		}

		char lpHostName[256] = {0};
		if (SOCKET_ERROR == gethostname(lpHostName, sizeof(lpHostName)))
		{
			WSACleanup();
			return "";
		}

		szHostName = lpHostName;


		if (NULL != pStrLocalIp || NULL != pStrLocalMacAddr)
		{
			char* lpLocalIp = NULL;
			HOSTENT* pHost = gethostbyname(lpHostName);
			if (pHost != NULL)
			{
				lpLocalIp = inet_ntoa(*(IN_ADDR*)pHost->h_addr_list[0]);
				if (NULL != pStrLocalIp)
					*pStrLocalIp = lpLocalIp;

				if (NULL != pStrLocalMacAddr)
				{
					byte mac[6] = {0};
					ULONG uMacLen = 6;
					DWORD dwStatus = SendARP(inet_addr(lpLocalIp), NULL, mac, &uMacLen);
					if (NO_ERROR == dwStatus)
					{
						char lpMac[20] = {0};
						sprintf_s(lpMac, "%02x-%02x-%02x-%02x-%02x-%02x",
							mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
						//pStrMacAddr->Format(_T(""),
						//);
						*pStrLocalMacAddr = lpMac;
					}
				}
			}
		}

		WSACleanup();

		return szHostName;
	}
};



#endif