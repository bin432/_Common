#ifndef __WIN_HANDLE_H_
#define __WIN_HANDLE_H_

#pragma once

#include <WinBase.h>
enum WinVerDef
{
	WIN_VER_UNKNOWN = -1,
	WIN_VER_95 = 0,
	WIN_VER_98,
	WIN_VER_ME,
	WIN_VER_NT4,
	WIN_VER_2K,
	WIN_VER_XP,
	WIN_VER_2K3,
	WIN_VER_2K3R2,
	WIN_VER_VISTA,
	WIN_VER_2K8,
	WIN_VER_2K8R2,
	WIN_VER_WIN7,
	WIN_WIN_WIN8,
	WIN_VER_WIN10,
};

class WindowsOp
{
public:
	static WinVerDef GetWindowsVersion()
	{
		OSVERSIONINFOEX os;   
		os.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);     
		WinVerDef nWinVer = WIN_VER_UNKNOWN;//0 unknow
		if(GetVersionEx((OSVERSIONINFO *)&os))  
		{   
			//下面根据版本信息判断操作系统名称   
			switch(os.dwMajorVersion)//判断主版本号  
			{  
			case 4:  
				switch(os.dwMinorVersion)//判断次版本号   
				{   
				case 0:  
					if(os.dwPlatformId == VER_PLATFORM_WIN32_NT)  
					{
						nWinVer =  WIN_VER_NT4;
					}
					else if(os.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)  
					{
						nWinVer = WIN_VER_95;
					}
					break;  
				case 10:  
					{
						nWinVer = WIN_VER_98;
					}

					break;  
				case 90:  
					{
						nWinVer = WIN_VER_ME;
					}
					break;  
				}  
				break;  

			case 5:  
				switch(os.dwMinorVersion)   //再比较dwMinorVersion的值  
				{   
				case 0:  
					nWinVer = WIN_VER_2K;
					break;  
				case 1:  
					nWinVer = WIN_VER_XP;
					break;  
				case 2:  
					if(GetSystemMetrics(SM_SERVERR2)==0)  
					{
						nWinVer = WIN_VER_2K3;
						//osname =_T("Microsoft Windows Server 2003");//2003年3月发布   
					}
					else if(GetSystemMetrics(SM_SERVERR2)!=0)  
					{
						nWinVer = WIN_VER_2K3R2;
						//osname =_T("Microsoft Windows Server 2003 R2");  
					}
					break;  
				}  
				break;  

			case 6:  
				switch(os.dwMinorVersion)  
				{  
				case 0:  
					if(os.wProductType == VER_NT_WORKSTATION)  
					{
						//osname =_T("Microsoft Windows Vista");  
						nWinVer = WIN_VER_VISTA;
					}
					else  
					{
						nWinVer = WIN_VER_2K8;
						//osname =_T("Microsoft Windows Server 2008");//服务器版本   
					}
					break;  
				case 1:  
					if(os.wProductType == VER_NT_WORKSTATION)  
					{
						nWinVer = WIN_VER_WIN7;
						//osname =_T("Microsoft Windows 7");  
					}
					else  
					{
						nWinVer = WIN_VER_2K8R2;
						//osname =_T("Microsoft Windows Server 2008 R2");  
					}
					break;  
				case 2:
					if(os.wProductType == VER_NT_WORKSTATION)  
					{
						nWinVer = WIN_VER_WIN10;
						//osname =_T("Microsoft Windows 7");  
					}
					else  
					{
						//nWinVer = WIN_VER_2K8R2;
						//osname =_T("Microsoft Windows Server 2008 R2");  
					}
				}  
				break;  
			}  
		}

		return nWinVer;
	}


	static bool Is64Sys()
	{
		bool b64System = false;
		SYSTEM_INFO si;
		typedef VOID (WINAPI *LPFN_GetNativeSystemInfo)(LPSYSTEM_INFO lpSystemInfo);   
		LPFN_GetNativeSystemInfo nsInfo =   
			(LPFN_GetNativeSystemInfo)GetProcAddress(GetModuleHandle(_T("kernel32")), "GetNativeSystemInfo");;    
		if (NULL != nsInfo)    
		{    
			nsInfo(&si);    
		}    
		else    
		{
			GetSystemInfo(&si); 
		}

		if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ||    
			si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64 )   
		{
			b64System = true;
		}

		return b64System;
	}
};

#endif	//__WIN_HANDLE_H_