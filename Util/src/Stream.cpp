#include <String>

#include "Stream.hpp"

namespace Util
{
	std::string Stream::ReadString(unsigned int count)
	{
		std::string value;
		auto rawBytes = GetBytes(count);
		value.append(rawBytes.data(), rawBytes.data() + count);
		return value;
	}

	std::vector<unsigned char> Stream::ReadRawBytes(unsigned int count)
	{
		return GetBytes(count);
	}
}