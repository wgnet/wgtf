#ifndef BATCH_COMMAND_HPP
#define BATCH_COMMAND_HPP

#include "command.hpp"

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

class BatchCommand : public Command
{
public:
	const char* getId() const override;
	ObjectHandle execute(const ObjectHandle& arguments) const override;
	CommandThreadAffinity threadAffinity() const override;

private:
	friend CommandManagerImpl;
	BatchCommand(CommandManager* pCommandManager);
	CommandManager* pCommandManager_;
};
} // end namespace wgt
#endif // BATCH_COMMAND_HPP
