#ifndef TEST_COMMAND_SYSTEM_FIXTURE_HPP
#define TEST_COMMAND_SYSTEM_FIXTURE_HPP

#include <memory>
#include "core_command_system/i_command_event_listener.hpp"

namespace wgt
{
class ObjectManager;
class CommandManager;
class ICommandManager;
class Command;
class IDefinitionManager;
class IObjectManager;
class ReflectionController;
class TestApplication;
class IEnvManager;

class TestCommandSystemFixture
	: public ICommandEventListener
{
public:
	TestCommandSystemFixture();
	~TestCommandSystemFixture();
	IObjectManager & getObjectManager() const;
	IDefinitionManager & getDefinitionManager() const;
	ICommandManager & getCommandSystemProvider() const;
	ReflectionController & getReflectionController() const;
	const ICommandEventListener::MultiCommandStatus & getMultiCommandStatus() const;
	void multiCommandStatusChanged( ICommandEventListener::MultiCommandStatus multiCommandStatus ) const override;

private:
	std::unique_ptr< TestApplication > application_;
	std::unique_ptr< ObjectManager > objectManager_;
	std::unique_ptr< IDefinitionManager > definitionManager_;
	std::unique_ptr< CommandManager > commandManager_;
	std::unique_ptr< Command > setReflectedPropertyCmd_;
	std::unique_ptr< Command > invokeReflectedMethodCmd_;
	std::unique_ptr< ReflectionController > reflectionController_;
	std::unique_ptr< IEnvManager > envManager_;
	mutable ICommandEventListener::MultiCommandStatus multiCommandStatus_;

};
} // end namespace wgt
#endif //TEST_COMMAND_SYSTEM_FIXTURE_HPP
