#ifndef __FILE_FOLDER_HELPER_HPP_
#define __FILE_FOLDER_HELPER_HPP_
#pragma once
#include <Shlwapi.h>
#include <ShlObj.h>
#include <string>
#include <list>

class FileFolderHelper
{
public:
	//新建目录    
	static bool CreateDirEx(const wchar_t* lpDir)
	{
		if (!PathFileExists(lpDir))
		{
			return 0 == ::SHCreateDirectory(NULL, lpDir);
		}
		return true;
	}
	// 创建 该 路径 的 父级 文件夹
	static bool CreateFileParentDir(const wchar_t* lpDirPath)
	{
		wchar_t lpDir[MAX_PATH] = {0};
		lstrcpyn(lpDir, lpDirPath, MAX_PATH);
		PathRemoveFileSpec(lpDir);
		PathUnquoteSpaces(lpDir);	//去除路径中的首尾空格
		if (!PathFileExists(lpDir))
		{
			return 0 == ::SHCreateDirectory(NULL, lpDir);
		}

		return true;
	}
	static bool MoveFile(const wchar_t* lpSrcFile, const wchar_t* lpDestFile)
	{
		/*int nS = wcslen(lpSrcFile);
		auto pF = new wchar_t[nS + 2];
		wcscpy(pF, lpSrcFile);
		pF[nS] = '\0';
		pF[nS+1] = '\0';

		int nD = wcslen(lpDestFile);
		auto pD = new wchar_t[wcslen(lpDestFile) + 2];
		wcscpy(pD, lpDestFile);
		pD[nS] = '\0';
		pD[nS + 1] = '\0';
*/
		SHFILEOPSTRUCTW info;
		ZeroMemory(&info, sizeof(SHFILEOPSTRUCTW));
		info.fFlags |= FOF_FILESONLY;
		//info.fFlags |= FOF_SILENT;			//不显示进度
		//info.fFlags |= FOF_NOERRORUI;			//不显示错误报告
		//info.fFlags |= FOF_NOCONFIRMATION;		// 不显示确认
		//info.fFlags &= ~FOF_ALLOWUNDO;			//直接删除 不放 回收站
		info.hNameMappings = NULL;
		info.hwnd = NULL;
		info.lpszProgressTitle = NULL;


		info.wFunc = FO_MOVE;
		info.pFrom = lpSrcFile + '\0';
		info.pTo = lpDestFile + '\0';

		

		int nRet = SHFileOperationW(&info);
		return 0 == nRet;
	}
	//删除目录 及 其子目录
	static bool DeleteDir(const wchar_t* lpDirPath)
	{
		if (nullptr == lpDirPath || 0 == lstrlen(lpDirPath))
			return false;

		if (!PathFileExists(lpDirPath))
			return false;

		std::wstring sFolderPath = lpDirPath;
		wchar_t cEnd = *(--sFolderPath.end());
		if ('\\' != cEnd && '/' != cEnd)
		{
			sFolderPath.append(L"\\");
		}

		if (RemoveDirectory(sFolderPath.c_str()))
			return true;

		std::list<std::wstring> listFolder;
		listFolder.push_back(sFolderPath);

		std::list<std::wstring> listDel;
		listDel.push_back(sFolderPath);

		while (listFolder.size() > 0)
		{
			std::wstring sPath = listFolder.front();
			listFolder.pop_front();

			WIN32_FIND_DATA fd;
			std::wstring sFindPath = sPath;

			sFindPath.append(L"*");
			HANDLE hFind = FindFirstFile(sFindPath.c_str(), &fd);
			if (INVALID_HANDLE_VALUE == hFind)
			{
				continue;
			}


			//枚举文件夹内的 文件信息
			do
			{
				if ('.' == fd.cFileName[0] && '\0' == fd.cFileName[1])
					continue;
				else if ('.' == fd.cFileName[0] && '.' == fd.cFileName[1] && '\0' == fd.cFileName[2])
					continue;

				std::wstring sFilePath = sPath;
				sFilePath.append(fd.cFileName);


				if (FILE_ATTRIBUTE_DIRECTORY == (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				{
					if (!RemoveDirectory(sFilePath.c_str()) && 145 == GetLastError())
					{
						//如果是文件夹 就先入列   
						listFolder.push_back(sFilePath + _T("\\"));
						listDel.push_back(sFilePath + _T("\\"));
					}
				}
				else
					DeleteFile(sFilePath.c_str());
			}
			while (FindNextFile(hFind, &fd));		//查找下一个

			FindClose(hFind);
		}

		while (listDel.size() > 0)
		{
			std::wstring sPath = listDel.back();
			listDel.pop_back();

			RemoveDirectory(sPath.c_str());
		}


		return true;
	}
	//判断是否 是文件夹
	static bool IsDir(const wchar_t* lpDirPath)
	{
		DWORD dwAttribute = GetFileAttributes(lpDirPath);
		if ((DWORD)-1 == dwAttribute)
			return false;

		if (FILE_ATTRIBUTE_DIRECTORY == (FILE_ATTRIBUTE_DIRECTORY & dwAttribute))
		{
			return true;
		}
		return false;
	}
	//计算文件夹的大小
	static unsigned __int64 CalcFolderSize(const wchar_t* lpFolderPath)
	{
		unsigned __int64 ulFolderSize = 0;
		std::list<std::wstring> listFolder;
		listFolder.push_back(lpFolderPath);

		while (listFolder.size() > 0)
		{
			//从队列首取出路径
			std::wstring sFolder = listFolder.front();
			listFolder.pop_front();

			std::wstring sFilePath = sFolder + _T("\\*");

			WIN32_FIND_DATA fd;
			HANDLE hFind = FindFirstFile(sFilePath.c_str(), &fd);
			if (INVALID_HANDLE_VALUE == hFind)
			{
				continue;
			}

			//枚举文件夹内的 文件信息
			do
			{
				std::wstring strName = fd.cFileName;
				if (_T(".") == strName || _T("..") == strName)
				{
					continue;
				}

				if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
				{
					//如果是文件夹 就先入列   
					listFolder.push_back(sFolder + _T("\\") + fd.cFileName);
					continue;
				}

				ULARGE_INTEGER liSize;
				liSize.LowPart = fd.nFileSizeLow;
				liSize.HighPart = fd.nFileSizeHigh;
				ulFolderSize += liSize.QuadPart;

			}
			while (FindNextFile(hFind, &fd));		//查找下一个

			FindClose(hFind);
		}
		return ulFolderSize;
	}

	struct WinFolderData
	{
		unsigned __int64				ulFileSize;
		unsigned __int64				ulMaxSize;
		unsigned int					uFileCount;
		unsigned int					uFolderCount;
		
		WinFolderData()
		{
			ulFileSize = 0;
			ulMaxSize = 0;
			uFileCount = 0;
			uFolderCount = 0;
		}
	};
	//计算文件夹 的信息
	static bool CalcFolderInfo(const wchar_t* lpFolderPath, WinFolderData& info)
	{
		std::list<std::wstring> listFolder;
		listFolder.push_back(lpFolderPath);

		while (listFolder.size() > 0)
		{
			//从队列首取出路径
			std::wstring sFolder = listFolder.front();
			listFolder.pop_front();

			std::wstring sFilePath = sFolder + _T("\\*");

			WIN32_FIND_DATA fd;
			HANDLE hFind = FindFirstFile(sFilePath.c_str(), &fd);
			if (INVALID_HANDLE_VALUE == hFind)
			{
				continue;
			}

			//枚举文件夹内的 文件信息
			do
			{
				std::wstring strName = fd.cFileName;
				if (_T(".") == strName || _T("..") == strName)
				{
					continue;
				}

				if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
				{
					//如果是文件夹 就先入列   
					listFolder.push_back(sFolder + _T("\\") + fd.cFileName);
					++info.uFolderCount;
					continue;
				}

				ULARGE_INTEGER liSize;
				liSize.LowPart = fd.nFileSizeLow;
				liSize.HighPart = fd.nFileSizeHigh;
				info.ulFileSize += liSize.QuadPart;
				++info.uFileCount;

				if (info.ulFileSize > info.ulMaxSize)
					info.ulMaxSize = info.ulFileSize;

			}
			while (FindNextFile(hFind, &fd));		//查找下一个

			FindClose(hFind);
		}
		return true;
	}
	static unsigned __int64 CalcFileSize(const wchar_t* lpFilePath)
	{
		ULARGE_INTEGER liSize;
		liSize.QuadPart = 0;

		WIN32_FIND_DATAW Find32;
		HANDLE hFind = FindFirstFileW(lpFilePath, &Find32);
		if (INVALID_HANDLE_VALUE != hFind)
		{
			liSize.LowPart = Find32.nFileSizeLow;
			liSize.HighPart = Find32.nFileSizeHigh;
			FindClose(hFind);
		}
		return liSize.QuadPart;
	}
	
	static std::wstring FileSizeToStr(unsigned __int64 ulFileSize)
	{
		if (ulFileSize < 0)
			return _T("未知");
		else if (0 == ulFileSize)
			return _T("0 字节");

		unsigned int nIndex = 0;

		double dSize = static_cast<double>(ulFileSize);
		while (dSize >= 1024.0 && nIndex<4)
		{
			dSize /= 1024.0;
			nIndex++;
		}

		static const wchar_t* s_sUnit[] = {_T("字节"), _T("KB"), _T("MB"), _T("GB")};

		wchar_t lpBuf[100] = {0};
		if (0 == nIndex)
			swprintf_s(lpBuf, 100, _T("%d %s"), (int)ulFileSize, s_sUnit[0]);
		else
			swprintf_s(lpBuf, 100, _T("%.2f %s"), dSize, s_sUnit[nIndex]);
		return std::wstring(lpBuf);
	}
	static std::wstring FileSizeToBSize(unsigned __int64 ulFileSize)
	{
		wchar_t lpBuf[MAX_PATH] = {0};
		swprintf_s(lpBuf, MAX_PATH, _T("%I64d"), ulFileSize);
		std::wstring szSizeText = lpBuf;
		int nLeng = szSizeText.length();
		for (int i = nLeng - 3; i>0; i -= 3)
		{
			szSizeText.insert(i, _T(","));
		}
		szSizeText.append(_T("字节"));
		return szSizeText;
	}
	
	
	//bool SlipStringByChar(LPCTSTR lpStr, TCHAR Char);
	static std::wstring GetFileName(const wchar_t* lpFilePath)
	{
		LPCTSTR lpFileName = ::PathFindFileName(lpFilePath);
		if (nullptr == lpFileName)
			return _T("");

		std::wstring sFileName(lpFileName);
		return sFileName;
	}
	static std::wstring GetFileExt(const wchar_t* lpFileName)
	{
		LPCTSTR lpExt = ::PathFindExtension(lpFileName);
		if (nullptr == lpExt)
			return _T("");

		std::wstring sExt(lpExt + 1);
		return sExt;
	}
	static std::wstring ToRemoveExt(const wchar_t* lpFilePath)
	{
		LPCTSTR lpExt = PathFindExtension(lpFilePath);
		if (nullptr == lpExt)
			return std::wstring(lpFilePath);

		std::wstring sName(lpFilePath, lpExt - lpFilePath);
		return sName;
	}
	static std::wstring GetFileNameWithoutExt(const wchar_t* lpFilePath)
	{
		LPCTSTR lpFileName = ::PathFindFileName(lpFilePath);
		if (nullptr == lpFileName)
			return lpFilePath;

		LPCTSTR lpExt = PathFindExtension(lpFileName);
		if (nullptr == lpExt)
			return std::wstring(lpFileName);

		std::wstring sName(lpFileName, lpExt - lpFileName);
		return sName;
	}

	static std::wstring GetRenameFile(const wchar_t* lpFilePath)
	{
		if (!PathFileExists(lpFilePath))
		{
			return std::wstring(lpFilePath);
		}
	
		std::wstring sFileName;
		auto lpExt = PathFindExtension(lpFilePath);
		if(nullptr != lpExt)
			sFileName = std::move(std::wstring(lpFilePath, lpExt - lpFilePath));

		if (')' == *sFileName.rbegin())
		{
			int n = sFileName.rfind('(');
			int nnn = isdigit(sFileName.at(n + 1));
			if(nnn > 0)
			{
				sFileName.erase(n);
			}
		}

		for(int i=1; ; ++i)
		{
			std::wstring sNewFile = sFileName;
			sNewFile.append(L"(");
//#if __cplusplus < 201103L
#if _MSC_VER >= 1600
			sNewFile.append(std::to_wstring((long long)i));
#else
			sNewFile.append(std::to_wstring(i));
#endif
			sNewFile.append(L")");
			sNewFile.append(lpExt);
			if (!PathFileExists(sNewFile.c_str()))
				return sNewFile;
		}

		return sFileName;
	}

	// 获取文件的当前目录 路径 就是 去掉文件名 
	static std::wstring GetFileCurDir(const wchar_t* lpFileName)
	{
		std::wstring sFilePath(lpFileName);
		if (sFilePath.at(sFilePath.length() - 1) == '\\')
		{
			sFilePath.pop_back();
		}
		//sFilePath.TrimRight('\\');
		//	sFilePath.
		int nPos = sFilePath.rfind('\\');
		if (-1 != nPos)
		{
			sFilePath = sFilePath.substr(0, nPos);
		}

		return sFilePath;
	}

	//检测文件 有效  就是 不能有 */\||之类的 字符
	static bool IsFileNameValid(const wchar_t* lpFileName)
	{
		if (nullptr != wcschr(lpFileName, '*'))
			return false;

		if (nullptr != wcschr(lpFileName, '/'))
			return false;

		if (nullptr != wcschr(lpFileName, '\\'))
			return false;

		if (nullptr != wcschr(lpFileName, '|'))
			return false;

		if (nullptr != wcschr(lpFileName, ':'))
			return false;

		if (nullptr != wcschr(lpFileName, '"'))
			return false;

		if (nullptr != wcschr(lpFileName, '?'))
			return false;

		if (nullptr != wcschr(lpFileName, '<'))
			return false;

		if (nullptr != wcschr(lpFileName, '>'))
			return false;

		return true;
	}

	//template<class stringT=std::wstring>
	//bool GetChildFilePathList(LPCTSTR lpFolderPath, std::list<stringT>& rootList);
	template<class stringT>
	static bool GetChildFilePathList(LPCTSTR lpFolderPath, std::list<stringT>& rootList, bool bIgnoreSysAndHide=true)
	{
		if (nullptr == lpFolderPath || 0 == lstrlen(lpFolderPath))
			return false;

		WIN32_FIND_DATA fd;
		std::wstring sFolderPath = lpFolderPath;
		wchar_t cEnd = *(--sFolderPath.end());
		if ('\\' != cEnd && '/' != cEnd)
		{
			sFolderPath.append(L"\\");
		}

		std::wstring sFindPath = sFolderPath;

		sFindPath.append(L"*");
		HANDLE hFind = FindFirstFile(sFindPath.c_str(), &fd);
		if (INVALID_HANDLE_VALUE == hFind)
		{
			return false;
		}

		//枚举文件夹内的 文件信息
		do
		{
			if ('.' == fd.cFileName[0] && '\0' == fd.cFileName[1])
				continue;
			else if ('.' == fd.cFileName[0] && '.' == fd.cFileName[1] && '\0' == fd.cFileName[2])
				continue;
			else if(bIgnoreSysAndHide && (FILE_ATTRIBUTE_SYSTEM & fd.dwFileAttributes) && (FILE_ATTRIBUTE_HIDDEN & fd.dwFileAttributes))
				continue;

			stringT sFilePath(sFolderPath.c_str(), sFolderPath.length());
			//sFilePath.append(L"\\");
			sFilePath += (fd.cFileName);
			rootList.push_back(sFilePath);

		} while (FindNextFile(hFind, &fd));		//查找下一个

		FindClose(hFind);
		return true;
	}

	// 返回值 是 LastWriteTime  0 失败
	static DWORD GetFileTime(const wchar_t* lpFilePath, DWORD* pCT=nullptr, DWORD* pLA=nullptr)
	{
		WIN32_FIND_DATAW ff32;

		HANDLE hFind = FindFirstFileW(lpFilePath, &ff32);
		if (hFind != INVALID_HANDLE_VALUE)
		{
			FindClose(hFind);
			DWORD dwLWT = 0;
			FILETIME ftLocal;
			FileTimeToLocalFileTime(&(ff32.ftLastWriteTime), &ftLocal);
			FileTimeToDosDateTime(&ftLocal, ((LPWORD)&dwLWT) + 1, ((LPWORD)&dwLWT) + 0);
			
			if (nullptr != pCT)
			{
				FILETIME ft1;
				FileTimeToLocalFileTime(&(ff32.ftCreationTime), &ft1);
				FileTimeToDosDateTime(&ft1, ((LPWORD)pCT) + 1, ((LPWORD)pCT) + 0);
			}

			if (nullptr != pLA)
			{
				FILETIME ft2;
				FileTimeToLocalFileTime(&(ff32.ftLastAccessTime), &ft2);
				FileTimeToDosDateTime(&ft2, ((LPWORD)pLA) + 1, ((LPWORD)pLA) + 0);
			}
			return dwLWT;
		}
		return 0;
	}
};

//////////////////////////////////////////////////////////////////////////
#endif // __FILE_FOLDER_HELPER_HPP_

