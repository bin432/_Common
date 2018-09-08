#include "stdafx.h"
#include "PrinterOp.h"
#pragma comment(lib, "Winspool.lib")
#include <gdiplus.h>
#pragma comment(lib, "Gdiplus.lib")

#include <vector>
#include "DeferOp.hpp"
#include "EtimesPathFun.hpp"
#include "CodingConv.hpp"
#include "StrFormat.hpp"
#include "../common/JobIniFileOp.hpp"

//#include "EtimesManagerApp.h"

PrinterOp::QrCodeShow PrinterOp::StrToQrCodeShow(LPCTSTR lpStr)
{
	QrCodeShow eShow = QrCodeShow::AllPage;
	std::wstring s(lpStr);
	if (0 == s.compare(L"All"))
		return QrCodeShow::AllPage;
	
	if (0 == s.compare(L"Head"))
		return QrCodeShow::HomePage;

	if (0 == s.compare(L"Tail"))
		return QrCodeShow::EndPage;

	if (0 == s.compare(L"Odd"))
		return QrCodeShow::OddPage;

	if (0 == s.compare(L"Even"))
		return QrCodeShow::EvenPage;

	if (0 == s.compare(L"Blank"))
		return QrCodeShow::NoPage;
	
	return eShow;
}
std::wstring PrinterOp::MakePrinterStatus(DWORD dwStatus)
{
	if (0 == dwStatus) return L"";

	std::wstring sStatusText;
	if (PRINTER_STATUS_PAUSED & dwStatus)
		sStatusText += L"��ӡ������ͣ,";
	if (PRINTER_STATUS_ERROR & dwStatus)
		sStatusText += L"��ӡ������,";
	if (PRINTER_STATUS_PENDING_DELETION & dwStatus)
		sStatusText += L"����ɾ��,";
	if (PRINTER_STATUS_PAPER_JAM & dwStatus)
		sStatusText += L"��ӡ����ֽ,";
	if (PRINTER_STATUS_PAPER_OUT  & dwStatus)
		sStatusText += L"��ӡ��ȱֽ,";
	if (PRINTER_STATUS_MANUAL_FEED & dwStatus)
		sStatusText += L"��ӡ�������ֶ���ֽ״̬,";
	if (PRINTER_STATUS_PAPER_PROBLEM & dwStatus)
		sStatusText += L"��ӡ��ֽ��������,";
	if (PRINTER_STATUS_OFFLINE & dwStatus)
		sStatusText += L"��ӡ���ѻ�,";
	if (PRINTER_STATUS_IO_ACTIVE & dwStatus)
		sStatusText += L"��ӡ�����ڻ�������״̬,";
	if (PRINTER_STATUS_BUSY & dwStatus)
		sStatusText += L"��ӡ��æ,";
	if (PRINTER_STATUS_PRINTING & dwStatus)
		sStatusText += L"��ӡ�����ڽ��д�ӡ,";
	if (PRINTER_STATUS_OUTPUT_BIN_FULL & dwStatus)
		sStatusText += L"��ӡ�������ֽ������,";
	if (PRINTER_STATUS_NOT_AVAILABLE & dwStatus)
		sStatusText += L"��ӡ��������,";
	if (PRINTER_STATUS_WAITING & dwStatus)
		sStatusText += L"��ӡ����ֽ,";
	if (PRINTER_STATUS_PROCESSING & dwStatus)
		sStatusText += L"��ӡ�����ڴ����ӡ��ҵ,";
	if (PRINTER_STATUS_INITIALIZING & dwStatus)
		sStatusText += L"��ӡ�����ڳ�ʼ��,";
	if (PRINTER_STATUS_WARMING_UP & dwStatus)
		sStatusText += L"��ӡ������Ԥ��,";
	if (PRINTER_STATUS_TONER_LOW & dwStatus)
		sStatusText += L"��ӡ��ī����,";
	if (PRINTER_STATUS_NO_TONER & dwStatus)
		sStatusText += L"��ӡ��ȱī,";
	if (PRINTER_STATUS_PAGE_PUNT & dwStatus)
		sStatusText += L"��ӡ���޷���ӡ��ǰҳ,";
	if (PRINTER_STATUS_USER_INTERVENTION & dwStatus)
		sStatusText += L"��ӡ��Ҫ���û���Ԥ,";
	if (PRINTER_STATUS_OUT_OF_MEMORY & dwStatus)
		sStatusText += L"��ӡ���ڴ����,";
	if (PRINTER_STATUS_DOOR_OPEN & dwStatus)
		sStatusText += L"��ӡ�����ű���,";
	if (PRINTER_STATUS_SERVER_UNKNOWN & dwStatus)
		sStatusText += L"��ӡ����δ֪����,";
	if (PRINTER_STATUS_POWER_SAVE & dwStatus)
		sStatusText += L"��ӡ�����ڵ�Դ����ģʽ,";
	if (PRINTER_STATUS_SERVER_OFFLINE & dwStatus)
		sStatusText += L"��ӡ��������,";
	if (PRINTER_STATUS_DRIVER_UPDATE_NEEDED & dwStatus)
		sStatusText += L"��ӡ����Ҫ��������,";

	if (sStatusText.empty())
	{
		sStatusText = L"��ӡ���쳣";
		sStatusText.append(L".");
	}
	else
	{
		sStatusText.at(sStatusText.length() - 1) = '.';
	}
	return sStatusText;
}
std::wstring PrinterOp::MakeJobStatus(DWORD dwStatus)
{
	std::wstring sStatusText;
	if (JOB_STATUS_PAUSED & dwStatus)
		sStatusText += L"����ͣ-";
	if (JOB_STATUS_ERROR & dwStatus)
		sStatusText += L"����-";
	if (JOB_STATUS_DELETING & dwStatus)
		sStatusText += L"����ɾ��-";
	if (JOB_STATUS_SPOOLING & dwStatus)
		sStatusText += L"�ѻ�-";
	if (JOB_STATUS_PRINTING  & dwStatus)
		sStatusText += L"���ڴ�ӡ-";
	if (JOB_STATUS_OFFLINE & dwStatus)
		sStatusText += L"����-";
	if (JOB_STATUS_PAPEROUT & dwStatus)
		sStatusText += L"ûֽ����-";
	if (JOB_STATUS_PRINTED & dwStatus)
		sStatusText += L"�Ѵ�ӡ-";
	if (JOB_STATUS_DELETED & dwStatus)
		sStatusText += L"��ɾ��-";
	if (JOB_STATUS_BLOCKED_DEVQ & dwStatus)
		sStatusText += L"��������-";
	if (JOB_STATUS_USER_INTERVENTION & dwStatus)
		sStatusText += L"��Ҫ��Ϊ����-";
	if (JOB_STATUS_RESTART & dwStatus)
		sStatusText += L"���´�ӡ-";
	
	if (!sStatusText.empty())
	{
		sStatusText.at(sStatusText.length() - 1) = ' ';
	}
	return sStatusText;
}

