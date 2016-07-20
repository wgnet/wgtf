#ifndef DEPRECATED_HPP_INCLUDED
#define DEPRECATED_HPP_INCLUDED

#if defined(WARN_DEPRECATED)

#if defined(_MSC_VER)
#define DEPRECATED __declspec(deprecated)
#elif defined(__GNUC__) || defined(__clang__)
#define DEPRECATED __attribute__ ((deprecated))
#else
#pragma message("WARNING: You need to implement DEPRECATED for this compiler")
#define DEPRECATED
#endif

#else

#define DEPRECATED

#endif

#endif // DEPRECATED_HPP_INCLUDED
