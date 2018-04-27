#ifndef MACROS_OBJECT_HPP
#define MACROS_OBJECT_HPP

#include "core_reflection/object_handle.hpp"
#include "core_dependency_system/depends.hpp"
#include "macros_model.hpp"
#include "core_command_system/i_command_manager.hpp"

namespace wgt
{

/**
 *	Wrapper for accessing the current position in the undo/redo list from QML.
 */
class MacrosObject : Depends<ICommandManager>
{
public:
	// ClassDefinition::create() requires a default constructor
	MacrosObject();

	const AbstractListModel* getMacros() const;

private:
	MacrosModel macrosModel_;
};
} // end namespace wgt
#endif // MACROS_OBJECT_HPP
