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

		char lpHostName[256] = {0};
		if (SOCKET_ERROR == gethostname(lpHostName, sizeof(lpHostName)))
		{
			WSACleanup();
			return "";
		}

		szHostName = lpHostName;


		if (nullptr != pStrLocalIp || nullptr != pStrLocalMacAddr)
		{
			char* lpLocalIp = nullptr;
			HOSTENT* pHost = gethostbyname(lpHostName);
			if (pHost != nullptr)
			{
				lpLocalIp = inet_ntoa(*(IN_ADDR*)pHost->h_addr_list[0]);
				if (nullptr != pStrLocalIp)
					*pStrLocalIp = lpLocalIp;

				if (nullptr != pStrLocalMacAddr)
				{
					byte mac[6] = {0};
					ULONG uMacLen = 6;
					DWORD dwStatus = SendARP(inet_addr(lpLocalIp), 0, mac, &uMacLen);
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

		return szHostName;
	}

	//
	struct NetAddr
	{
		//std::string name;
		std::string ip;
		std::string mac;
	};

	static std::string GetNetAddr(std::vector<NetAddr>& addrs)
	{
		char lpHostName[256] = { 0 };
		if (SOCKET_ERROR == gethostname(lpHostName, sizeof(lpHostName)))
		{
			return "";
		}
		std::string hostName = lpHostName;

		HOSTENT* host = gethostbyname(lpHostName);
		if (nullptr == host) return 0;

		char* al = nullptr;
		int i = 0;
		while(true)
		{
			al = host->h_addr_list[i];
			if (nullptr == al)
				break;

			char* ip = inet_ntoa(*(IN_ADDR*)al);
			
			NetAddr na;
			na.ip = ip;
			na.mac = GetMacAddr(ip);
			addrs.push_back(std::move(na));

			++i;
		}
		return hostName;
	}

	static std::string GetMacAddr(const char* ip)
	{
		byte buf[6] = { 0 };
		ULONG uMacLen = 6;
		DWORD dwStatus = SendARP(inet_addr(ip), 0, buf, &uMacLen);

		std::string mac(17, 0);
		sprintf_s((char*)mac.data(), 18, "%02X-%02X-%02X-%02X-%02X-%02X",
			buf[0], buf[1], buf[2], buf[3], buf[4], buf[5]);

		return mac;
	}
};



#endif