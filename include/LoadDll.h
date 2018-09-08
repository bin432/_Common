#ifndef __LOADDLL_H__
#define __LOADDLL_H__

template<class T>
class DLLFun
{
public:
	DLLFun(LPCTSTR lpDllName, LPCSTR lpFnName)
		: m_hDll(NULL)
		, m_pFn(NULL)
	{
		m_hDll = LoadLibrary(lpDllName);
		if (NULL == m_hDll)
		{
			return ;
		}

		m_pFn = (T)GetProcAddress(m_hDll, lpFnName);
	}
	~DLLFun()
	{
		if(NULL != m_hDll)
		{
			FreeLibrary(m_hDll);
			m_hDll = NULL;
		}
	}
	bool IsValid() const{return NULL != m_pFn;}	
	//T operator->()	const	{return m_pFn;}
	operator T()	const	{return m_pFn;}
private:
	HMODULE m_hDll;
	T m_pFn;
};


#endif//__LOADDLL_H__