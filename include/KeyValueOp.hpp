#ifndef __KEYVALUE_OP_H___
#define __KEYVALUE_OP_H___
#pragma once

#include <string>
#include <map>

template<typename ch>
class KValueT
{
public:
	//template<ch cEqu = '=', ch cSep = ';'>
	template<ch cEqu, ch cSep>
	static bool Parse(const ch* lpStr, int nLen, std::map<std::basic_string<ch>, std::basic_string<ch>>& keyVal)
	{
		if (nLen < 0)
			nLen = lstrlen(lpStr);

		int nKeyBegin = 0;
		int nKeyEnd = 0;
		int nValueBegin = 0;
	
		for (int i=1; i<=nLen; ++i)
		{
			if(cEqu == lpStr[i])			// ÓÐ µÈÓÚ 
			{
				nKeyEnd = i;
				nValueBegin = i+1;
			}
			else if(cSep == lpStr[i] || '\0' == lpStr[i])
			{
				if (nKeyBegin == nKeyEnd)
				{
					continue;
				}
				std::basic_string<ch> sKey(lpStr+nKeyBegin, nKeyEnd - nKeyBegin);
				std::basic_string<ch> sValue(lpStr+nValueBegin, i - nValueBegin);

				keyVal[sKey] = sValue;

				nKeyBegin = i + 1;
				nKeyEnd = nKeyBegin;
				nValueBegin = nKeyBegin;
			}
		}

		return true;
	}
};

typedef KValueT<char>				KValueA;
typedef KValueT<wchar_t>			KValueW;

#ifdef _UNICODE
	typedef KValueW                        kValue;
#else
	typedef KValueA                        kValue;
#endif



#endif		// __KEYVALUE_OP_H___