#ifndef REFLECTION_DLL

#include "core_common/platform_export.hpp"

#if defined(BW_SHARED_LIBS)
#if defined(REFLECTION_EXPORT)
#define REFLECTION_DLL PLATFORM_EXPORT
#else
#define REFLECTION_DLL PLATFORM_IMPORT
#endif
#else
#define REFLECTION_DLL
#endif

#endif