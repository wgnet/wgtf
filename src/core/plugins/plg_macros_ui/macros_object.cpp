#include "macros_object.hpp"

#include "core_generic_plugin/interfaces/i_component_context.hpp"

namespace wgt
{
//==============================================================================
MacrosObject::MacrosObject()
{
	auto commandSystem = get<ICommandManager>();
	macrosModel_.setSource(commandSystem->getMacros());
}

//==============================================================================
const AbstractListModel* MacrosObject::getMacros() const
{
	return &macrosModel_;
}
} // end namespace wgt
