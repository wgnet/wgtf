#ifndef BATCH_COMMAND_HPP
#define BATCH_COMMAND_HPP

#include "command.hpp"
#include <string>

namespace wgt
{
class ObjectHandle;
class IDataStream;
class CommandManager;
namespace
{
class CommandManagerImpl;
}

enum class BatchCommandStage : uint8_t
{
	Begin = 0,
	End = 1,
	Abort = 2,
};

class BatchCommandParameters
{
public:
	BatchCommandStage stage_;
	std::string description_;
};

class BatchCommand : public Command
{
public:
	const char* getId() const override;
	Variant execute(const ObjectHandle& arguments) const override;
	CommandThreadAffinity threadAffinity() const override;
	ManagedObjectPtr copyArguments(const ObjectHandle& arguments) const override;
	CommandDescription getCommandDescription(const ObjectHandle& arguments) const override;

private:
	friend CommandManagerImpl;
	BatchCommand(CommandManager* pCommandManager);
	CommandManager* pCommandManager_;
};
} // end namespace wgt
#endif // BATCH_COMMAND_HPP
