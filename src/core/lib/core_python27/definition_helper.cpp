#include "pch.hpp"
#include "definition_helper.hpp"
#include "defined_instance.hpp"
#include "core_variant/type_id.hpp"

namespace wgt
{
namespace ReflectedPython
{
	TypeId DefinitionHelper::typeId() const
	{
		static TypeId id = TypeId::getType< DefinedInstance >();
		return id;
	}

	IClassDefinition * DefinitionHelper::getDefinition( const ObjectHandle & object ) const
	{
		auto instance = object.getBase< DefinedInstance >();
		assert( instance != nullptr );
		return instance->getDefinition();
	}
}
} // end namespace wgt
