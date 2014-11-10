/************************************************************************/
/* Provides some wrappers on common casting operations that include asserts
/************************************************************************/

#include <limits>
#include <type_traits>

#include "Arc_Assert.hpp"

#ifdef max
#undef max
#endif

namespace Util
{
	template<typename Smaller, typename Larger>
	typename std::enable_if<!(std::is_signed<Larger>::value && std::is_unsigned<Smaller>::value), Smaller>::type SafeIntCast(Larger num)
	{
			static_assert(std::is_integral<Smaller>::value, "Cannot cast ints if the value is not integral!");
			static_assert(std::is_integral<Larger>::value, "Cannot cast ints if the value is not integral!");

			ARC_ASSERT_MSG(num <= std::numeric_limits<Smaller>::max(), "Overflow when casting integral to smaller size!");

			return static_cast<Smaller>(num);
	}

	template<typename Smaller, typename Larger>
	typename std::enable_if<std::is_signed<Larger>::value && std::is_unsigned<Smaller>::value, Smaller>::type SafeIntCast(Larger num)
	{
		static_assert(std::is_integral<Smaller>::value, "Cannot cast ints if the value is not integral!");
		static_assert(std::is_integral<Larger>::value, "Cannot cast ints if the value is not integral!");
	
		ARC_ASSERT_MSG(num <= std::numeric_limits<Smaller>::max(), "Overflow when casting integral to smaller size!");
		// we are assigning an unsigned value to a signed value, make sure there is no overflow
		ARC_ASSERT_MSG(num >= 0, "Attempted to assign a negative value to an unsigned number!");

		return static_cast<Smaller>(num);
	}
}