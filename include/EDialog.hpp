#ifndef __E_DIALOG_H___
#define __E_DIALOG_H___
#pragma once
#include <commdlg.h>
#include <list>

class ESaveFileDlg
{
public:
	ESaveFileDlg(LPCTSTR lpFileName, LPCTSTR lpTitle=NULL, HWND hWndParent=NULL)
	{
		memset(&m_ofn, 0, sizeof(m_ofn)); // initialize structure to 0/NULL
		
		if (NULL != lpTitle)
			lstrcpy(m_szFileTitle, lpTitle);
		else
			m_szFileTitle[0] = '\0';

		if (NULL != lpFileName)
			lstrcpy(m_szFilePath, lpFileName);
		
		m_ofn.lStructSize = sizeof(m_ofn);
		m_ofn.hwndOwner = hWndParent;

		m_ofn.lpstrFile = m_szFilePath;
		m_ofn.nMaxFile = _MAX_PATH;
		m_ofn.lpstrDefExt = nullptr;
		m_ofn.lpstrFileTitle = (LPTSTR)m_szFileTitle;
		m_ofn.nMaxFileTitle = _MAX_FNAME;
		m_ofn.lpstrFilter = _T("All Files\0 * .*\0\0");


		m_ofn.Flags = OFN_NOCHANGEDIR | OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	}

	INT_PTR DoModal(HWND hWndParent = ::GetActiveWindow())
	{
		if (m_ofn.hwndOwner == NULL)   // set only if not specified before
			m_ofn.hwndOwner = hWndParent;

		return ::GetSaveFileName(&m_ofn);
	}

	LPCTSTR GetFilePath()
	{
		return m_szFilePath;
	}
protected:
	OPENFILENAME				m_ofn;
	TCHAR							m_szFileTitle[_MAX_FNAME];												//TCHAR							m_szFileName[_MAX_PATH];		// contains full path name after return
	TCHAR							m_szFilePath[_MAX_PATH];
};

class EOpenFileDlg
{
public:
	EOpenFileDlg(bool bMultiSelect=false, HWND hWndParent = NULL)
		: m_nFileNameLeng(_MAX_PATH)
	{
		memset(&m_ofn, 0, sizeof(m_ofn)); // initialize structure to 0/NULL
		m_szFileTitle[0] = '\0';
		
		if (bMultiSelect)
			m_nFileNameLeng = 100 * _MAX_PATH;

		m_lpFileName = new TCHAR[m_nFileNameLeng];
		memset(m_lpFileName, 0, m_nFileNameLeng);

		m_ofn.lStructSize = sizeof(m_ofn);
		m_ofn.hwndOwner = hWndParent;

		m_ofn.lpstrFile = m_lpFileName;
		m_ofn.nMaxFile = m_nFileNameLeng;
		m_ofn.lpstrDefExt = nullptr;
		m_ofn.lpstrFileTitle = (LPTSTR)m_szFileTitle;
		m_ofn.nMaxFileTitle = _MAX_FNAME;
		m_ofn.lpstrFilter = _T("All Files\0 * .*\0\0");
		

		m_ofn.Flags = OFN_NOCHANGEDIR | OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
		if (bMultiSelect)
			m_ofn.Flags |= OFN_ALLOWMULTISELECT;
		

		// setup initial file name
		//if(lpszFileName != NULL)
		//	_tcscpy_s(m_szFileName, _countof(m_szFileName), lpszFileName);
	}
	~EOpenFileDlg()
	{
		if(nullptr != m_lpFileName)
		{
			delete[] m_lpFileName;
			m_lpFileName = nullptr;
			m_nFileNameLeng = 0;
		}
	}
	void SetFlags(DWORD dwFlags)
	{

	}
	void SetTitle(LPCTSTR lpTitle)
	{
		m_ofn.lpstrTitle = lpTitle;
	}
	INT_PTR DoModal(HWND hWndParent = ::GetActiveWindow())
	{
		if(m_ofn.hwndOwner == NULL)   // set only if not specified before
			m_ofn.hwndOwner = hWndParent;
			
		return ::GetOpenFileName(&m_ofn);
	}