PrinterOp::PrinterOp(LPCTSTR lpPrinter)
	: m_fnMakePDF417(nullptr)
{
	wcscpy_s(m_lpPrinterName, 255, lpPrinter);

	Gdiplus::GdiplusStartupInput gdiplusStartupInput;	
	Gdiplus::GdiplusStartup(&m_uGdiPlusToken, &gdiplusStartupInput, NULL);

	auto sPath = EtimesPathFun::GetAppPath();
	sPath.append(_T("BCEncode.dll"));
	m_hBCModule = ::LoadLibrary(sPath.c_str());
	m_fnMakePDF417 = (FnMakePDF417)::GetProcAddress(m_hBCModule, "MakePDF417");
}
PrinterOp::~PrinterOp()
{
	if (NULL != m_hBCModule)
	{
		FreeLibrary(m_hBCModule);
	}
	Gdiplus::GdiplusShutdown(m_uGdiPlusToken);
	m_uGdiPlusToken = 0;
}
bool PrinterOp::GetPrintInfo(PrintInfo& info)
{
	HANDLE hPrinter = NULL;
	if (!::OpenPrinterW(m_lpPrinterName, &hPrinter, NULL))
	{
		DWORD dwErr = GetLastError();
		if (ERROR_INVALID_PRINTER_NAME == dwErr)
			m_sErrorText = L"��Ч�Ĵ�ӡ������";
		else
			StrFormat::Format(m_sErrorText, _T("OpenPrinterW err %d"), dwErr);
		return false;
	}
	Defer(if (NULL != hPrinter) { ClosePrinter(hPrinter); hPrinter = NULL; });

	DWORD dwNeed = 0;
	if (!::GetPrinterW(hPrinter, 2, nullptr, 0, &dwNeed))
	{
		DWORD dwErr = GetLastError();
		if (ERROR_INSUFFICIENT_BUFFER != dwErr)
		{
			StrFormat::Format(m_sErrorText, _T("GetPrinterW_1 err %d"), dwErr);
			return false;
		}
	}
	PRINTER_INFO_2W* printerInfo = (PRINTER_INFO_2W*)malloc(dwNeed);
	if (nullptr == printerInfo)
	{
		StrFormat::Format(m_sErrorText, _T("malloc err %d"), GetLastError());
		return false;
	}
	Defer(free(printerInfo); printerInfo = nullptr;);

	DWORD dwUsed = 0;
	if (!::GetPrinterW(hPrinter, 2, (LPBYTE)printerInfo, dwNeed, &dwUsed))
	{
		StrFormat::Format(m_sErrorText, _T("GetPrinterW_2 err %d"), GetLastError());
		return false;
	}

	if (PRINTER_ATTRIBUTE_WORK_OFFLINE & printerInfo->Attributes)
	{
		m_sErrorText = _T("��ӡ���ѻ�");
		return false;
	}

	info.dwPrinterStatus = printerInfo->Status;
	if (NO_ERROR == printerInfo->Status)
	{
		
	}
	else
	{
		info.sPrinterStatus = MakePrinterStatus(info.dwPrinterStatus);
	}

	
	// ö�� ���� 
	dwNeed = 0;
	DWORD dwJobCount = 0;
	if (!::EnumJobsW(hPrinter, 0, 0xFFFF, 1, NULL, 0, &dwNeed, &dwJobCount))
	{
		DWORD dwErr = GetLastError();
		if (ERROR_INSUFFICIENT_BUFFER != dwErr)
		{
			StrFormat::Format(m_sErrorText, _T("EnumJobsW err %d"), GetLastError());
			return false;
		}
	}

	JOB_INFO_1W* jobList = (JOB_INFO_1W*)malloc(dwNeed);
	if (nullptr == jobList)
	{
		StrFormat::Format(m_sErrorText, _T("malloc err %d"), GetLastError());
		return false;
	}
	Defer(free(jobList););

	if (!::EnumJobsW(hPrinter, 0, 0xFFFF, 1, (LPBYTE)jobList, dwNeed, &dwNeed, &dwJobCount))
	{
		StrFormat::Format(m_sErrorText, _T("EnumJobsW2 err %d"), GetLastError());
		return false;
	}
//#ifdef _DEBUG
//	auto& job0 = jobList[0];
//	auto& job1 = jobList[1];
//#endif // _DEBUG
	
	for (int i = 0; i < dwJobCount; ++i)
	{
		info.jobStatusList.push_back(jobList[i].Status);
	}

	return true;
}
bool PrinterOp::GetPrinterStatus(std::wstring& sStatusText, DWORD& dwStatus)
{
	HANDLE hPrinter = NULL;
	if (!::OpenPrinterW(m_lpPrinterName, &hPrinter, NULL))
	{
		DWORD dwErr = GetLastError();
		if (ERROR_INVALID_PRINTER_NAME == dwErr)
			m_sErrorText = L"��Ч�Ĵ�ӡ������";
		else
			StrFormat::Format(m_sErrorText, _T("OpenPrinterW err %d"), dwErr);
	}
	Defer(if (NULL != hPrinter) { ClosePrinter(hPrinter); hPrinter = NULL; });
	DWORD dwNeed = 0;
	if (!::GetPrinterW(hPrinter, 2, nullptr, 0, &dwNeed))
	{
		DWORD dwErr = GetLastError();
		if (ERROR_INSUFFICIENT_BUFFER != dwErr)
		{
			StrFormat::Format(m_sErrorText, _T("GetPrinterW_1 err %d"), dwErr);
			return false;
		}
	}
	PRINTER_INFO_2W* printerInfo = (PRINTER_INFO_2W*)malloc(dwNeed);
	if (nullptr == printerInfo)
	{
		StrFormat::Format(m_sErrorText, _T("malloc err %d"), GetLastError());
		return false;
	}
	Defer(free(printerInfo); printerInfo = nullptr;);

	DWORD dwUsed = 0;
	if (!::GetPrinterW(hPrinter, 2, (LPBYTE)printerInfo, dwNeed, &dwUsed))
	{
		StrFormat::Format(m_sErrorText, _T("GetPrinterW_2 err %d"), GetLastError());
		return false;
	}

	if (PRINTER_ATTRIBUTE_WORK_OFFLINE & printerInfo->Attributes)
	{
		m_sErrorText = _T("��ӡ���ѻ�");
		return false;
	}

	dwStatus = printerInfo->Status;
	if (NO_ERROR == printerInfo->Status)
	{
		return true;
	}

	sStatusText = MakePrinterStatus(dwStatus);

	return true;
}

