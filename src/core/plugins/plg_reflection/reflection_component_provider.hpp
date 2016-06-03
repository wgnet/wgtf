#ifndef REFLECTION_COMPONENT_PROVIDER_HPP
#define REFLECTION_COMPONENT_PROVIDER_HPP

#include "core_ui_framework/i_component_provider.hpp"
#include "core_reflection/i_definition_manager.hpp"

namespace wgt
{
class ReflectionComponentProvider : public IComponentProvider
{
public:
	ReflectionComponentProvider( IDefinitionManager & defManager )
		: defManager_( defManager )
	{
	}

	const char * componentId( const TypeId & typeId,
		std::function< bool ( size_t ) > & predicate ) const
	{
		if (typeId.isPointer())
		{
			auto typeDef = defManager_.getDefinition( typeId.removePointer().getName() );
			if (typeDef != nullptr)
			{
				return "polystruct";
			}
		}

		return nullptr;
	}

public:
	IDefinitionManager & defManager_;
};
} // end namespace wgt
#endif//REFLECTION_COMPONENT_PROVIDER_HPP
