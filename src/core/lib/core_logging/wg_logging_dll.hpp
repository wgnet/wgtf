#ifndef WG_LOGGING_DLL

#include "core_common/platform_export.hpp"

#if defined(BW_SHARED_LIBS)
#if defined(WG_LOGGING_EXPORT)
#define WG_LOGGING_DLL PLATFORM_EXPORT
#else
#define WG_LOGGING_DLL PLATFORM_IMPORT
#endif
#else
#define WG_LOGGING_DLL
#endif

#endif