int PrinterOp::Print(HWND hWnd, LPCTSTR lpJobFile, const PrintParam& param, std::function<bool(int, int)> fun)
{
	if (!PathFileExists(lpJobFile))
	{
		m_sErrorText = _T("lpJobFile file is not Exists");
		return -1;
	}
	
	std::wstring sJobFileDir(lpJobFile, PathFindFileName(lpJobFile)- lpJobFile);
	// ׼������
	JobIniFileOp iniFile(lpJobFile);
	int nPaperSize = iniFile.GetPaperSize();
	int nOrientation = iniFile.GetOrientation();
	//int nCollate = iniFile.GetCollate();
	int nCollate = 1;			// ǿ�� ���ó� ��ݴ�ӡ
	SStringT sPaperSizeName = iniFile.GetPaperSizeName();

	std::vector<PageInfo*> vctPages;
	SStringT sTempPaper = sPaperSizeName;
	int nTempPaperSize = nPaperSize;
	int nTempOrientation = nOrientation;
	int nCount = iniFile.GetEMFCount();
	for(int i=0; i<nCount; ++i)
	{
		sTempPaper = iniFile.GetExtraPaperSizeName(i, sTempPaper);
		nTempPaperSize = iniFile.GetExtraPaperSize(i, nTempPaperSize);
		nTempOrientation = iniFile.GetExtraOrientation(i, nTempOrientation);
		PageInfo* page = new PageInfo;
		page->sPaperSizeName = sTempPaper;
		page->nPaperSize = nTempPaperSize;
		page->nOrientation = nTempOrientation;
		
		page->sEmfFile = sJobFileDir;
		page->sEmfFile.append(iniFile.GetEMFFile(i));

		vctPages.push_back(page);
	}
	Defer(for (int i = 0; i < nCount; ++i)delete vctPages.at(i););
	
	HANDLE hPrinter = NULL;
	if (!::OpenPrinterW(m_lpPrinterName, &hPrinter, NULL))
	{
		DWORD dwErr = GetLastError();
		if (ERROR_INVALID_PRINTER_NAME == dwErr)
			m_sErrorText = L"��Ч�Ĵ�ӡ������";
		else
			StrFormat::Format(m_sErrorText, _T("OpenPrinterW err %d"), dwErr);
		return -1;
	}
	Defer(if (NULL != hPrinter) {::ClosePrinter(hPrinter); hPrinter = NULL; });
	
	LONG lLen = ::DocumentPropertiesW(hWnd, hPrinter, m_lpPrinterName, 0, 0, 0);
	if (lLen <= 0)
	{
		StrFormat::Format(m_sErrorText, _T("DocumentPropertiesW err %d"), GetLastError());
		return -1;
	}

	DEVMODEW* pDevMode = (DEVMODEW*)malloc(lLen);
	if (nullptr == pDevMode)
	{
		StrFormat::Format(m_sErrorText, _T("malloc err %d"), GetLastError());
		return -1;
	}
	Defer(free(pDevMode););
	::DocumentPropertiesW(hWnd, hPrinter, m_lpPrinterName, pDevMode, nullptr, DM_OUT_BUFFER);
		
	if (!sPaperSizeName.IsEmpty())
	{
		wcsncpy_s(pDevMode->dmFormName, CCHFORMNAME, sPaperSizeName, _TRUNCATE);
	}

	pDevMode->dmCopies = 1;
	pDevMode->dmDuplex = iniFile.GetDuplex();
	pDevMode->dmPaperSize = nPaperSize;
	pDevMode->dmPaperWidth = iniFile.GetPaperWidth();
	pDevMode->dmPaperLength = iniFile.GetPaperLength();
	pDevMode->dmOrientation = nOrientation;

	::DocumentPropertiesW(hWnd, hPrinter, m_lpPrinterName, pDevMode, pDevMode, DM_IN_BUFFER|DM_OUT_BUFFER);

	//theLog->Debug_0(_T("PrinterOp - Print - CreateDC Begin"));
	HDC hPrinterDc = NULL;
	try 
	{
		//EnumDisplayDevicesW()
		// ���� ��ӡ�� dc 
		hPrinterDc = ::CreateDCW(L"WINSPOOL", m_lpPrinterName, NULL, pDevMode);
		if (NULL == hPrinterDc)
		{
			StrFormat::Format(m_sErrorText, _T("CreateDCW err %d"), GetLastError());
			return -1;
		}
	}
	catch (...)
	{
		//theLog->Debug_0(_T("PrinterOp - Print - CreateDC-3"));
		StrFormat::Format(m_sErrorText, _T("CreateDCW catch %d"), GetLastError());
		//theLog->Debug_0(_T("PrinterOp - Print - CreateDC-4"));
		return -2;
	}
	//theLog->Debug_0(_T("PrinterOp - Print - CreateDC End"));
	Defer(::ReleaseDC(NULL, hPrinterDc); ::DeleteDC(hPrinterDc););
		
	std::wstring sDocName = iniFile.GetJobName();
	DOCINFOW docInfoW;
	memset(&docInfoW, 0, sizeof(DOCINFOW));
	docInfoW.cbSize = sizeof(DOCINFOW);
	docInfoW.lpszDatatype = L"raw";
	docInfoW.lpszDocName = sDocName.c_str();			// ���� Ҫ��֤ �ַ���ָ���� ��Χ�� ��Ч
	if (!::SetMapMode(hPrinterDc, MM_TEXT))
	{
		OutputDebugString(_T("PrinterOp::Print - SetMapMode err"));
	}
		
	int nJobId = ::StartDocW(hPrinterDc, &docInfoW);
	if (nJobId <= 0)
	{
		DWORD dwErr = GetLastError();
		if (ERROR_CANCELLED == dwErr)
			m_sErrorText = L"�û�ȡ���˴�ӡ��";
		else
			StrFormat::Format(m_sErrorText, _T("StartDocW err %d"), GetLastError());
		return nJobId;
	}

	// ���� ���� 
	HFONT hFont = CreateFontW(100, 0, 0, 0, 0,
		FALSE, FALSE, 0, ANSI_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"����");

	LONG nQrFontH = abs(MulDiv(param.qrParam.nTextFontSize, GetDeviceCaps(hPrinterDc, LOGPIXELSY), 72));
	if (0 == nQrFontH) nQrFontH = 72;
	HFONT hQrFont = CreateFontW(nQrFontH, 0, 0, 0, 0,
		FALSE, FALSE, 0, ANSI_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"����");

	
	// ����ֽ�ŵĸ߶�����(��λ���豸��λ)
	const int nPaperW = ::GetDeviceCaps(hPrinterDc, PHYSICALWIDTH);
	const int nPaperH = ::GetDeviceCaps(hPrinterDc, PHYSICALHEIGHT);

	// ����ֽ�ŵı߾�(��λ���豸��λ���� rclBounds ��λ��ͬ)
	const int nPaperMarginX = ::GetDeviceCaps(hPrinterDc, PHYSICALOFFSETX);
	const int nPaperMarginY = ::GetDeviceCaps(hPrinterDc, PHYSICALOFFSETY);

	// �ɴ�ӡ����ĸ߶�����(��λ������)
	const int nPixelsW = ::GetDeviceCaps(hPrinterDc, HORZRES);
	const int nPixelsH = ::GetDeviceCaps(hPrinterDc, VERTRES);

	// emf �ļ���ӡ Ҫ ���� �߾� 
	RECT rcEmfRect = {-nPaperMarginX, -nPaperMarginY, nPixelsW + nPaperMarginX, nPixelsH + nPaperMarginY};
	// �Զ���� ֱ����
	RECT rcOtherRect = {0, 0, nPixelsW, nPixelsH};


#if 0
	// ÿ���豸��λ�����ظ���(�ɴ�ӡ���ߴ����������ߴ��ȥ�����߾�)
	const double aDuppX = nPixelsW / ((double)nPaperW - nPaperMarginX * 2.0);
	const double aDuppY = nPixelsH / ((double)nPaperH - nPaperMarginY * 2.0);

	// ÿӢ���Ӧ�����ظ���
	const int nPixelsPeerInchX = ::GetDeviceCaps(hPrinterDc, LOGPIXELSX);
	const int nPixelsPeerInchY = ::GetDeviceCaps(hPrinterDc, LOGPIXELSY);

	// ����߾������(����ֽ�Ų��ɴ�ӡ�ı߾࣬ȡ��������)
	const int aPixelsEdgeW = (int)(aDuppX * nPaperMarginX + 0.5);
	const int aPixelsEdgeH = (int)(aDuppY * nPaperMarginY + 0.5);

	// ��λת������(0.01���� -> Ӣ���ת������)
	const double aMM001_2Inch = 0.0393700787 / 100.0;

	// ��λת������(0.01���� -> �豸��λ��ת��)
	const double dbMM001_2DuX = (aMM001_2Inch * nPixelsPeerInchX) / aDuppX;
	const double dbMM001_2DuY = (aMM001_2Inch * nPixelsPeerInchY) / aDuppY;
#endif

	int nPages = vctPages.size();
	int nCopies = iniFile.GetCopies();
	// �� ��� ˫���ӡ �� �� ���� ҳ�� ��Ҫ ��ӡһ�ſհ�ҳ
	const bool bPrintBlankPage = (iniFile.GetDuplex() != DMDUP_SIMPLEX) && (1 == (nPages % 2)) && (nCopies > 1);
	
	auto& qr = param.qrParam; // ��ά����� ���� 
	
	std::wstring sHeadQrcode = qr.sQrcode;
	sHeadQrcode  += nCollate ? L"/Z" : L"/S";
	std::wstring sHeadQrText = qr.sText;
	sHeadQrText += nCollate ? L"/Z" : L"/S";

	

	// ���� page 
	auto FnDrawItem = [&](int nPage, int nCopie, LPCTSTR lpEMF) -> bool
	{
		int nRet = ::StartPage(hPrinterDc);
		if (nRet <= 0)
		{
			return false;
		}

		// ���� EMF ԭʼ �ļ�����
		HENHMETAFILE hEmf = ::GetEnhMetaFileW(lpEMF);
		if (NULL == hEmf)
		{
			::EndPage(hPrinterDc);
			return false;
		}
	#if 0
		ENHMETAHEADER emfHeader;
		RECT rcRect = {0, 0, 0, 0};
		memset(&emfHeader, 0, sizeof(ENHMETAHEADER));
		emfHeader.nSize = sizeof(ENHMETAHEADER);
		UINT uRet = ::GetEnhMetaFileHeader(hEmf, sizeof(ENHMETAHEADER), &emfHeader);
		rcRect.left = (LONG)(emfHeader.rclFrame.left * dbMM001_2DuX);
		rcRect.top = (LONG)(emfHeader.rclFrame.top * dbMM001_2DuY);
		rcRect.right = (LONG)(emfHeader.rclFrame.right * dbMM001_2DuX);
		rcRect.bottom = (LONG)(emfHeader.rclFrame.bottom * dbMM001_2DuY);
		
		OffsetRect(&rcRect, -nPaperMarginX, -nPaperMarginY);				
	#endif
		
		if (!::PlayEnhMetaFile(hPrinterDc, hEmf, &rcEmfRect))
		{
			OutputDebugString(_T("PrinterOp-Print-PlayEnhMetaFile Err"));
		}

		// ���� ���� ��ʼ ���� �Զ�������
		//OffsetRect(&rcRect, nPaperMarginX, nPaperMarginY);
		//rcRect.left += 2 * nPaperMarginX;
		//rcRect.top += 2 * nPaperMarginY;
		//rcRect.right -= nPaperMarginX;
		//rcRect.bottom -= nPaperMarginY;

		// ���� �ܼ���ʶ
		if ((SecretMarkShow::AllPage == param.smParam.eShow) ||
			((SecretMarkShow::HomePage == param.smParam.eShow) && (1 == nPage))
			)
		{
			DrawSecretMark(hPrinterDc, rcOtherRect, param.smParam.eSite, param.smParam.sText, hFont);
		}

		// ���� ˮӡ
		if (!param.wmParam.sText.empty())
		{
			DrawWaterMark(hPrinterDc, rcOtherRect, param.wmParam.sText);
		}

		// ���ƶ�ά�뼰��ά���ı�
		std::wstring sQrcode;
		if (QrCodeShow::AllPage == qr.eQrcodeShow)
		{
			StrFormat::Format(sQrcode, L"%s/%d/%d", sHeadQrcode.c_str(), nCopie, nPage);
		}
		else if ((QrCodeShow::HomePage == qr.eQrcodeShow) && (1 == nPage))
		{
			StrFormat::Format(sQrcode, L"%s/%d/T/%d", sHeadQrcode.c_str(), nCopie, nPages);
		}
		else if ((QrCodeShow::EndPage == qr.eQrcodeShow) && (nPages == nPage))
		{
			StrFormat::Format(sQrcode, L"%s/%d/T/%d", sHeadQrcode.c_str(), nCopie, nPages);
		}
		else if ((QrCodeShow::OddPage == qr.eQrcodeShow) && (1 == (nPage % 2)))
		{
			StrFormat::Format(sQrcode, L"%s/%d/O/%d", sHeadQrcode.c_str(), nCopie, nPage);
		}
		else if ((QrCodeShow::EvenPage == qr.eQrcodeShow) && (0 == (nPage % 2)))
		{
			StrFormat::Format(sQrcode, L"%s/%d/E/%d", sHeadQrcode.c_str(), nCopie, nPage);
		}

		std::wstring sText;			// ��ά�� �ı�
		if (QrCodeShow::AllPage == qr.eTextShow)
		{
			StrFormat::Format(sText, L"%s/����:%d/ҳ��:%d", sHeadQrText.c_str(), nCopie, nPage);
		}
		else if ((QrCodeShow::HomePage == qr.eTextShow) && (1 == nPage))
		{
			StrFormat::Format(sText, L"%s/����:%d/T/��ҳ��:%d", sHeadQrText.c_str(), nCopie, nPages);
		}
		else if ((QrCodeShow::EndPage == qr.eTextShow) && (nPages == nPage))
		{
			StrFormat::Format(sText, L"%s/����:%d/T/��ҳ��:%d", sHeadQrText.c_str(), nCopie, nPages);
		}
		else if ((QrCodeShow::OddPage == qr.eTextShow) && (1 == (nPage % 2)))
		{
			StrFormat::Format(sText, L"%s/����:%d/O/ҳ��:%d", sHeadQrText.c_str(), nCopie, nPage);
		}
		else if ((QrCodeShow::EvenPage == qr.eTextShow) && (0 == (nPage % 2)))
		{
			StrFormat::Format(sText, L"%s/����:%d/E/ҳ��:%d", sHeadQrText.c_str(), nCopie, nPage);
		}

		// ���� �߾�
		rcOtherRect.left += qr.rcMargin.left;
		rcOtherRect.top += qr.rcMargin.top;
		rcOtherRect.right -= qr.rcMargin.right;
		rcOtherRect.bottom -= qr.rcMargin.bottom;

		DrawQrCodeInfo(hPrinterDc, rcOtherRect, param.qrParam.eSite, sQrcode, qr.fScale, sText, hQrFont);

		::EndPage(hPrinterDc);
		::DeleteEnhMetaFile(hEmf);
		return true;
	};
		
	// ����Ĭ�� ��ݴ�ӡ
	for (int nCopie=1; nCopie<=nCopies; ++nCopie)
	{
		for(int nPage=1; nPage<=nPages; ++nPage)
		{
			if (nullptr != fun && !fun(nPage, nCopie))
				goto end;			// ����ȡ�� ��ӡ�� ����ֱ���˳�

			auto paInfo = vctPages.at(nPage-1);
			if (paInfo->nOrientation != pDevMode->dmOrientation)	// �ĵ���������������ʱ����������ʵʱ�л���ӡ����
			{
				//theLog->Debug(_T("ResetDC page %d ori %d, DM ori %d"), nPage, paInfo->nOrientation, pDevMode->dmOrientation);
				pDevMode->dmOrientation = paInfo->nOrientation;
				//pDevMode->dmFields |= DM_ORIENTATION;
				if (!ResetDCW(hPrinterDc, pDevMode))
				{
					OutputDebugString(_T("ResetDC err"));
					//theLog->Debug(_T("ResetDC page %d err %d"), nPage, GetLastError());
				}
			}
			FnDrawItem(nPage, nCopie, paInfo->sEmfFile.c_str());
		}

		if (bPrintBlankPage)			// ��ӡ�հ�ҳ
		{
			::StartPage(hPrinterDc);
			::EndPage(hPrinterDc);
		}
	}


end:
	int i = ::EndDoc(hPrinterDc);

	if (NULL != hFont)
	{
		DeleteObject(hFont);
		hFont = NULL;
	}

	if (NULL != hQrFont)
	{
		DeleteObject(hQrFont);
		hQrFont = NULL;
	}

	return nJobId;
}

