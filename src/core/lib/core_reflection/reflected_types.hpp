#ifndef REFLECTED_TYPES_HPP
#define REFLECTED_TYPES_HPP

#include "reflection_dll.hpp"

namespace wgt
{
class IDefinitionManager;

namespace Reflection
{
	REFLECTION_DLL void initReflectedTypes( IDefinitionManager & definitionManager );
}
} // end namespace wgt
#endif
