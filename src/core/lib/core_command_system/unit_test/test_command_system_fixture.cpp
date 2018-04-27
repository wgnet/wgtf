#include "pch.hpp"
#include "core_unit_test/test_application.hpp"
#include "test_command_system_fixture.hpp"
#include "core_unit_test/test_definition_manager.hpp"
#include "core_unit_test/test_object_manager.hpp"
#include "core_command_system/command_manager.hpp"
#include "core_reflection_utils/commands/set_reflectedproperty_command.hpp"
#include "core_reflection_utils/commands/invoke_reflected_method_command.hpp"
#include "core_reflection_utils/reflection_controller.hpp"
#include "core_environment_system/env_system.hpp"
#include "core_unit_test/test_framework.hpp"

#include "reflection_auto_reg.mpp"
#include "core_reflection/utilities/reflection_auto_register.hpp"

namespace wgt
{
//==============================================================================
TestCommandSystemFixture::TestCommandSystemFixture()
    : application_(new TestApplication())
	, framework_(new TestFramework())
	, envManager_(new EnvManager())
	, commandManager_(new CommandManager(*envManager_))
	, setReflectedPropertyCmd_(new SetReflectedPropertyCommand(getDefinitionManager()))
	, invokeReflectedMethodCmd_(new InvokeReflectedMethodCommand(getDefinitionManager()))
	, reflectionController_(new ReflectionController())
	, multiCommandStatus_(MultiCommandStatus_Begin)
{
	ReflectionAutoRegistration::initAutoRegistration(getDefinitionManager());
	commandManager_->init(*application_, getDefinitionManager() );
	commandManager_->registerCommand(setReflectedPropertyCmd_.get());
	commandManager_->registerCommand(invokeReflectedMethodCmd_.get());

	reflectionController_->init(*commandManager_);
	commandManager_->registerCommandStatusListener(this);
}

//==============================================================================
TestCommandSystemFixture::~TestCommandSystemFixture()
{
	commandManager_->deregisterCommandStatusListener(this);

	commandManager_->deregisterCommand(invokeReflectedMethodCmd_->getId());
	commandManager_->deregisterCommand(setReflectedPropertyCmd_->getId());
	commandManager_->fini();

	invokeReflectedMethodCmd_.reset();
	setReflectedPropertyCmd_.reset();
	reflectionController_.reset();
	commandManager_.reset();
}

//==============================================================================
IObjectManager& TestCommandSystemFixture::getObjectManager() const
{
	return framework_->getObjectManager();
}

//==============================================================================
IDefinitionManager& TestCommandSystemFixture::getDefinitionManager() const
{
	return framework_->getDefinitionManager();
}

//==============================================================================
ICommandManager& TestCommandSystemFixture::getCommandSystemProvider() const
{
	return *commandManager_;
}

//==============================================================================
ReflectionController& TestCommandSystemFixture::getReflectionController() const
{
	return *reflectionController_;
}

//==============================================================================
const ICommandEventListener::MultiCommandStatus& TestCommandSystemFixture::getMultiCommandStatus() const
{
	return multiCommandStatus_;
}

//==============================================================================
void TestCommandSystemFixture::multiCommandStatusChanged(
ICommandEventListener::MultiCommandStatus multiCommandStatus) const
{
	multiCommandStatus_ = multiCommandStatus;
}
} // end namespace wgt
