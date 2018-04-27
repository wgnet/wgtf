#ifndef REFLECTED_COLLECTION_INSERT_COMMAND_HPP
#define REFLECTED_COLLECTION_INSERT_COMMAND_HPP

#include "core_command_system/command.hpp"

namespace wgt
{
class ReflectedCollectionInsertCommandParameters
{
public:
	RefObjectId id_;
	std::string path_;
	Variant key_;
	Variant value_;

private:
	bool inserted_;

	friend class ReflectedCollectionInsertCommand;
};

class ReflectedCollectionInsertCommand : public Command
{
public:
	ReflectedCollectionInsertCommand(IDefinitionManager& definitionManager);
	virtual ~ReflectedCollectionInsertCommand();

	const char* getId() const override;
	const char* getName() const override;
	virtual Variant execute(const ObjectHandle& arguments) const override;
	bool customUndo() const override;
	bool undo(const ObjectHandle& arguments) const override;
	bool redo(const ObjectHandle& arguments) const override;
	CommandThreadAffinity threadAffinity() const override;
    CommandDescription getCommandDescription(const ObjectHandle& arguments) const override;
	ManagedObjectPtr copyArguments(const ObjectHandle& arguments) const override;

private:
	IDefinitionManager& definitionManager_;
};
} // end namespace wgt
#endif // REFLECTED_COLLECTION_INSERT_COMMAND_HPP
