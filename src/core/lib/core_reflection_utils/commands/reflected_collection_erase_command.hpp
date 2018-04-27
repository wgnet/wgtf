#ifndef REFLECTED_COLLECTION_ERASE_COMMAND_HPP
#define REFLECTED_COLLECTION_ERASE_COMMAND_HPP

#include "core_command_system/command.hpp"

namespace wgt
{
class ReflectedCollectionEraseCommand;

class ReflectedCollectionEraseCommandParameters
{
public:
	RefObjectId id_;
	std::string path_;
	Variant key_;

private:
	Variant value_;
	bool erased_;

	friend class ReflectedCollectionEraseCommand;
};

class ReflectedCollectionEraseCommand : public Command
{
public:
	ReflectedCollectionEraseCommand(IDefinitionManager& definitionManager);
	virtual ~ReflectedCollectionEraseCommand();

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
#endif // REFLECTED_COLLECTION_ERASE_COMMAND_HPP
