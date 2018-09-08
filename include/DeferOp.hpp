/**
* fun	Defer(); 括号里 就可以直接放 代码 可以直接用本地参数
* Describe    模拟golang  里 defer 功能
*/
#ifndef __DEFER_H_
#define __DEFER_H_
#pragma once

#define CONCAT_(a, b) a ## b
#define CONCAT(a, b) CONCAT_(a,b)
// 延迟 处理 一般在 作用域结束时 执行
#define Defer(fn) DeferOp CONCAT(__defer__, __LINE__) = [&](){fn}

class DeferOp
{
public:
	template<class Callable>
	DeferOp(Callable &&fn)
		: m_fun(std::forward<Callable>(fn))
	{

	}
	DeferOp(DeferOp &&other)
		: m_fun(std::move(other.m_fun))
	{
		other.m_fun = nullptr;
	}
	~DeferOp()
	{
		if (nullptr != m_fun)
		{
			m_fun();
		}
	}
	DeferOp(const DeferOp&) = delete;
	void operator=(const DeferOp &) = delete;
protected:
	std::function<void()> m_fun;
};

#endif	//__AUTO_DELETE_H_