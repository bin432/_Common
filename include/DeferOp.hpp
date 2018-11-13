/**
* fun	Defer(); 括号里 就可以直接放 代码 可以直接用本地参数
* Describe    模拟golang  里 defer 功能
*/
#ifndef __DEFER_H_
#define __DEFER_H_
#pragma once
#include <functional>

#define CONCAT_(a, b) a##b
#define CONCAT(a, b) CONCAT_(a,b)
/*
eg. defer(代码);  注意后面 一定要加上 ;
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