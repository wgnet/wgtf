#ifndef __PLATFORM_STD_HPP__
#define __PLATFORM_STD_HPP__

#if defined( _WIN32 )
#define NOEXCEPT
#endif

#ifdef __APPLE__
#define NOEXCEPT noexcept
#endif // __APPLE__

#endif // __PLATFORM_STD_HPP__
