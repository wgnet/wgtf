#ifndef TEST_COMMAND_HPP
#define TEST_COMMAND_HPP

#include "core_command_system/command.hpp"

namespace wgt
{
/// Base class for TestCommand classes
class TestCommandBase
	: public Command
{
public:
	TestCommandBase( uint8_t processCounter = 0 )
		: processCounter_( processCounter )
	{}

	bool canUndo( const ObjectHandle & arguments ) const override { return false; }

protected:
	uint8_t processCounter_;
};

/// Test command class that will emulate 3 second wait and log the process to output window each second when the command is executed
class TestCommand1
	: public TestCommandBase
{
public:
	TestCommand1( uint8_t processCounter = 3 )
		: TestCommandBase( processCounter )
	{}

	const char * getId() const override;
	ObjectHandle execute( const ObjectHandle & arguments ) const override;
};

/// Test command class that will emulate 5 second wait and log the process to output window each second when the command is executed
class TestCommand2
	: public TestCommandBase
{
public:
	TestCommand2( uint8_t processCounter = 5 )
		: TestCommandBase( processCounter )
	{}

	const char * getId() const override;
	ObjectHandle execute( const ObjectHandle & arguments ) const override;
};
} // end namespace wgt
#endif //TEST_COMMAND_HPP
