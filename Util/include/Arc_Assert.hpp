#pragma once
#ifndef ARC_ASSERT_HPP
#define ARC_ASSERT_HPP

#include <iostream>
#include <string>

#if _DEBUG

#define ARC_FAIL(msg) { std::cerr << msg << std::endl; __debugbreak(); }
#define ARC_ASSERT(statement) if (!(statement)) { __debugbreak(); }
#define ARC_ASSERT_MSG(statement, msg) if(!(statement)) { std::cerr << msg << std::endl; __debugbreak(); }

#define ARC_ThrowIfFailed(hresult)								\
	if(FAILED(hresult))											\
	{															\
		__debugbreak();											\
		throw Platform::Exception::CreateException(hresult);	\
	}															\

#else

#define ARC_FAIL(msg)
#define ARC_ASSERT(statement)
#define ARC_ASSERT_MSG(statement, msg)
#define ARC_ThrowIfFailed(hresult)

#endif

#endif