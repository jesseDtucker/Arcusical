#pragma once

#ifdef ARC_DLL
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif
