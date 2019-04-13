#ifdef NETCLIENTDLL_EXPORTS
#define NETCLIENTDLL_API __declspec(dllexport)
#else
#define NETCLIENTDLL_API __declspec(dllimport)
#endif

namespace NetClient
{

// Http 返回 的 响应对象
class IResponse
{
public:
	virtual int GetStatusCode() = 0;
	virtual const char* GetStatusText() = 0;
	virtual const char* GetHeader(const char* lpKey) = 0;
	virtual const char* GetBody(int* pLen = nullptr) = 0;
};

// http 返回 的 读 
class IRespReader
{
public:
	virtual int GetStatusCode() = 0;
	virtual const char* GetStatusText() = 0;
	virtual const char* GetHeader(const char* lpKey) = 0;
	virtual int Read(char* pBuf, int nRead) = 0;
};

// 对数据进行 掩码 操作 
void NETCLIENTDLL_API MaskData(const unsigned char* pData, unsigned char* pDestData, int nLength);


// 
#define	WSE_START						1
#define	WSE_DIAL							2
#define	WSE_SEND							3
#define	WSE_CLOSE						4
///////////////////////////////////----IWebSocketClient------begin----///////////////////////////////////////
// 异步 的 websocket 客户端 事件 都 通过 IWebSocketListtener 在线程 里 返回
class IWebSocketClient
{
public:
	// nPos 上面宏 的 值
	// return 系统 错误代码
	virtual int GetErrorCode(int& nPos) = 0;

	//************************************
	// Method:    Dial
	// FullName:  NetClient::IWebSocketClient::Dial
	// Access:    virtual public 
	// Returns:   bool
	// Qualifier:
	// Parameter: const wchar_t * lpAddr		ip 
	// Parameter: int nPort	端口
	// Parameter: const wchar_t * path	http 服务器 地址  如 /ws		要带上 / 
	// Parameter: const char * lpHeader	可选的 http 头信息 如Sec-WebSocket-Protocol 或 自定义
	//************************************
	virtual bool Dial(const wchar_t* lpAddr, int nPort, const char* path, const char* lpHeader=nullptr) = 0;	// 连接
	
	// 关闭 发送 0x8 的 关闭帧 由服务端 主动 关闭 连接
	virtual void Close() = 0;	
	
	// 发送 数据 这个是 自动解决粘包的			ep:  这个是 异步的
	// bMask 是 表示 是否做 掩码操作
	// 还是系统 调用MaskData 后再调用这个接口。
	virtual bool Send(const unsigned char* pData, int nLen, bool bMask) = 0;
};

// 
class IWebSocketListener
{
public:
	virtual bool WebSocketShake(IResponse* response) = 0;
	virtual void WebSocketRecv(const unsigned char* lpData, unsigned int uLen) = 0;
	// nOperationCode 关闭类型 SO_UNKNOWN	= 0,	// Unknown
	//	SO_ACCEPT = 1,	// Acccept
	//	SO_CONNECT = 2,	// Connect
	//	SO_SEND = 3,	// Send
	//	SO_RECEIVE = 4,	// Receive
	//	SO_CLOSE = 5,	// Close
	// 8  websocket 关闭帧 nErrorOrStatus关闭状态
	// nErrorOrStatus错误代码或状态码
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
// 采用websocket 上传 下载 文件 的 客户端
class IWSFileClient
{
public:
	// nPos 上面宏 的 值
	// return 系统 错误代码
	virtual int GetErrorCode(int& nPos) = 0;
	virtual IResponse* Dial(const wchar_t* lpAddr, int nPort, const char* path, const char* lpHeader = nullptr) = 0;	// 连接
	virtual int Send(const unsigned char* pData, int nLen, bool bMask) = 0;		// 
	virtual int Recv(unsigned char* pData, int nLen, int nTimeoutSec=20) = 0;
	virtual void Close() = 0;	// 关闭
};

bool NETCLIENTDLL_API fnCreateWSFileClient(IWSFileClient** ppClient);
void NETCLIENTDLL_API fnDeleteWSFileClient(IWSFileClient* pClient);
///////////////////////////////////----IWSFileClient------end----///////////////////////////////////////


// GetErrorCode 中 nPos  的 值 方便 定位
#define HEP_GETSOCKADDR			1
#define HEP_SOCKET						2
#define HEP_KEEPALIVE					3
#define HEP_CONNECT					4
#define HEP_SENDHEADER				5
#define HEP_SENDBODY					6
#define HEP_TIMEOUT						7
#define HEP_RESPONSE					8

// http api接口方法 客户端 
class IHttpClient
{
public:
	// nPos 上面宏 的 值
	// return 系统 错误代码
	virtual int GetErrorCode(int& nPos) = 0;
	// 设置 超时 时间  默认 60 秒
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

