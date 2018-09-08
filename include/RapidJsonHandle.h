#ifndef __RAPID_JSON_H__
#define __RAPID_JSON_H__


#pragma once

#include "rapidjson/document.h"  
#include "rapidjson/writer.h"
#include "rapidjson/prettywriter.h"  
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filereadstream.h"

using namespace rapidjson;  


typedef GenericValue<UTF16<>>										ValueW;

typedef ValueW::MemberIterator										ValueIter;		
typedef ValueW::ConstMemberIterator								CValueIter;	
typedef GenericDocument<UTF16<>>::AllocatorType			AllocatorW;

//typedef GenericDocument<UTF16<>>								DocumentW;
//typedef GenericDocument<UTF16<>>::AllocatorType		RapidAllocatorType;
//typedef GenericStringStream<UTF16<> >						StringStreamW;
//typedef GenericStringBuffer<UTF16<>>							StringBufferW;
//typedef Writer<StringBufferW, UTF16<>, UTF16<>>		WriterW;
/****************************************************

*****************************************************/


#if 1
/*
这个封装 简化了 原来的代码量。适用于 只有简单类型的键值对 。
对于要添加数组和对象的 还是用原来的方法添加。AddMember
*/
template <typename Encoding>
class GenericDocumentHandle : public GenericDocument<Encoding>
{
public:
	typedef typename Encoding::Ch Ch;
	GenericDocumentHandle()
	{
		SetObject();
	}
	~GenericDocumentHandle()
	{

	}

	GenericValue<Encoding>& Add(const Ch* lpKey, const Ch* lpValue)
	{
		auto& allocator = GetAllocator();
		GenericValue<Encoding> key(lpKey, allocator);
		GenericValue<Encoding> value(lpValue, allocator);
		return GenericDocument<Encoding>::AddMember(key, value, allocator);
	}

	GenericValue<Encoding>& Add(const Ch* lpKey, bool bValue)
	{
		auto& allocator = GetAllocator();
		GenericValue<Encoding> key(lpKey, allocator);
		return GenericDocument<Encoding>::AddMember(key, bValue, allocator);
	}

	GenericValue<Encoding>& Add(const Ch* lpKey, double dbValue)
	{
		auto& allocator = GetAllocator();
		GenericValue<Encoding> key(lpKey, allocator);
		return GenericDocument<Encoding>::AddMember(key, dbValue, allocator);
	}

	GenericValue<Encoding>& Add(const Ch* lpKey, int nValue)
	{
		auto& allocator = GetAllocator();
		GenericValue<Encoding> key(lpKey, allocator);
		return GenericDocument<Encoding>::AddMember(key, nValue, allocator);
	}
	
	GenericValue<Encoding>& Add(const Ch* lpKey, unsigned int uValue)
	{
		auto& allocator = GetAllocator();
		GenericValue<Encoding> key(lpKey, allocator);
		return GenericDocument<Encoding>::AddMember(key, uValue, allocator);
	}
	GenericValue<Encoding>& Add(const Ch* lpKey, unsigned long ulValue)
	{
		auto& allocator = GetAllocator();
		GenericValue<Encoding> key(lpKey, allocator);
		return GenericDocument<Encoding>::AddMember(key, static_cast<unsigned int>(ulValue), allocator);
	}
	GenericValue<Encoding>& Add(const Ch* lpKey, int64_t nValue)
	{
		auto& allocator = GetAllocator();
		GenericValue<Encoding> key(lpKey, allocator);
		return GenericDocument<Encoding>::AddMember(key, nValue, allocator);
	}

	GenericValue<Encoding>& Add(const Ch* lpKey, uint64_t ulValue)
	{
		auto& allocator = GetAllocator();
		GenericValue<Encoding> key(lpKey, allocator);
		return GenericDocument<Encoding>::AddMember(key, ulValue, allocator);
	}
	
	GenericValue<Encoding>& Add(const Ch* lpKey, GenericValue<Encoding>& jValue)
	{
		auto& allocator = GetAllocator();
		GenericValue<Encoding> key(lpKey, allocator);
		return GenericDocument<Encoding>::AddMember(key, jValue, allocator);
	}

	template <typename EnumType>
	GenericValue<Encoding>& AddEnum(const Ch* lpKey, EnumType eEnum)
	{
		return Add(lpKey, (int)eEnum);
	}

	GenericValue<Encoding>& PushBack2(const Ch* lpValue)
	{
		auto& allocator = GetAllocator();
		GenericValue<Encoding> value(lpValue, allocator);
		return GenericDocument<Encoding>::PushBack(value, allocator);
	}

	std::basic_string<Ch> toStyledString()
	{
		GenericStringBuffer<Encoding> s;
		Writer<GenericStringBuffer<Encoding>, Encoding, Encoding> writer(s);
		Accept(writer);
		return s.GetString();
	}
};

