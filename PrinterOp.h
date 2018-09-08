#pragma once
#include <WinSpool.h>
#include <list>

class PrinterOp
{
public:
	// �ܼ���ʶ ��ʾ 
	enum class SecretMarkShow
	{
		HomePage = 0,			// ����ҳ
		AllPage = 1,				// ����ҳ
		NoPage = 2,				// ����ӡ
	};
	// �ܼ���ʶ λ��
	enum class SecretMarkSite
	{
		LeftTop = 1,
		LeftBottom,
		RightTop,
		RightBottom,
	};
	// ��ά�� ����Ϣ λ��
	enum class QrCodeSite
	{
		LeftTop = 1,
		CenterTop = 2,
		RightTop = 3,
		LeftBottom = 4,
		CenterBottom = 5,
		RightBottom = 6,
	};
	// ��ά�뼰��Ϣ ��ʾ
	enum class QrCodeShow
	{
		NoPage = 0,				// ����ӡ
		AllPage,						// ȫ��
		HomePage,					// ����ҳ
		EndPage,					// ��βҳ
		OddPage,					// ����ҳ
		EvenPage,					// ż��ҳ
	};

	struct SMarkParam			// �ܼ���ʶ ��ӡ ����
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
	struct WMarkParam			// ˮӡ ��ӡ����
	{
		std::wstring sText;
	};
	struct QrcodeParam			// ��ά�� ��ӡ����
	{
		QrCodeSite eSite;						// λ��
		QrCodeShow eQrcodeShow;			// ��ά����ʾ	
		float fScale;								// ���� 
		RECT rcMargin;							// �߿� �� λ�� ���Ӧ
		std::wstring sQrcode;					// ��ά�� ����
		QrCodeShow eTextShow;				// �ı���ʾ
		std::wstring sText;						// �ı�
		int nTextFontSize;						// �ı������С

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
	// ��̬����
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
	// ���� �ܼ���ʶ 
	bool DrawSecretMark(HDC hDC, RECT rcRect, SecretMarkSite eSite, const std::wstring& sText, HFONT hFont);
	bool DrawWaterMark(HDC hDC, RECT rcRect, const std::wstring& sText);
	bool DrawQrCodeInfo(HDC hDC, RECT rcRect, QrCodeSite eSite, const std::wstring& sQrcode, float fScale, const std::wstring& sText, HFONT hFont);
protected:
	struct PageInfo				// ÿҳ �� ����
	{
		int nPaperSize;			// 
		int nOrientation;			// ���� ���� �� ����
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

