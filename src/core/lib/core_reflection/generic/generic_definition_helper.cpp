#include "generic_definition_helper.hpp"
#include "generic_object.hpp"

namespace wgt
{
TypeId GenericDefinitionHelper::typeId() const
{
	static TypeId id = TypeId::getType<GenericObject>();
	return id;
}

IClassDefinition* GenericDefinitionHelper::getDefinition(const ObjectHandle& object) const
{
	auto genericObject = object.getBase<GenericObject>();
	assert(genericObject != nullptr);
	return genericObject->getDefinition();
}
} // end namespace wgt