template <typename Encoding>
class RapidFunT
{
public:
	typedef typename Encoding::Ch Ch;
	static std::basic_string<Ch> ToString(const GenericValue<Encoding>& jValue)
	{
		GenericStringBuffer<Encoding> s;
		Writer<GenericStringBuffer<Encoding>, Encoding, Encoding> writer(s);
		jValue.Accept(writer);
		return s.GetString();
	}
	
	static bool GetBool(const GenericValue<Encoding>& jValue, const Ch* lpKey, bool nDefault = false)
	{
		auto ite = jValue.FindMember(lpKey);
		if (ite == jValue.MemberEnd())
			return nDefault;

		return ite->value.GetBool();
	}

	static int GetInt(const GenericValue<Encoding>& jValue, const Ch* lpKey, int nDefault=0)
	{
		auto ite = jValue.FindMember(lpKey);
		if (ite == jValue.MemberEnd())
			return nDefault;

		return ite->value.GetInt();
	}

	static UINT GetUint(const GenericValue<Encoding>& jValue, const Ch* lpKey, UINT nDefault = 0)
	{
		auto ite = jValue.FindMember(lpKey);
		if (ite == jValue.MemberEnd())
			return nDefault;

		return ite->value.GetUint();
	}

	static INT64 GetInt64(const GenericValue<Encoding>& jValue, const Ch* lpKey, INT64 nDefault = 0)
	{
		auto ite = jValue.FindMember(lpKey);
		if (ite == jValue.MemberEnd())
			return nDefault;

		return ite->value.GetInt64();
	}

	static UINT64 GetUint64(const GenericValue<Encoding>& jValue, const Ch* lpKey, UINT64 nDefault = 0)
	{
		auto ite = jValue.FindMember(lpKey);
		if (ite == jValue.MemberEnd())
			return nDefault;

		return ite->value.GetUint64();
	}
	
	static const Ch* GetString(const GenericValue<Encoding>& jValue, const Ch* lpKey, const Ch* lpDefault=nullptr)
	{
		auto ite = jValue.FindMember(lpKey);
		if (ite == jValue.MemberEnd())
			return lpDefault;

		return ite->value.GetString();
	}

	static float GetFloat(const GenericValue<Encoding>& jValue, const Ch* lpKey, float fDefault=0.0)
	{
		auto ite = jValue.FindMember(lpKey);
		if (ite == jValue.MemberEnd())
			return fDefault;

		return ite->value.GetFloat();
	}

	template <typename EnumType>
	static EnumType GetEnum(const GenericValue<Encoding>& jValue, const Ch* lpKey, EnumType eDefault=0)
	{
		auto ite = jValue.FindMember(lpKey);
		if (ite == jValue.MemberEnd())
			return eDefault;

		return (EnumType)ite->value.GetInt();
	}
};

typedef RapidFunT<UTF16<>> RapidFunW;
typedef RapidFunT<UTF8<>> RapidFunA;
#ifdef UNICODE
#define RapidFun RapidFunW
#else
#define RapidFun RapidFunA
#endif // UNICODE

// 这个用于 Unicode 字符集的工程
typedef GenericDocumentHandle<UTF16<>> RapidJsonW;
// 这个用于 多字节 
typedef GenericDocumentHandle<UTF8<>> RapidJson;


#endif





/*

DocumentW document1;
document1.SetObject();
DocumentW::AllocatorType& allocator = document1.GetAllocator();

ValueW common_info;
common_info.SetArray();

//for(int i=0; i<doc_count; ++i)
//{
//	rapidjson::Value auc_info;
//	auc_info.SetObject();

//	for( size_t j=0 ; j<outer_field_count; ++j )
//	{
//		auc_info.AddMember(vec[j].c_str(), str, allocator);
//	}

//	common_info.PushBack(auc_info, allocator);
//}

ValueW auction_info;
auction_info.SetObject();
auction_info.AddMember(L"auctions", common_info, allocator);

document1.AddMember(L"main", auction_info, allocator);
document1.AddMember(L"bStatus", true, allocator);
document1.AddMember(L"sUserName", L"wuxingbin", allocator);
document1.AddMember(L"sPassWord", L"dhsjdsevjhvsvdss", allocator);
document1.AddMember(L"nId", 895634, allocator);
document1.AddMember(L"dPos", 5624.1546, allocator);

StringBufferW buffer;
Writer<StringBufferW, UTF16<>, UTF16<>> sdde(buffer);

document1.Accept(sdde);


CString drewd = buffer.GetString() ;



DocumentW document;  
if (document.Parse<0>(sResult).HasParseError())  
{  

}  
if(document.HasMember(L"bStatus"))
{
bool sss = document[L"bStatus"].GetBool();
sss = true;
}
*/



#endif	//__RAPID_JSON_H__