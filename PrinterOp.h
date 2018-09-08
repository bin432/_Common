#pragma once
#include <WinSpool.h>
#include <list>

class PrinterOp
{
public:
	// 密级标识 显示 
	enum class SecretMarkShow
	{
		HomePage = 0,			// 仅首页
		AllPage = 1,				// 所有页
		NoPage = 2,				// 不打印
	};
	// 密级标识 位置
	enum class SecretMarkSite
	{
		LeftTop = 1,
		LeftBottom,
		RightTop,
		RightBottom,
	};
	// 二维码 及信息 位置
	enum class QrCodeSite
	{
		LeftTop = 1,
		CenterTop = 2,
		RightTop = 3,
		LeftBottom = 4,
		CenterBottom = 5,
		RightBottom = 6,
	};
	// 二维码及信息 显示
	enum class QrCodeShow
	{
		NoPage = 0,				// 不打印
		AllPage,						// 全部
		HomePage,					// 仅首页
		EndPage,					// 仅尾页
		OddPage,					// 奇数页
		EvenPage,					// 偶数页
	};

	struct SMarkParam			// 密级标识 打印 参数
	{
		SecretMarkShow eShow;
		SecretMarkSite eSite;
		std::wstring sText;
		SMarkParam()
		{
			eShow = SecretMarkShow::NoPage;
			eSite = SecretMarkSite::LeftTop;
		}
	};
	struct WMarkParam			// 水印 打印参数
	{
		std::wstring sText;
	};
	struct QrcodeParam			// 二维码 打印参数
	{
		QrCodeSite eSite;						// 位置
		QrCodeShow eQrcodeShow;			// 二维码显示	
		float fScale;								// 缩放 
		RECT rcMargin;							// 边框 和 位置 相对应
		std::wstring sQrcode;					// 二维码 内容
		QrCodeShow eTextShow;				// 文本显示
		std::wstring sText;						// 文本
		int nTextFontSize;						// 文本字体大小

		QrcodeParam()
		{
			eQrcodeShow = QrCodeShow::NoPage;
			eTextShow = QrCodeShow::NoPage;
			nTextFontSize = 8;
			rcMargin = {0, 0, 0, 0};
			fScale = 0.75;
		}
	};
	struct PrintParam
	{
		SMarkParam smParam;
		WMarkParam wmParam;
		QrcodeParam qrParam;
	};
	// 静态方法
	static QrCodeShow StrToQrCodeShow(LPCTSTR lpStr);	
	static std::wstring MakePrinterStatus(DWORD dwStatus);
	static std::wstring MakeJobStatus(DWORD dwStatus);
public:
	PrinterOp(LPCTSTR lpPrinter);
	~PrinterOp();

	struct PrintInfo
	{
		DWORD dwPrinterStatus;
		std::wstring sPrinterStatus;
		std::list<DWORD> jobStatusList;
	};
	bool GetPrintInfo(PrintInfo& info);
	bool GetPrinterStatus(std::wstring& sStatusText, DWORD& dwStatus);
	
	int Print(HWND hWnd, LPCTSTR lpJobFile, const PrintParam& param, std::function<bool(int,int)> fun=nullptr);
	int GetJobInfo(int nJobId, std::wstring& sDocName, DWORD& dwStatus, std::wstring& sOwner, SYSTEMTIME& time);
	int GetJobInfo(int nJobId, DWORD& dwStatus, DWORD& dwPrintedPage, DWORD& dwPrinterStatus);
	bool DeleteJob(int nJobId);
	LPCTSTR GetErrorText() { return m_sErrorText.c_str(); }
protected:
	
private:
	// 绘制 密级标识 
	bool DrawSecretMark(HDC hDC, RECT rcRect, SecretMarkSite eSite, const std::wstring& sText, HFONT hFont);
	bool DrawWaterMark(HDC hDC, RECT rcRect, const std::wstring& sText);
	bool DrawQrCodeInfo(HDC hDC, RECT rcRect, QrCodeSite eSite, const std::wstring& sQrcode, float fScale, const std::wstring& sText, HFONT hFont);
protected:
	struct PageInfo				// 每页 的 属性
	{
		int nPaperSize;			// 
		int nOrientation;			// 方向 横向 和 竖向
		std::wstring sEmfFile;
		std::wstring sPaperSizeName;
	};
	
	//std::wstring m_sPrinterName;
	WCHAR m_lpPrinterName[255];
	std::wstring m_sErrorText;

private:
	ULONG_PTR m_uGdiPlusToken;
	HMODULE m_hBCModule;
	typedef HBITMAP(_stdcall *FnMakePDF417)(LPVOID pData,
		DWORD dwDataBytes,
		unsigned int nRows,
		unsigned int nColumns,
		unsigned int nEccLevel,
		unsigned int nRotate,
		unsigned int xScale,
		unsigned int yScale, LPDWORD pError);
	FnMakePDF417 m_fnMakePDF417;
};