bool PrinterOp::DrawSecretMark(HDC hDC, RECT rcRect, SecretMarkSite eSite, const std::wstring& sText, HFONT hFont)
{
	if (sText.empty())
		return false;

	HFONT hOldFont = (HFONT)SelectObject(hDC, hFont);
	RECT rcText = rcRect;
	RECT rcClac = {0, 0, rcRect.right, rcRect.bottom};
	DrawText(hDC, sText.c_str(), sText.length(), &rcClac, DT_CALCRECT);
	LONG nTextHeight = rcClac.bottom - rcClac.top;
	UINT uFormat = DT_VCENTER;
	switch (eSite)
	{
		case SecretMarkSite::LeftTop:
			rcText.bottom = rcText.top + nTextHeight;
			uFormat |= DT_LEFT;
			break;
		case SecretMarkSite::LeftBottom:
			rcText.top = rcText.bottom - nTextHeight;
			uFormat |= DT_LEFT;
			break;
		case SecretMarkSite::RightTop:
			rcText.bottom = rcText.top + nTextHeight;
			uFormat |= DT_RIGHT;
			break;
		case SecretMarkSite::RightBottom:
			rcText.top = rcText.bottom - nTextHeight;
			uFormat |= DT_RIGHT;
			break;
		default:
			break;
	}

	DrawText(hDC, sText.c_str(), sText.length(), &rcText, uFormat);
	SelectObject(hDC, hOldFont);
	return false;
}

bool PrinterOp::DrawWaterMark(HDC hDC, RECT rcRect, const std::wstring& sText)
{
	if (sText.empty())
		return false;

	Gdiplus::Graphics grap(hDC);
	grap.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
	grap.SetPageUnit(Gdiplus::UnitPixel);

	Gdiplus::PointF pt(0.0, 0.0);
	Gdiplus::FontFamily fontF(L"����");
	Gdiplus::REAL real = 30.0;
	Gdiplus::SolidBrush soBrush(Gdiplus::Color(128/*��͸��*/, 192, 192, 192));
	Gdiplus::Font font(&fontF, real, Gdiplus::FontStyleRegular, Gdiplus::UnitPoint);

	Gdiplus::REAL aPixelsPeerDeviceUnit = grap.GetDpiX() / 72.0f;
	Gdiplus::REAL aTextDeviceUnitW = sText.length() * real;
	DOUBLE aWaterMarkW = (aTextDeviceUnitW * aPixelsPeerDeviceUnit) * 0.866;		// 60�� 

	Gdiplus::REAL aRectW = (Gdiplus::REAL)(rcRect.right - rcRect.left);
	Gdiplus::REAL aRectH = (Gdiplus::REAL)(rcRect.bottom - rcRect.top);
	Gdiplus::REAL aRectL = rcRect.left + (Gdiplus::REAL)((aRectW - aWaterMarkW) / 2.0f);

	// ��ָ���ǶȶԻ��������ת
	grap.TranslateTransform(aRectL, aRectH * 2 / 5.0f);
	grap.RotateTransform(330);
	grap.DrawString(sText.c_str(), -1, &font, pt, &soBrush);
	grap.ResetTransform();

	// ��ָ���ǶȶԻ��������ת
	pt.X = pt.Y = 0;
	grap.TranslateTransform(aRectL, aRectH * 4 / 5.0f);
	grap.RotateTransform(330);
	grap.DrawString(sText.c_str(), -1, &font, pt, &soBrush);
	grap.ResetTransform();

	return true;
}

