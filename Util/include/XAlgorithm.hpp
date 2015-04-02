#pragma once
#ifndef X_ALGORITHM_HPP
#define X_ALGORITHM_HPP

namespace Util
{
	template<typename InputItr, typename Func, typename Result = decltype(Func())>
	Result Reduce(InputItr begin, InputItr end, Result initialValue, Func func)
	{
		auto result = initialValue;
		for (begin; begin != end; ++begin)
		{
			result = func(result, *begin);
		}
		return result;
	}
}

#endif