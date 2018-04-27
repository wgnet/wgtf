#pragma once

#include "core_command_system/command.hpp"
#include "core_reflection/reflected_object.hpp"
#include "core_dependency_system/depends.hpp"

#include <memory>

namespace wgt
{
class IComponentContext;

/**
 *	Type of command for inserting data into an AbstractItemModel.
 *	Currently data cannot be serialized to history.
 */
class InsertDefaultCommand : public Command, Depends<IDefinitionManager>
{
public:
	virtual ~InsertDefaultCommand();

	virtual bool customUndo() const override;
	virtual bool canUndo(const ObjectHandle& arguments) const override;
	virtual bool undo(const ObjectHandle& arguments) const override;
	virtual bool redo(const ObjectHandle& arguments) const override;
	virtual CommandDescription getCommandDescription(const ObjectHandle& arguments) const override;
	virtual const char* getId() const override;
	virtual Variant execute(const ObjectHandle& arguments) const override;
	virtual bool validateArguments(const ObjectHandle& arguments) const override;
	virtual CommandThreadAffinity threadAffinity() const override;
	virtual ManagedObjectPtr copyArguments(const ObjectHandle& arguments) const override;
};

} // end namespace wgt
