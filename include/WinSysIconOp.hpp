#ifndef __WIN_SYS_ICON__
#define __WIN_SYS_ICON__
#pragma once
#include <shellapi.h>
//#include "stdafx.h"


class WinSysIconOp
{
protected:
	static int m_nFolderIcon ;
public:
	static int GetFolderIcon()
	{
		if(0 == m_nFolderIcon)
		{
			TCHAR cDir[MAX_PATH] = {0};
			UINT nLens = ::GetSystemDirectory(cDir, sizeof(cDir));

			SHFILEINFO fileInfo;
			if (!::SHGetFileInfo(cDir, FILE_ATTRIBUTE_NORMAL, &fileInfo, sizeof(fileInfo), SHGFI_ICON|SHGFI_LARGEICON)) 
			{
				return -1;
			}
			m_nFolderIcon = fileInfo.iIcon;
		}
		
		return m_nFolderIcon;
	}
	static int GetFileIcon(const wchar_t* lpFileName)
	{
		SHFILEINFO fileInfo;
		if (!::SHGetFileInfo(lpFileName, FILE_ATTRIBUTE_NORMAL, &fileInfo, sizeof(fileInfo), SHGFI_SYSICONINDEX | SHGFI_USEFILEATTRIBUTES)) 
		{
			return -1;
		}

		return fileInfo.iIcon;
	}
};

_declspec(selectany) int WinSysIconOp::m_nFolderIcon = 0;
//////////////////////////////////////////////////////////////////////////
#endif // __WIN_SYS_ICON__

