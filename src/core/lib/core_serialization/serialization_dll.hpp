#ifndef SERIALIZATION_DLL

#include "core_common/platform_export.hpp"

#if defined(BW_SHARED_LIBS)
#	if defined(SERIALIZATION_EXPORT)
#		define SERIALIZATION_DLL PLATFORM_EXPORT
#	else
#		define SERIALIZATION_DLL PLATFORM_IMPORT
#	endif
#else
#	define SERIALIZATION_DLL
#endif

#endif