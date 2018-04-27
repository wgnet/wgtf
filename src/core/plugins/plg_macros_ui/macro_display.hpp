#ifndef MACRO_DISPLAY_HPP
#define MACRO_DISPLAY_HPP
#include "core_command_system/compound_command.hpp"
#include "core_reflection/reflected_object.hpp"
#include "macros_model.hpp"

namespace wgt
{
class IComponentContext;

class MacroDisplay
{
	DECLARE_REFLECTED
public:
	MacroDisplay();
	const char* getMacroId() const;
	const std::string& getName() const;
	void setName(const std::string& name);
	const AbstractListModel* getMacroSteps() const;
	void init(CompoundCommand* macro);
	CompoundCommand* getMacro() const;

private:
	std::string name_;
	MacroStepsModel stepsModel_;
	CompoundCommand* macro_;
};
} // end namespace wgt
#endif // MACRO_DISPLAY_HPP
