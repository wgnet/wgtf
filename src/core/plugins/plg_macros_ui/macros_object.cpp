#include "macros_object.hpp"
#include "core_command_system/i_command_manager.hpp"
// TODO: remove this pragma
#pragma warning(push)
#pragma warning(disable : 4996)
#include "core_data_model/generic_list.hpp"
#pragma warning(pop)
#include "core_command_system/macro_object.hpp"
#include "core_command_system/compound_command.hpp"
#include "core_logging/logging.hpp"
#include <cassert>

namespace wgt
{
//==============================================================================
MacrosObject::MacrosObject() : commandSystem_(nullptr), currentIndex_(-1)
{
}

//==============================================================================
void MacrosObject::init(ICommandManager& commandSystem)
{
	commandSystem_ = &commandSystem;
}

//==============================================================================
const IListModel* MacrosObject::getMacros() const
{
	assert(commandSystem_ != nullptr);
	return &commandSystem_->getMacros();
}

//==============================================================================
void MacrosObject::setSelectedRow(const int index)
{
	currentIndex_ = index;
}

//==============================================================================
ObjectHandle MacrosObject::getSelectedCompoundCommand() const
{
	assert(commandSystem_ != nullptr);
	typedef GenericListT<ObjectHandleT<CompoundCommand>> MacroList;
	const MacroList& macros = static_cast<const MacroList&>(commandSystem_->getMacros());
	if (currentIndex_ == -1)
	{
		NGT_ERROR_MSG("Please select a macro. \n");
		return nullptr;
	}
	assert(currentIndex_ >= 0 && currentIndex_ < static_cast<int>(macros.size()));
	const ObjectHandleT<CompoundCommand>& macro = macros[currentIndex_];
	if (macro == nullptr)
	{
		NGT_ERROR_MSG("The macro does not exist. \n");
		return nullptr;
	}
	return macro;
}
} // end namespace wgt