	LPCTSTR GetFilePath()
	{
		return m_lpFileName;
	}

	bool GetFileList(std::list<std::wstring>& fileList)
	{
		std::wstring sPath;
		bool bFirst = true;
		int nPos = 1;
		for (int i=1; i<m_nFileNameLeng; ++i)
		{
			if('\0' == m_lpFileName[i])
			{
				if (bFirst)
				{
					sPath = std::wstring(m_lpFileName, i);
					bFirst = false;
				}
				else
				{					
					std::wstring sFile(m_lpFileName + nPos, i - nPos);
					if (sFile.empty())
						break;

					std::wstring sNewFile = sPath;
					sNewFile.append(L"\\");
					sNewFile.append(std::move(sFile));
					fileList.push_back(sNewFile);
				}
				nPos = i + 1;
			}
		}
		
		if (0 == fileList.size() && !sPath.empty())
			fileList.push_back(sPath);

		return true;
	}
public:
	OPENFILENAME				m_ofn;
protected:
	
	TCHAR							m_szFileTitle[_MAX_FNAME];		// contains file title after return
	//TCHAR							m_szFileName[_MAX_PATH];		// contains full path name after return
	TCHAR*							m_lpFileName;
	int									m_nFileNameLeng;
};

class EDirDialog
{
public:
	EDirDialog(LPCTSTR lpTitle=nullptr, LPCTSTR lpDefaultPath=nullptr)
	{
		if (nullptr == lpDefaultPath)
		{
			memset(m_lpDir, 0, MAX_PATH);
		}
		else
			memcpy(m_lpDir, lpDefaultPath, MAX_PATH);
	
		m_BrowserInfo.hwndOwner = NULL;
		m_BrowserInfo.pidlRoot = NULL;
		m_BrowserInfo.pszDisplayName = m_lpDir;
		m_BrowserInfo.lpszTitle = lpTitle;
		m_BrowserInfo.ulFlags = BIF_STATUSTEXT | BIF_USENEWUI | BIF_RETURNONLYFSDIRS;
		m_BrowserInfo.lpfn = NULL;		
		m_BrowserInfo.iImage = 0;

		m_BrowserInfo.lParam = (LPARAM)this;
		m_BrowserInfo.lpfn = _fnCallback;
		//m_lpDir = lpDefaultPath;
	}

	// false  取消 
	bool DoModal(HWND hWndParent = ::GetActiveWindow())
	{
		if (NULL == m_BrowserInfo.hwndOwner)   // set only if not specified before
			m_BrowserInfo.hwndOwner = hWndParent;
		
		ITEMIDLIST* pidl = SHBrowseForFolder(&m_BrowserInfo);
		if (pidl == NULL)//
		{
			return false;	//没有选s
		}
		if (!SHGetPathFromIDList(pidl, m_lpDir))
		{
			return false;
		}

		return true;
	}
	LPCTSTR GetDirPath()
	{
		return m_lpDir;
	}
protected:
	static int __stdcall _fnCallback(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
	{
		EDirDialog* pDlg = (EDirDialog*)lpData;
		switch (uMsg)
		{
		case BFFM_INITIALIZED:  //选择文件夹对话框初始化   
			//设置默认路径为lpData即'D:\'    
			::SendMessage(hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)pDlg->m_lpDir);
			//在STATUSTEXT区域显示当前路径    
			::SendMessage(hwnd, BFFM_SETSTATUSTEXT, 0, (LPARAM)pDlg->m_lpDir);
			//设置选择文件夹对话框的标题    
			::SetWindowText(hwnd, pDlg->m_BrowserInfo.lpszTitle);
			break;
		case BFFM_SELCHANGED:   //选择文件夹变更时   
		{
			TCHAR pszPath[MAX_PATH];
			//获取当前选择路径    
			SHGetPathFromIDList((LPCITEMIDLIST)lParam, pszPath);

			SetDlgItemText(hwnd, 0x3742, pszPath);
		}
		break;
		}
		return 0;
	};
protected:
	BROWSEINFO				m_BrowserInfo;
	

	TCHAR						m_lpDir[MAX_PATH];
};


#endif