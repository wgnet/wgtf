#ifndef WG_MEMORY_DLL

#include "core_common/platform_export.hpp"

#if defined(BW_SHARED_LIBS)
#if defined(WG_MEMORY_EXPORT)
#define WG_MEMORY_DLL PLATFORM_EXPORT
#else
#define WG_MEMORY_DLL PLATFORM_IMPORT
#endif
#else
#define WG_MEMORY_DLL
#endif

#endif
