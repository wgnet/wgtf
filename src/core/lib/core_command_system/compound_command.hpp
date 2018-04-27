#ifndef COMPOUND_COMMAND_HPP
#define COMPOUND_COMMAND_HPP

#include "command.hpp"
#include "core_reflection/reflected_object.hpp"

namespace wgt
{
class IReflectionController;

class CompoundCommand : public Command
{
public:
	friend CommandManagerImpl;
	typedef std::vector<std::pair<std::string, ManagedObjectPtr>> SubCommandStorage;
	typedef std::vector<std::pair<std::string, ObjectHandle>> SubCommandHandles;
	CompoundCommand(const char* id);
	~CompoundCommand();

	const char* getId() const override;
	const char* getName() const override;
	Variant execute(const ObjectHandle& arguments) const override;
	bool validateArguments(const ObjectHandle& arguments) const override;
	CommandThreadAffinity threadAffinity() const override;
	virtual bool customUndo() const override;
	virtual bool undo(const ObjectHandle& arguments) const override;
	virtual bool redo(const ObjectHandle& arguments) const override;

	void addCommand(const char* commandId, const ObjectHandle& commandArguments);
	const Collection& getSubCommands() const override;
	void setName(const char* name);
	ManagedObjectPtr copyArguments(const ObjectHandle& arguments) const override;

private:
	SubCommandStorage subCommandStorage_;
	SubCommandHandles subCommandHandles_;
	mutable std::vector<CommandInstancePtr> subInstances_;
	std::string id_;
	std::string name_;
};
} // end namespace wgt
#endif // COMPOUND_COMMAND_HPP
