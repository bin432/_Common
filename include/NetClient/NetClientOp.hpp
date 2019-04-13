/************************************************************************/
// 对 NetClientDll的一些封装 
/************************************************************************/
#ifndef _NETCLIENTOP_H
#define _NETCLIENTOP_H
#pragma  once

#include "NetClientDll.h"

#ifdef _WIN64
	#if _DEBUG
		#pragma comment(lib, "NetClientDll64_d.lib")
	#else
		#pragma comment(lib, "NetClientDll64.lib")
	#endif
#else
	#if _DEBUG
		#pragma comment(lib, "NetClientDll_d.lib")
	#else
		#pragma comment(lib, "NetClientDll.lib")
	#endif
#endif


// Http 响应 封装 IResponse
class Response
{
protected:
	NetClient::IResponse* m_resp;
public:
	Response(NetClient::IResponse* resp)
		: m_resp(resp)
	{

	}
	~Response()
	{
		if(nullptr != m_resp)
		{
			NetClient::fnDeleteObject(m_resp);
			m_resp = nullptr;
		}
	}
public:
	bool IsVaild()
	{
		return nullptr != m_resp;
	}
	int GetStatusCode()
	{
		return m_resp->GetStatusCode();
	}
	const char* GetStatusText()
	{
		return m_resp->GetStatusText();
	}
	const char* GetHeader(const char* lpKey)
	{
		return m_resp->GetHeader(lpKey);
	}
	const char* GetBody(int* pLen = nullptr)
	{
		return m_resp->GetBody(pLen);
	}
};

class RespReader
{
protected:
	NetClient::IRespReader* m_resp;
public:
	RespReader(NetClient::IRespReader* resp)
		: m_resp(resp)
	{

	}
	~RespReader()
	{
		if (nullptr != m_resp)
		{
			NetClient::fnDeleteObject(m_resp);
			m_resp = nullptr;
		}
	}
public:
	bool IsVaild()
	{
		return nullptr != m_resp;
	}
	int GetStatusCode()
	{
		return m_resp->GetStatusCode();
	}
	const char* GetStatusText()
	{
		return m_resp->GetStatusText();
	}
	const char* GetHeader(const char* lpKey)
	{
		return m_resp->GetHeader(lpKey);
	}
	int Read(char* pBuf, int nRead)
	{
		return m_resp->Read(pBuf, nRead);
	}
};

// 
class HttpStream
{
protected:
	NetClient::IHttpClient*	m_pHttpClient;
	int m_nReq;
public:
	HttpStream(NetClient::IHttpClient* pHttpClient)
		: m_pHttpClient(pHttpClient)
	{

	}
	~HttpStream()
	{

	}
	int GetErrorCode(int& nPos)
	{
		return m_pHttpClient->GetErrorCode(nPos);
	}
	
public:
	
	bool Post(const char* lpPath, const char* lpHeader, unsigned int nContentLen)
	{
		m_nReq = m_pHttpClient->Request_Post2(lpPath, lpHeader, nContentLen);
		return -1 != m_nReq;
	}

	int Write(const char* pData, int nLen)
	{
		return m_pHttpClient->SendBody(m_nReq, pData, nLen);
	}

	Response End()
	{
		auto resp = m_pHttpClient->EndRequest(m_nReq);
		return Response(resp);
	}
};


// http 客户端 实现  支持 多线程 也是 安全的 
class NetHttpClient
{
protected:
	NetClient::IHttpClient*	m_pHttpClient;
public:
	NetHttpClient()
		: m_pHttpClient(nullptr)
	{
		//fnCreateCrypto(&m_ICryptFun);
	}
	~NetHttpClient()
	{
		if(nullptr != m_pHttpClient)
			NetClient::fnDeleteHttpClient(m_pHttpClient);
	}
	bool IsValid()
	{
		return nullptr != m_pHttpClient;
	}
	int GetErrorCode(int& nPos)
	{
		return m_pHttpClient->GetErrorCode(nPos);
	}
public:
	bool Create(LPCTSTR lpIp, int nPort)
	{
		return NetClient::fnCreateHttpClient(&m_pHttpClient, lpIp, nPort);
	}
	void Reset()
	{
		if(nullptr != m_pHttpClient)
		{
			NetClient::fnDeleteHttpClient(m_pHttpClient);
			m_pHttpClient = nullptr;
		}		
	}
	Response Get(const char* lpPath, const char* lpHeader)
	{
		NetClient::IResponse* resp = m_pHttpClient->Request_Get(lpPath, lpHeader);
		return Response(resp);
	}

	RespReader GetFile(const char* lpPath, const char* lpHeader=nullptr)
	{
		NetClient::IRespReader* resp = m_pHttpClient->Request_GetFile(lpPath, lpHeader);
		return RespReader(resp);
	}

	Response Post(const char* lpPath, const char* lpHeader, const char* pData, int nLen)
	{
		NetClient::IResponse* resp = m_pHttpClient->Request_Post(lpPath, lpHeader, pData, nLen);
		return Response(resp);
	}

	HttpStream NewStream()
	{
		return HttpStream(m_pHttpClient);
	}
};

#endif