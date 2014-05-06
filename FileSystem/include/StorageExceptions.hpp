#pragma  once
#ifndef STORAGE_EXCEPTIONS_HPP

#include <stdexcept>

namespace FileSystem
{
	class ReadTooLongException : public std::runtime_error
	{
	public:
		ReadTooLongException(std::string msg)
			:std::runtime_error(msg)
		{}
	};
}

#endif