#ifdef NETCLIENTDLL_EXPORTS
#define NETCLIENTDLL_API __declspec(dllexport)
#else
#define NETCLIENTDLL_API __declspec(dllimport)
#endif

namespace NetClient
{

// Http ���� �� ��Ӧ����
class IResponse
{
public:
	virtual int GetStatusCode() = 0;
	virtual const char* GetStatusText() = 0;
	virtual const char* GetHeader(const char* lpKey) = 0;
	virtual const char* GetBody(int* pLen = nullptr) = 0;
};

// http ���� �� �� 
class IRespReader
{
public:
	virtual int GetStatusCode() = 0;
	virtual const char* GetStatusText() = 0;
	virtual const char* GetHeader(const char* lpKey) = 0;
	virtual int Read(char* pBuf, int nRead) = 0;
};

// �����ݽ��� ���� ���� 
void NETCLIENTDLL_API MaskData(const unsigned char* pData, unsigned char* pDestData, int nLength);


// 
#define	WSE_START						1
#define	WSE_DIAL							2
#define	WSE_SEND							3
#define	WSE_CLOSE						4
///////////////////////////////////----IWebSocketClient------begin----///////////////////////////////////////
// �첽 �� websocket �ͻ��� �¼� �� ͨ�� IWebSocketListtener ���߳� �� ����
class IWebSocketClient
{
public:
	// nPos ����� �� ֵ
	// return ϵͳ �������
	virtual int GetErrorCode(int& nPos) = 0;

	//************************************
	// Method:    Dial
	// FullName:  NetClient::IWebSocketClient::Dial
	// Access:    virtual public 
	// Returns:   bool
	// Qualifier:
	// Parameter: const wchar_t * lpAddr		ip 
	// Parameter: int nPort	�˿�
	// Parameter: const wchar_t * path	http ������ ��ַ  �� /ws		Ҫ���� / 
	// Parameter: const char * lpHeader	��ѡ�� http ͷ��Ϣ ��Sec-WebSocket-Protocol �� �Զ���
	//************************************
	virtual bool Dial(const wchar_t* lpAddr, int nPort, const char* path, const char* lpHeader=nullptr) = 0;	// ����
	
	// �ر� ���� 0x8 �� �ر�֡ �ɷ���� ���� �ر� ����
	virtual void Close() = 0;	
	
	// ���� ���� ����� �Զ����ճ����			ep:  ����� �첽��
	// bMask �� ��ʾ �Ƿ��� �������
	// ����ϵͳ ����MaskData ���ٵ�������ӿڡ�
	virtual bool Send(const unsigned char* pData, int nLen, bool bMask) = 0;
};

// 
class IWebSocketListener
{
public:
	virtual bool WebSocketShake(IResponse* response) = 0;
	virtual void WebSocketRecv(const unsigned char* lpData, unsigned int uLen) = 0;
	// nOperationCode �ر����� SO_UNKNOWN	= 0,	// Unknown
	//	SO_ACCEPT = 1,	// Acccept
	//	SO_CONNECT = 2,	// Connect
	//	SO_SEND = 3,	// Send
	//	SO_RECEIVE = 4,	// Receive
	//	SO_CLOSE = 5,	// Close
	// 8  websocket �ر�֡ nErrorOrStatus�ر�״̬
	// nErrorOrStatus��������״̬��
	virtual void WebSocketClose(int nOperationCode, int nErrorOrStatus) = 0;
};

bool NETCLIENTDLL_API fnCreateWebSocketClient(IWebSocketClient** ppClient, IWebSocketListener* fun);
void NETCLIENTDLL_API fnDeleteWebSocketClient(IWebSocketClient* pClient);
///////////////////////////////////----IWebSocketClient------end----///////////////////////////////////////



//
#define WSP_GETSOCKADDR			1
#define WSP_SOCKET						2
#define WSP_KEEPALIVE					3
#define WSP_CONNECT					4
#define WSP_SENDSHAKE				5
#define WSP_TIMEOUT					6
#define WSP_RESPONSE					7

///////////////////////////////////----IWSFileClient------begin----///////////////////////////////////////
// ����websocket �ϴ� ���� �ļ� �� �ͻ���
class IWSFileClient
{
public:
	// nPos ����� �� ֵ
	// return ϵͳ �������
	virtual int GetErrorCode(int& nPos) = 0;
	virtual IResponse* Dial(const wchar_t* lpAddr, int nPort, const char* path, const char* lpHeader = nullptr) = 0;	// ����
	virtual int Send(const unsigned char* pData, int nLen, bool bMask) = 0;		// 
	virtual int Recv(unsigned char* pData, int nLen, int nTimeoutSec=20) = 0;
	virtual void Close() = 0;	// �ر�
};

bool NETCLIENTDLL_API fnCreateWSFileClient(IWSFileClient** ppClient);
void NETCLIENTDLL_API fnDeleteWSFileClient(IWSFileClient* pClient);
///////////////////////////////////----IWSFileClient------end----///////////////////////////////////////


// GetErrorCode �� nPos  �� ֵ ���� ��λ
#define HEP_GETSOCKADDR			1
#define HEP_SOCKET						2
#define HEP_KEEPALIVE					3
#define HEP_CONNECT					4
#define HEP_SENDHEADER				5
#define HEP_SENDBODY					6
#define HEP_TIMEOUT						7
#define HEP_RESPONSE					8

// http api�ӿڷ��� �ͻ��� 
class IHttpClient
{
public:
	// nPos ����� �� ֵ
	// return ϵͳ �������
	virtual int GetErrorCode(int& nPos) = 0;
	// ���� ��ʱ ʱ��  Ĭ�� 60 ��
	virtual void SetTimeout(DWORD dwSecond) = 0;
	virtual IResponse* Request_Get(const char* lpPath, const char* lpHeader = nullptr) = 0;
	virtual IRespReader* Request_GetFile(const char* lpPath, const char* lpHeader = nullptr) = 0;

	virtual IResponse* Request_Post(const char* lpPath, const char* lpHeader, const char* pData, int nLen) = 0;
	virtual int Request_Post2(const char* lpPath, const char* lpHeader, unsigned int nContentLen) = 0;
	virtual int SendBody(int nReq, const char* pData, int nLen) = 0;
	virtual IResponse* EndRequest(int nReq) = 0;

	virtual void Close() = 0;
};


bool NETCLIENTDLL_API fnCreateHttpClient(IHttpClient** ppClient, const wchar_t* lpHost, int nPort);
void NETCLIENTDLL_API fnDeleteHttpClient(IHttpClient* pClient);
void NETCLIENTDLL_API fnDeleteObject(void* p);

}// end namespace

