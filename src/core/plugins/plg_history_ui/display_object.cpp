#include "display_object.hpp"

#include "core_common/assert.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_reflection/i_object_manager.hpp"
#include "core_reflection/generic/generic_object.hpp"
#include "core_reflection_utils/commands/reflectedproperty_undoredo_helper.hpp"
#include "core_reflection/metadata/meta_impl.hpp"
#include "core_reflection/metadata/meta_utilities.hpp"
#include "core_reflection_utils/commands/set_reflectedproperty_command.hpp"

namespace wgt
{
//==============================================================================
DisplayObject::DisplayObject() : data_(nullptr)
{
}

//==============================================================================
void DisplayObject::init(IDefinitionManager& defManager, const CommandInstancePtr& instance)
{
	TF_ASSERT(instance != nullptr);
	data_ = instance->getCommandDescription();
}

//==============================================================================
ObjectHandle DisplayObject::getDisplayData() const
{
	return data_;
}
} // end namespace wgt
