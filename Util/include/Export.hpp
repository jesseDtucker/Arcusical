#ifndef EXPORT_HPP
#define EXPORT_HPP

#ifdef ARC_DLL
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

#endif