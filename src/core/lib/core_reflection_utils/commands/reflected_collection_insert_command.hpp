#ifndef REFLECTED_COLLECTION_INSERT_COMMAND_HPP
#define REFLECTED_COLLECTION_INSERT_COMMAND_HPP

#include "core_command_system/command.hpp"

namespace wgt
{
class ReflectedCollectionInsertCommandParameters
{
	DECLARE_REFLECTED
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
	ObjectHandle execute(const ObjectHandle& arguments) const override;
	bool customUndo() const override;
	bool undo(const ObjectHandle& arguments) const override;
	bool redo(const ObjectHandle& arguments) const override;
	CommandThreadAffinity threadAffinity() const override;

	ObjectHandle getCommandDescription(const ObjectHandle& arguments) const override;

private:
	IDefinitionManager& definitionManager_;
};
} // end namespace wgt
#endif // REFLECTED_COLLECTION_INSERT_COMMAND_HPP