bool PrinterOp::DrawQrCodeInfo(HDC hDC, RECT rcRect, QrCodeSite eSite, const std::wstring& sQrcode, float fScale, const std::wstring& sText, HFONT hFont)
{
	if (nullptr == m_fnMakePDF417) return false;

	LONG lWidth = rcRect.right - rcRect.left;
	LONG lCenter = rcRect.left + lWidth / 2;

	LONG lTextHeight = 0;
	// ���� lpText �� ��С 
	if (!sText.empty())
	{
		HFONT hOld = (HFONT)SelectObject(hDC, hFont);
		RECT rcText = rcRect;
		RECT rcCalc = {0, 0, rcRect.right, rcRect.bottom};
		DrawText(hDC, sText.c_str(), sText.length(), &rcCalc, DT_CALCRECT);
		lTextHeight = rcCalc.bottom - rcCalc.top;
		UINT uFormat = DT_VCENTER;
		switch (eSite)
		{
			case QrCodeSite::LeftTop:
			default:
				rcText.bottom = rcText.top + lTextHeight;
				uFormat |= DT_LEFT;
				break;
			case QrCodeSite::CenterTop:
				rcText.bottom = rcText.top + lTextHeight;
				uFormat |= DT_CENTER;
				break;
			case QrCodeSite::RightTop:
				rcText.bottom = rcText.top + lTextHeight;
				uFormat |= DT_RIGHT;
				break;
			case QrCodeSite::LeftBottom:
				rcText.top = rcText.bottom - lTextHeight;
				uFormat |= DT_LEFT;
				break;
			case QrCodeSite::CenterBottom:
				rcText.top = rcText.bottom - lTextHeight;
				uFormat |= DT_CENTER;
				break;
			case QrCodeSite::RightBottom:
				rcText.top = rcText.bottom - lTextHeight;
				uFormat |= DT_RIGHT;
				break;
		}
		DrawTextW(hDC, sText.c_str(), sText.length(), &rcText, uFormat);
		SelectObject(hDC, hOld);
	}

	// ��ʼ ���� ��ά�� 
	if (!sQrcode.empty())
	{
		auto sQrcodeA = CodingConv::UToUtf8(sQrcode.c_str());
		DWORD uBarCode = 0;
		HBITMAP hBitmap = m_fnMakePDF417((LPSTR)sQrcodeA.c_str(), sQrcodeA.length(), 13, 0, 2, 0, 13, 13, &uBarCode);
		if (NULL == hBitmap) return false;

		BITMAP bitmap;
		if (!::GetObject(hBitmap, sizeof(BITMAP), &bitmap))
		{
			return false;
		}

		HDC hMemDC = ::CreateCompatibleDC(hDC);
		if (NULL == hMemDC)
			return false;
		HGDIOBJ hOldG = ::SelectObject(hMemDC, hBitmap);
		int nQrcodeWidth = (int)bitmap.bmWidth * fScale;		// ���� ��
		int nQrcodeHeight = (int)bitmap.bmHeight * fScale;
		int nDestX = 0;
		int nDestY = 0;
		switch (eSite)
		{
			case QrCodeSite::LeftTop:
			default:
				nDestX = rcRect.left;
				nDestY = rcRect.top + lTextHeight;
				break;
			case QrCodeSite::CenterTop:
				nDestX = lCenter - nQrcodeWidth / 2;
				nDestY = rcRect.top + lTextHeight;
				break;
			case QrCodeSite::RightTop:
				nDestX = rcRect.right - nQrcodeWidth;
				nDestY = rcRect.top + lTextHeight;
				break;
			case QrCodeSite::LeftBottom:
				nDestX = rcRect.left;
				nDestY = rcRect.bottom - lTextHeight - nQrcodeHeight;
				break;
			case QrCodeSite::CenterBottom:
				nDestX = lCenter - nQrcodeWidth / 2;
				nDestY = rcRect.bottom - lTextHeight - nQrcodeHeight;
				break;
			case QrCodeSite::RightBottom:
				nDestX = rcRect.right - nQrcodeWidth;
				nDestY = rcRect.bottom - lTextHeight - nQrcodeHeight;
				break;
		}

		::StretchBlt(hDC, nDestX, nDestY, nQrcodeWidth, nQrcodeHeight, hMemDC, 0, 0, bitmap.bmWidth, bitmap.bmHeight, SRCCOPY);

		::SelectObject(hMemDC, hOldG);
		::DeleteDC(hMemDC);
		::DeleteObject(hBitmap);
	}
	
	return true;
}

