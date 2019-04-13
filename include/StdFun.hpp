/* 封装一些 std 的方法

*/

#pragma once
#ifndef __STD_FUN_HPP___
#define __STD_FUN_HPP___

class StdFun
{
public:
	template<class Container, class Item>
	static int DelContainerIter(Container& lists, Item p)
	{
		int count = 0;
		auto ite = lists.begin();
		while (ite != lists.end())
		{
			if (*ite == p)
			{
				ite = lists.erase(ite);
				++count;
				continue;
			}

			++ite;
		}
		return count;
	}

};



#endif		// __STD_FUN_HPP___