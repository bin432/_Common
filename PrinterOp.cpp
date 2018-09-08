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
		sStatusText += L"打印机已暂停,";
	if (PRINTER_STATUS_ERROR & dwStatus)
		sStatusText += L"打印机错误,";
	if (PRINTER_STATUS_PENDING_DELETION & dwStatus)
		sStatusText += L"正在删除,";
	if (PRINTER_STATUS_PAPER_JAM & dwStatus)
		sStatusText += L"打印机卡纸,";
	if (PRINTER_STATUS_PAPER_OUT  & dwStatus)
		sStatusText += L"打印机缺纸,";
	if (PRINTER_STATUS_MANUAL_FEED & dwStatus)
		sStatusText += L"打印机处于手动进纸状态,";
	if (PRINTER_STATUS_PAPER_PROBLEM & dwStatus)
		sStatusText += L"打印机纸张有问题,";
	if (PRINTER_STATUS_OFFLINE & dwStatus)
		sStatusText += L"打印机脱机,";
	if (PRINTER_STATUS_IO_ACTIVE & dwStatus)
		sStatusText += L"打印机处于活动输入输出状态,";
	if (PRINTER_STATUS_BUSY & dwStatus)
		sStatusText += L"打印机忙,";
	if (PRINTER_STATUS_PRINTING & dwStatus)
		sStatusText += L"打印机正在进行打印,";
	if (PRINTER_STATUS_OUTPUT_BIN_FULL & dwStatus)
		sStatusText += L"打印机的输出纸盒已满,";
	if (PRINTER_STATUS_NOT_AVAILABLE & dwStatus)
		sStatusText += L"打印机不可用,";
	if (PRINTER_STATUS_WAITING & dwStatus)
		sStatusText += L"打印机卡纸,";
	if (PRINTER_STATUS_PROCESSING & dwStatus)
		sStatusText += L"打印机正在处理打印作业,";
	if (PRINTER_STATUS_INITIALIZING & dwStatus)
		sStatusText += L"打印机正在初始化,";
	if (PRINTER_STATUS_WARMING_UP & dwStatus)
		sStatusText += L"打印机正在预热,";
	if (PRINTER_STATUS_TONER_LOW & dwStatus)
		sStatusText += L"打印机墨量低,";
	if (PRINTER_STATUS_NO_TONER & dwStatus)
		sStatusText += L"打印机缺墨,";
	if (PRINTER_STATUS_PAGE_PUNT & dwStatus)
		sStatusText += L"打印机无法打印当前页,";
	if (PRINTER_STATUS_USER_INTERVENTION & dwStatus)
		sStatusText += L"打印机要求用户干预,";
	if (PRINTER_STATUS_OUT_OF_MEMORY & dwStatus)
		sStatusText += L"打印机内存溢出,";
	if (PRINTER_STATUS_DOOR_OPEN & dwStatus)
		sStatusText += L"打印机仓门被打开,";
	if (PRINTER_STATUS_SERVER_UNKNOWN & dwStatus)
		sStatusText += L"打印服务未知错误,";
	if (PRINTER_STATUS_POWER_SAVE & dwStatus)
		sStatusText += L"打印机处于电源保存模式,";
	if (PRINTER_STATUS_SERVER_OFFLINE & dwStatus)
		sStatusText += L"打印服务离线,";
	if (PRINTER_STATUS_DRIVER_UPDATE_NEEDED & dwStatus)
		sStatusText += L"打印机需要更新驱动,";

	if (sStatusText.empty())
	{
		sStatusText = L"打印机异常";
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
		sStatusText += L"已暂停-";
	if (JOB_STATUS_ERROR & dwStatus)
		sStatusText += L"错误-";
	if (JOB_STATUS_DELETING & dwStatus)
		sStatusText += L"正在删除-";
	if (JOB_STATUS_SPOOLING & dwStatus)
		sStatusText += L"脱机-";
	if (JOB_STATUS_PRINTING  & dwStatus)
		sStatusText += L"正在打印-";
	if (JOB_STATUS_OFFLINE & dwStatus)
		sStatusText += L"离线-";
	if (JOB_STATUS_PAPEROUT & dwStatus)
		sStatusText += L"没纸张了-";
	if (JOB_STATUS_PRINTED & dwStatus)
		sStatusText += L"已打印-";
	if (JOB_STATUS_DELETED & dwStatus)
		sStatusText += L"已删除-";
	if (JOB_STATUS_BLOCKED_DEVQ & dwStatus)
		sStatusText += L"驱动故障-";
	if (JOB_STATUS_USER_INTERVENTION & dwStatus)
		sStatusText += L"需要人为操作-";
	if (JOB_STATUS_RESTART & dwStatus)
		sStatusText += L"重新打印-";
	
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
			m_sErrorText = L"无效的打印机名称";
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
		m_sErrorText = _T("打印机脱机");
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

	
	// 枚举 任务 
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
			m_sErrorText = L"无效的打印机名称";
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
		m_sErrorText = _T("打印机脱机");
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
	// 准备工作
	JobIniFileOp iniFile(lpJobFile);
	int nPaperSize = iniFile.GetPaperSize();
	int nOrientation = iniFile.GetOrientation();
	//int nCollate = iniFile.GetCollate();
	int nCollate = 1;			// 强制 设置成 逐份打印
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
			m_sErrorText = L"无效的打印机名称";
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
		// 创建 打印机 dc 
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
	docInfoW.lpszDocName = sDocName.c_str();			// 这里 要保证 字符串指针在 范围内 有效
	if (!::SetMapMode(hPrinterDc, MM_TEXT))
	{
		OutputDebugString(_T("PrinterOp::Print - SetMapMode err"));
	}
		
	int nJobId = ::StartDocW(hPrinterDc, &docInfoW);
	if (nJobId <= 0)
	{
		DWORD dwErr = GetLastError();
		if (ERROR_CANCELLED == dwErr)
			m_sErrorText = L"用户取消了打印。";
		else
			StrFormat::Format(m_sErrorText, _T("StartDocW err %d"), GetLastError());
		return nJobId;
	}

	// 计算 字体 
	HFONT hFont = CreateFontW(100, 0, 0, 0, 0,
		FALSE, FALSE, 0, ANSI_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"宋体");

	LONG nQrFontH = abs(MulDiv(param.qrParam.nTextFontSize, GetDeviceCaps(hPrinterDc, LOGPIXELSY), 72));
	if (0 == nQrFontH) nQrFontH = 72;
	HFONT hQrFont = CreateFontW(nQrFontH, 0, 0, 0, 0,
		FALSE, FALSE, 0, ANSI_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"宋体");

	
	// 物理纸张的高度与宽度(单位：设备单位)
	const int nPaperW = ::GetDeviceCaps(hPrinterDc, PHYSICALWIDTH);
	const int nPaperH = ::GetDeviceCaps(hPrinterDc, PHYSICALHEIGHT);

	// 物理纸张的边距(单位：设备单位，与 rclBounds 单位相同)
	const int nPaperMarginX = ::GetDeviceCaps(hPrinterDc, PHYSICALOFFSETX);
	const int nPaperMarginY = ::GetDeviceCaps(hPrinterDc, PHYSICALOFFSETY);

	// 可打印区域的高度与宽度(单位：像素)
	const int nPixelsW = ::GetDeviceCaps(hPrinterDc, HORZRES);
	const int nPixelsH = ::GetDeviceCaps(hPrinterDc, VERTRES);

	// emf 文件打印 要 扩大 边距 
	RECT rcEmfRect = {-nPaperMarginX, -nPaperMarginY, nPixelsW + nPaperMarginX, nPixelsH + nPaperMarginY};
	// 自定义的 直接在
	RECT rcOtherRect = {0, 0, nPixelsW, nPixelsH};


#if 0
	// 每个设备单位的像素个数(可打印区尺寸域等于物理尺寸减去两个边距)
	const double aDuppX = nPixelsW / ((double)nPaperW - nPaperMarginX * 2.0);
	const double aDuppY = nPixelsH / ((double)nPaperH - nPaperMarginY * 2.0);

	// 每英寸对应的像素个数
	const int nPixelsPeerInchX = ::GetDeviceCaps(hPrinterDc, LOGPIXELSX);
	const int nPixelsPeerInchY = ::GetDeviceCaps(hPrinterDc, LOGPIXELSY);

	// 计算边距的像素(物理纸张不可打印的边距，取四舍五入)
	const int aPixelsEdgeW = (int)(aDuppX * nPaperMarginX + 0.5);
	const int aPixelsEdgeH = (int)(aDuppY * nPaperMarginY + 0.5);

	// 单位转换计算(0.01毫米 -> 英寸的转换常量)
	const double aMM001_2Inch = 0.0393700787 / 100.0;

	// 单位转换计算(0.01毫米 -> 设备单位的转换)
	const double dbMM001_2DuX = (aMM001_2Inch * nPixelsPeerInchX) / aDuppX;
	const double dbMM001_2DuY = (aMM001_2Inch * nPixelsPeerInchY) / aDuppY;
#endif

	int nPages = vctPages.size();
	int nCopies = iniFile.GetCopies();
	// 是 多份 双面打印 且 是 奇数 页数 就要 打印一张空白页
	const bool bPrintBlankPage = (iniFile.GetDuplex() != DMDUP_SIMPLEX) && (1 == (nPages % 2)) && (nCopies > 1);
	
	auto& qr = param.qrParam; // 二维码参数 引用 
	
	std::wstring sHeadQrcode = qr.sQrcode;
	sHeadQrcode  += nCollate ? L"/Z" : L"/S";
	std::wstring sHeadQrText = qr.sText;
	sHeadQrText += nCollate ? L"/Z" : L"/S";

	

	// 绘制 page 
	auto FnDrawItem = [&](int nPage, int nCopie, LPCTSTR lpEMF) -> bool
	{
		int nRet = ::StartPage(hPrinterDc);
		if (nRet <= 0)
		{
			return false;
		}

		// 绘制 EMF 原始 文件内容
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

		// 调整 区域 开始 绘制 自定义内容
		//OffsetRect(&rcRect, nPaperMarginX, nPaperMarginY);
		//rcRect.left += 2 * nPaperMarginX;
		//rcRect.top += 2 * nPaperMarginY;
		//rcRect.right -= nPaperMarginX;
		//rcRect.bottom -= nPaperMarginY;

		// 绘制 密级标识
		if ((SecretMarkShow::AllPage == param.smParam.eShow) ||
			((SecretMarkShow::HomePage == param.smParam.eShow) && (1 == nPage))
			)
		{
			DrawSecretMark(hPrinterDc, rcOtherRect, param.smParam.eSite, param.smParam.sText, hFont);
		}

		// 绘制 水印
		if (!param.wmParam.sText.empty())
		{
			DrawWaterMark(hPrinterDc, rcOtherRect, param.wmParam.sText);
		}

		// 绘制二维码及二维码文本
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

		std::wstring sText;			// 二维码 文本
		if (QrCodeShow::AllPage == qr.eTextShow)
		{
			StrFormat::Format(sText, L"%s/份数:%d/页数:%d", sHeadQrText.c_str(), nCopie, nPage);
		}
		else if ((QrCodeShow::HomePage == qr.eTextShow) && (1 == nPage))
		{
			StrFormat::Format(sText, L"%s/份数:%d/T/总页数:%d", sHeadQrText.c_str(), nCopie, nPages);
		}
		else if ((QrCodeShow::EndPage == qr.eTextShow) && (nPages == nPage))
		{
			StrFormat::Format(sText, L"%s/份数:%d/T/总页数:%d", sHeadQrText.c_str(), nCopie, nPages);
		}
		else if ((QrCodeShow::OddPage == qr.eTextShow) && (1 == (nPage % 2)))
		{
			StrFormat::Format(sText, L"%s/份数:%d/O/页数:%d", sHeadQrText.c_str(), nCopie, nPage);
		}
		else if ((QrCodeShow::EvenPage == qr.eTextShow) && (0 == (nPage % 2)))
		{
			StrFormat::Format(sText, L"%s/份数:%d/E/页数:%d", sHeadQrText.c_str(), nCopie, nPage);
		}

		// 设置 边距
		rcOtherRect.left += qr.rcMargin.left;
		rcOtherRect.top += qr.rcMargin.top;
		rcOtherRect.right -= qr.rcMargin.right;
		rcOtherRect.bottom -= qr.rcMargin.bottom;

		DrawQrCodeInfo(hPrinterDc, rcOtherRect, param.qrParam.eSite, sQrcode, qr.fScale, sText, hQrFont);

		::EndPage(hPrinterDc);
		::DeleteEnhMetaFile(hEmf);
		return true;
	};
		
	// 这里默认 逐份打印
	for (int nCopie=1; nCopie<=nCopies; ++nCopie)
	{
		for(int nPage=1; nPage<=nPages; ++nPage)
		{
			if (nullptr != fun && !fun(nPage, nCopie))
				goto end;			// 外面取消 打印了 所以直接退出

			auto paInfo = vctPages.at(nPage-1);
			if (paInfo->nOrientation != pDevMode->dmOrientation)	// 文档包含横向与纵向时：根据配置实时切换打印方向
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

		if (bPrintBlankPage)			// 打印空白页
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
	Gdiplus::FontFamily fontF(L"宋体");
	Gdiplus::REAL real = 30.0;
	Gdiplus::SolidBrush soBrush(Gdiplus::Color(128/*半透明*/, 192, 192, 192));
	Gdiplus::Font font(&fontF, real, Gdiplus::FontStyleRegular, Gdiplus::UnitPoint);

	Gdiplus::REAL aPixelsPeerDeviceUnit = grap.GetDpiX() / 72.0f;
	Gdiplus::REAL aTextDeviceUnitW = sText.length() * real;
	DOUBLE aWaterMarkW = (aTextDeviceUnitW * aPixelsPeerDeviceUnit) * 0.866;		// 60度 

	Gdiplus::REAL aRectW = (Gdiplus::REAL)(rcRect.right - rcRect.left);
	Gdiplus::REAL aRectH = (Gdiplus::REAL)(rcRect.bottom - rcRect.top);
	Gdiplus::REAL aRectL = rcRect.left + (Gdiplus::REAL)((aRectW - aWaterMarkW) / 2.0f);

	// 以指定角度对画板进行旋转
	grap.TranslateTransform(aRectL, aRectH * 2 / 5.0f);
	grap.RotateTransform(330);
	grap.DrawString(sText.c_str(), -1, &font, pt, &soBrush);
	grap.ResetTransform();

	// 以指定角度对画板进行旋转
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
	// 计算 lpText 的 大小 
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

	// 开始 绘制 二维码 
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
		int nQrcodeWidth = (int)bitmap.bmWidth * fScale;		// 缩放 比
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
			m_sErrorText = L"无效的打印机名称";
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
			// 这里 设置为  找不到改 id
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

	int nRet = -2;// 默认 取不到
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
			m_sErrorText = L"无效的打印机名称";
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
			// 这里 设置为  找不到改 id
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

	// 获取 打印机 状态
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
		m_sErrorText = _T("打印机脱机");
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
			m_sErrorText = L"无效的打印机名称";
		else
			StrFormat::Format(m_sErrorText, _T("OpenPrinterW err %d"), dwErr);
		return -1;
	}
	Defer(if (NULL != hPrinter) { ClosePrinter(hPrinter); hPrinter = NULL; });

	return TRUE == ::SetJobW(hPrinter, nJobId, 0, NULL, JOB_CONTROL_DELETE);
}