int PrinterOp::GetJobInfo(int nJobId, std::wstring& sDocName, DWORD& dwStatus, std::wstring& sOwner, SYSTEMTIME& time)
{
	HANDLE hPrinter = NULL;
	if (!::OpenPrinterW(m_lpPrinterName, &hPrinter, NULL))
	{
		DWORD dwErr = GetLastError();
		if (ERROR_INVALID_PRINTER_NAME == dwErr)
			m_sErrorText = L"��Ч�Ĵ�ӡ������";
		else
			StrFormat::Format(m_sErrorText, _T("OpenPrinterW err %d"), dwErr);
		return -1;
	}
	Defer(if (NULL != hPrinter) { ClosePrinter(hPrinter); hPrinter = NULL; });

	//
	DWORD dwNeed = 0;
	if (!::GetJob(hPrinter, nJobId, 1, nullptr, 0, &dwNeed))
	{
		DWORD dwErr = GetLastError();
		if (ERROR_INVALID_PARAMETER == dwErr)
		{
			// ���� ����Ϊ  �Ҳ����� id
			return -2;
		}
		else if (ERROR_INSUFFICIENT_BUFFER != dwErr)
		{
			StrFormat::Format(m_sErrorText, _T("GetJob err %d"), GetLastError());
			return -1;
		}
	}

	JOB_INFO_1W* jobInfo = (JOB_INFO_1W*)malloc(dwNeed);
	if (nullptr == jobInfo)
	{
		StrFormat::Format(m_sErrorText, _T("malloc err %d"), GetLastError());
		return -1;
	}
	Defer(free(jobInfo););

	if (!::GetJob(hPrinter, nJobId, 1, (LPBYTE)jobInfo, dwNeed, &dwNeed))
	{
		StrFormat::Format(m_sErrorText, _T("GetJob2 err %d"), GetLastError());
		return -1;
	}

	sDocName = jobInfo->pDocument;
	dwStatus = jobInfo->Status;
	sOwner = jobInfo->pUserName;
	time = jobInfo->Submitted;

	return 0;
/*
	DWORD dwJobCount = 0;
	if (!EnumJobsW(hPrinter, 0, 0xFFFF, 1, NULL, 0, &dwNeed, &dwJobCount))
	{
		DWORD dwErr = GetLastError();
		if (ERROR_INSUFFICIENT_BUFFER != dwErr)
		{
			StrFormat::Format(m_sErrorText, _T("EnumJobsW err %d"), GetLastError());
			return -1;
		}
	}

	JOB_INFO_1W* jobList = (JOB_INFO_1W*)malloc(dwNeed);
	if (nullptr == jobList)
	{
		StrFormat::Format(m_sErrorText, _T("malloc err %d"), GetLastError());
		return -1;
	}
	Defer(free(jobList););

	if (!EnumJobsW(hPrinter, 0, 0xFFFF, 1, (LPBYTE)jobList, dwNeed, &dwNeed, &dwJobCount))
	{
		StrFormat::Format(m_sErrorText, _T("EnumJobsW2 err %d"), GetLastError());
		return -1;
	}

	int nRet = -2;// Ĭ�� ȡ����
	for (int i = 0; i < dwJobCount; ++i)
	{
		auto& job = jobList[i];
		if (job.JobId == nJobId)
		{
			sDocName = job.pDocument;
			dwStatus = job.Status;
			sOwner = job.pUserName;
			time = job.Submitted;
			nRet = 0;
			break;
		}
	}
	
	return nRet;*/
}

