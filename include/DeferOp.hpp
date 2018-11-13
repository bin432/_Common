/**
* fun	Defer(); ������ �Ϳ���ֱ�ӷ� ���� ����ֱ���ñ��ز���
* Describe    ģ��golang  �� defer ����
*/
#ifndef __DEFER_H_
#define __DEFER_H_
#pragma once
#include <functional>

#define CONCAT_(a, b) a##b
#define CONCAT(a, b) CONCAT_(a,b)
/*
eg. defer(����);  ע����� һ��Ҫ���� ;
*/

#define defer(code) DeferOp CONCAT(_defer_, __LINE__) = [&](){code}

class DeferOp
{
public:
	DeferOp(std::function<void()>&& fn)
		: m_fun(std::move(fn))
	{}
	~DeferOp()
	{
		if (nullptr != m_fun)
		{
			m_fun();
		}
	}

#if _MSC_VER >= 1700	//VS2012
	DeferOp(DeferOp &&other) = delete;
	DeferOp(const DeferOp&) = delete;
	void operator=(const DeferOp &) = delete;
#else
	DeferOp(DeferOp &&other);
	DeferOp(const DeferOp&);
	void operator=(const DeferOp &);
#endif
protected:
	std::function<void()> m_fun;
};

#endif	//__DEFER_H_