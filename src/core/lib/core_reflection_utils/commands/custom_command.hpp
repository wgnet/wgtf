#ifndef CUSTOM_COMMAND_HPP
#define CUSTOM_COMMAND_HPP

#include "core_command_system/command.hpp"
#include "core_reflection/reflected_object.hpp"

#include <memory>

namespace wgt
{
using ExecuteFunc = std::function<bool ()>;
using UndoFunc = std::function<bool ()>;

class CustomCommandParameters final
{
	DECLARE_REFLECTED
public:
	std::string description_;
	ExecuteFunc execute_;
	UndoFunc undo_;
};

class CustomCommand : public Command
{
public:
	bool customUndo() const override { return true; }
	const char* getId() const override;
	const char* getName() const override;
	bool validateArguments(const ObjectHandle& arguments) const override;
	virtual Variant execute(const ObjectHandle& arguments) const override;
	CommandThreadAffinity threadAffinity() const override;
	bool undo(const ObjectHandle& arguments) const override;
	bool redo(const ObjectHandle& arguments) const override;
	virtual CommandDescription getCommandDescription(const ObjectHandle& arguments) const override;
	ManagedObjectPtr copyArguments(const ObjectHandle& arguments) const override;
};
} // end namespace wgt
#endif // CUSTOM_COMMAND_HPP
