#pragma once
#ifndef RAW_PARSER_HPP
#define RAW_PARSER_HPP

#include <string>
#include "boost/detail/endian.hpp"

namespace Util
{
	template <typename T_Number, typename T_Data>
	T_Number ReadInteger(T_Data rawData)
	{
		static_assert(std::is_integral<T_Number>::value, "Read integer must be provided an integral value!");
		T_Number value = *(reinterpret_cast<const T_Number*>(&*rawData));

#ifdef BOOST_LITTLE_ENDIAN
		//then we need to swap the bytes
		std::reverse(reinterpret_cast<char*>(&value), reinterpret_cast<char*>(&value) + sizeof(T_Number));
#endif

		return value;
	}

	// the warning can be safely disabled as there are static asserts to ensure that there cannot
	// be a bad copy or buffer overrun
#pragma warning(push)
#pragma warning(disable : 4996)

	template <typename T_Number, typename T_Data, uint32_t countBytes>
	T_Number ReadInteger(T_Data rawData)
	{
		static_assert(std::is_integral<T_Number>::value, "Read integer must be provided an integral value!");
		static_assert(sizeof(T_Number) >= countBytes, "T_Number must be large enough to store the number of bytes read into it!");

		T_Number value = 0;
		std::copy(rawData, rawData + countBytes, reinterpret_cast<char*>(&value));

		//Note: this is if it is not defined, we read it in as though it were already in little endian format!
#ifdef BOOST_LITTLE_ENDIAN
		//then we need to swap the bytes
		std::reverse(reinterpret_cast<char*>(&value), reinterpret_cast<char*>(&value) + countBytes);
#endif

		return value;
	}

#pragma warning(pop)
}

#endif