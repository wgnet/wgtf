#ifndef VARIANT_DLL

#include "core_common/platform_export.hpp"

#if defined(BW_SHARED_LIBS)
#if defined(VARIANT_EXPORT)
#define VARIANT_DLL PLATFORM_EXPORT
#else
#define VARIANT_DLL PLATFORM_IMPORT
#endif
#else
#define VARIANT_DLL
#endif

#endif