int PrinterOp::GetJobInfo(int nJobId, DWORD& dwStatus, DWORD& dwPrintedPage, DWORD& dwPrinterStatus)
{
	HANDLE hPrinter = NULL;
	if (!OpenPrinterW(m_lpPrinterName, &hPrinter, NULL))
	{
		DWORD dwErr = GetLastError();
		if (ERROR_INVALID_PRINTER_NAME == dwErr)
			m_sErrorText = L"��Ч�Ĵ�ӡ������";
		else
			StrFormat::Format(m_sErrorText, _T("OpenPrinterW err %d"), dwErr);
		return -1;
	}
	Defer(if (NULL != hPrinter) { ClosePrinter(hPrinter); hPrinter = NULL; });

	//
	DWORD dwNeed = 0;
	if (!::GetJob(hPrinter, nJobId, 1, nullptr, 0, &dwNeed))
	{
		DWORD dwErr = GetLastError();
		if (ERROR_INVALID_PARAMETER == dwErr)
		{
			// ���� ����Ϊ  �Ҳ����� id
			return -2;
		}
		else if (ERROR_INSUFFICIENT_BUFFER != dwErr)
		{
			StrFormat::Format(m_sErrorText, _T("GetJob err %d"), GetLastError());
			return -1;
		}
	}

	JOB_INFO_1W* jobInfo = (JOB_INFO_1W*)malloc(dwNeed);
	if (nullptr == jobInfo)
	{
		StrFormat::Format(m_sErrorText, _T("malloc err %d"), GetLastError());
		return -1;
	}
	Defer(free(jobInfo););

	if (!::GetJob(hPrinter, nJobId, 1, (LPBYTE)jobInfo, dwNeed, &dwNeed))
	{
		StrFormat::Format(m_sErrorText, _T("GetJob2 err %d"), GetLastError());
		return -1;
	}
	dwStatus = jobInfo->Status;
	dwPrintedPage = jobInfo->PagesPrinted;

	// ��ȡ ��ӡ�� ״̬
	dwNeed = 0;			
	if (!GetPrinterW(hPrinter, 2, nullptr, 0, &dwNeed))
	{
		DWORD dwErr = GetLastError();
		if (ERROR_INSUFFICIENT_BUFFER != dwErr)
		{
			StrFormat::Format(m_sErrorText, _T("GetPrinterW_1 err %d"), dwErr);
			return -1;
		}
	}
	PRINTER_INFO_2W* printerInfo = (PRINTER_INFO_2W*)malloc(dwNeed);
	if (nullptr == printerInfo)
	{
		StrFormat::Format(m_sErrorText, _T("malloc err %d"), GetLastError());
		return -1;
	}
	Defer(free(printerInfo); printerInfo = nullptr;);

	DWORD dwUsed = 0;
	if (!GetPrinterW(hPrinter, 2, (LPBYTE)printerInfo, dwNeed, &dwUsed))
	{
		StrFormat::Format(m_sErrorText, _T("GetPrinterW_2 err %d"), GetLastError());
		return -1;
	}

	if (PRINTER_ATTRIBUTE_WORK_OFFLINE & printerInfo->Attributes)
	{
		m_sErrorText = _T("��ӡ���ѻ�");
		return -1;
	}

	dwPrinterStatus = printerInfo->Status;

	return 0;
}

bool PrinterOp::DeleteJob(int nJobId)
{
	HANDLE hPrinter = NULL;
	if (!OpenPrinterW(m_lpPrinterName, &hPrinter, NULL))
	{
		DWORD dwErr = GetLastError();
		if (ERROR_INVALID_PRINTER_NAME == dwErr)
			m_sErrorText = L"��Ч�Ĵ�ӡ������";
		else
			StrFormat::Format(m_sErrorText, _T("OpenPrinterW err %d"), dwErr);
		return -1;
	}
	Defer(if (NULL != hPrinter) { ClosePrinter(hPrinter); hPrinter = NULL; });

	return TRUE == ::SetJobW(hPrinter, nJobId, 0, NULL, JOB_CONTROL_DELETE);
}
