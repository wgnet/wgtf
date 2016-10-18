#ifndef PLATFORM_EXPORT_HPP_INCLUDED
#define PLATFORM_EXPORT_HPP_INCLUDED

#if defined(_MSC_VER)

#define PLATFORM_EXPORT __declspec(dllexport)
#define PLATFORM_IMPORT __declspec(dllimport)

#else

// assume gcc or clang
#define PLATFORM_EXPORT __attribute__((visibility("default")))
#define PLATFORM_IMPORT

#endif


#endif
