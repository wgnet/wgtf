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

namespace wgt
{

class IComponentContext;
class IDefinitionManager;

namespace ReflectionShared
{

REFLECTION_DLL void initContext(IComponentContext & componentContext);
REFLECTION_DLL void initDefinitionManager(IDefinitionManager & defManager);

} // end namespace ReflectionShared

} //end namespace wgt
 

#endif