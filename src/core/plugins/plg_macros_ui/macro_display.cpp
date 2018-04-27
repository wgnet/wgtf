#include "macro_display.hpp"

#include "core_common/assert.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"

namespace wgt
{
//==============================================================================
MacroDisplay::MacroDisplay() : macro_(nullptr)
{
}

void MacroDisplay::init(CompoundCommand* macro)
{
	macro_ = macro;
	name_ = macro_->getName();
	stepsModel_.setSource(macro_->getSubCommands());
}

const char* MacroDisplay::getMacroId() const
{
	TF_ASSERT(macro_ != nullptr);
	return macro_->getId();
}

const std::string& MacroDisplay::getName() const
{
	return name_;
}

void MacroDisplay::setName(const std::string& name)
{
	name_ = name;
}

const AbstractListModel* MacroDisplay::getMacroSteps() const
{
	return &stepsModel_;
}

CompoundCommand* MacroDisplay::getMacro() const
{
	return macro_;
}

} // end namespace wgt
