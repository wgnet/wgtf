#pragma once

#include "core_command_system/command.hpp"
#include "core_reflection/reflected_object.hpp"
#include "core_dependency_system/di_ref.hpp"

namespace wgt
{
class IComponentContext;

/**
 *	Type of command for inserting data into an AbstractItemModel.
 *	Currently data cannot be serialized to history.
 */
class InsertItemCommand
: public Command
{
public:
	InsertItemCommand(IComponentContext& context);

	virtual bool customUndo() const override;
	virtual bool canUndo(const ObjectHandle& arguments) const override;
	virtual bool undo(const ObjectHandle& arguments) const override;
	virtual bool redo(const ObjectHandle& arguments) const override;
	virtual ObjectHandle getCommandDescription(
	const ObjectHandle& arguments) const override;
	virtual const char* getId() const override;
	virtual ObjectHandle execute(const ObjectHandle& arguments) const override;
	virtual bool validateArguments(const ObjectHandle& arguments) const override;
	virtual CommandThreadAffinity threadAffinity() const override;

private:
	DIRef<IDefinitionManager> definitionManager_;
};

} // end namespace wgt
