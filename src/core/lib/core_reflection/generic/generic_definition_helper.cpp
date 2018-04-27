#include "generic_definition_helper.hpp"

#include "generic_object.hpp"

#include "core_common/assert.hpp"

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
	TF_ASSERT(genericObject != nullptr);
	return genericObject->getDefinition();
}
} // end namespace wgt
