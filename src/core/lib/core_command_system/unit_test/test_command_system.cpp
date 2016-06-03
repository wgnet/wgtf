#include "pch.hpp"

#include "core_unit_test/unit_test.hpp"

#include "test_objects.hpp"
#include "core_reflection/interfaces/i_base_property.hpp"
#include "core_reflection/property_accessor.hpp"
#include "core_reflection_utils/reflection_controller.hpp"
#include "core_command_system/i_command_manager.hpp"
#include "core_data_model/variant_list.hpp"
#include "core_command_system/compound_command.hpp"
#include "core_command_system/macro_object.hpp"

namespace wgt
{
TEST_F( TestCommandFixture, runSingleCommand )
{
	auto & controller = getReflectionController();

	auto objHandle = klass_->createManagedObject();

	PropertyAccessor counter = klass_->bindProperty("counter", objHandle );
	CHECK(counter.isValid());

	const int TEST_VALUE = 57;
	{
		int value = TEST_VALUE;
		controller.setValue( counter, value );
	}

	{
		int value = 0;
		Variant variant = controller.getValue( counter );
		CHECK(variant.tryCast( value ));
		CHECK_EQUAL(TEST_VALUE, value);
	}

}

TEST_F( TestCommandFixture, runBatchCommand )
{
	auto & controller = getReflectionController();

	auto objHandle = klass_->createManagedObject();

	const int TEST_VALUE = 57;
	const char * TEST_TEXT = "HelloCommand";

	PropertyAccessor counter = klass_->bindProperty("counter", objHandle );
	CHECK(counter.isValid());
	PropertyAccessor text = klass_->bindProperty( "text", objHandle );
	CHECK(text.isValid());
	PropertyAccessor incrementCounter = klass_->bindProperty("incrementCounter", objHandle );
	CHECK(incrementCounter.isValid());

	{
		int value = 0;
		Variant variantValue = controller.getValue( counter );
		CHECK( variantValue.tryCast( value ) );
		CHECK( TEST_VALUE != value );

		std::string text_value;
		Variant variantText = controller.getValue( text );
		CHECK( variantText.tryCast( text_value ) );
		CHECK( TEST_TEXT != text_value );
	}

	{
		int value = TEST_VALUE;
		auto & commandSystemProvider = getCommandSystemProvider();
		commandSystemProvider.beginBatchCommand();
		controller.setValue( counter, value );
		std::string text_value = TEST_TEXT;
		controller.setValue( text, text_value );
		commandSystemProvider.abortBatchCommand();
	}

	{
		int value = 0;
		Variant variantValue = controller.getValue( counter );
		CHECK(variantValue.tryCast( value ));
		CHECK(TEST_VALUE != value);

		std::string text_value;
		Variant variantText = controller.getValue( text );
		CHECK(variantText.tryCast( text_value ));
		CHECK(TEST_TEXT != text_value);
	}

	{
		int value = TEST_VALUE;
		auto & commandSystemProvider = getCommandSystemProvider();
		commandSystemProvider.beginBatchCommand();
		controller.setValue( counter, value );
		std::string text_value = TEST_TEXT;
		controller.setValue( text, text_value );
		commandSystemProvider.endBatchCommand();
	}

	{
		int value = 0;
		Variant variantValue = controller.getValue( counter );
		CHECK(variantValue.tryCast( value ));
		CHECK_EQUAL(TEST_VALUE, value);

		std::string text_value;
		Variant variantText = controller.getValue( text );
		CHECK(variantText.tryCast( text_value ));
		CHECK_EQUAL(TEST_TEXT, text_value);
	}

	{
		Variant variantValue;
		int value = 0;
		auto & commandSystemProvider = getCommandSystemProvider();
		commandSystemProvider.beginBatchCommand();
		controller.setValue( counter, 0 );
		commandSystemProvider.beginBatchCommand();
		controller.setValue( counter, 1 );
		commandSystemProvider.beginBatchCommand();
		controller.setValue( counter, 2 );
		commandSystemProvider.beginBatchCommand();
		controller.setValue( counter, 3 );
		variantValue = controller.getValue( counter );
		CHECK( variantValue.tryCast( value ) );
		CHECK( value == 3 );
		commandSystemProvider.abortBatchCommand();
		variantValue = controller.getValue( counter );
		CHECK( variantValue.tryCast( value ) );
		CHECK( value == 2 );
		commandSystemProvider.abortBatchCommand();
		variantValue = controller.getValue( counter );
		CHECK( variantValue.tryCast( value ) );
		CHECK( value == 1);
		commandSystemProvider.abortBatchCommand();
		variantValue = controller.getValue( counter );
		CHECK( variantValue.tryCast( value ) );
		CHECK( value == 0);
		commandSystemProvider.endBatchCommand();
		variantValue = controller.getValue( counter );
		CHECK( variantValue.tryCast( value ) );
		CHECK( value == 0);
	}

	{
		int value = 0;
		controller.setValue( counter, value );
		Variant variantValue = controller.getValue( counter );
		CHECK(variantValue.tryCast( value ));
		CHECK_EQUAL(0, value);

		auto & commandSystemProvider = getCommandSystemProvider();
		commandSystemProvider.beginBatchCommand();
		controller.invoke( incrementCounter, ReflectedMethodParameters() );
		controller.invoke( incrementCounter, ReflectedMethodParameters() );
		commandSystemProvider.endBatchCommand();
		variantValue = controller.getValue( counter );
		CHECK(variantValue.tryCast( value ));
		CHECK_EQUAL(2, value);

		commandSystemProvider.beginBatchCommand();
		controller.invoke( incrementCounter, ReflectedMethodParameters() );
		controller.invoke( incrementCounter, ReflectedMethodParameters() );
		commandSystemProvider.abortBatchCommand();
		variantValue = controller.getValue( counter );
		CHECK(variantValue.tryCast( value ));
		CHECK_EQUAL(2, value);
	}
}

TEST_F( TestCommandFixture, undo_redo )
{
	auto & controller = getReflectionController();

	auto objHandle = klass_->createManagedObject();

	PropertyAccessor counter = klass_->bindProperty("counter", objHandle );
	CHECK(counter.isValid());
	int oldValue = -1;
	{
		Variant variant = counter.getValue();
		CHECK(variant.tryCast( oldValue ));
	}
	auto & commandSystemProvider = getCommandSystemProvider();
	const int TEST_VALUE = 57;
	{
		int value = TEST_VALUE;
		
		controller.setValue( counter, value );
	}

	{
		int value = 0;
		Variant variant = controller.getValue( counter );
		CHECK(variant.tryCast( value ));
		CHECK_EQUAL(TEST_VALUE, value);
	}

	{
		commandSystemProvider.undo();
		CHECK(!commandSystemProvider.canUndo());
		CHECK(commandSystemProvider.canRedo());

		int value = 0;
		Variant variant = controller.getValue( counter );
		CHECK(variant.tryCast( value ));
		CHECK_EQUAL(oldValue, value);
	}

	{
		commandSystemProvider.redo();
		CHECK(commandSystemProvider.canUndo());
		CHECK(!commandSystemProvider.canRedo());

		int value = 0;
		Variant variant = controller.getValue( counter );
		CHECK(variant.tryCast( value ));
		CHECK_EQUAL(TEST_VALUE, value);
	}
}


TEST_F( TestCommandFixture, creatMacro )
{
	auto & controller = getReflectionController();

	auto objHandle = klass_->createManagedObject();

	PropertyAccessor counter = klass_->bindProperty("counter", objHandle );
	CHECK(counter.isValid());

	const int TEST_VALUE = 57;
	{
		int value = TEST_VALUE;
		controller.setValue( counter, value );
	}

	{
		// TODO: wait on controller
		controller.getValue( counter );
	}

	{
		auto & commandSystemProvider = getCommandSystemProvider();
		auto & history = commandSystemProvider.getHistory();
		commandSystemProvider.createMacro( history );
		CHECK(commandSystemProvider.getMacros().empty() == false );
	}
}

TEST_F( TestCommandFixture, executeMacro )
{
	auto & controller = getReflectionController();

	auto objHandle = klass_->createManagedObject();

	PropertyAccessor counter = klass_->bindProperty("counter", objHandle );
	CHECK(counter.isValid());

	const int TEST_VALUE = 57;
	{
		int value = TEST_VALUE;
		controller.setValue( counter, value );
	}

	{
		// TODO: wait on controller
		controller.getValue( counter );
	}

	{
		auto & commandSystemProvider = getCommandSystemProvider();
		commandSystemProvider.undo();
		auto & history = commandSystemProvider.getHistory();
		commandSystemProvider.createMacro( history, "Macro1" );
		CHECK(commandSystemProvider.getMacros().empty() == false );
		auto macroObj = static_cast<CompoundCommand*>( commandSystemProvider.findCommand( "Macro1" ) )->getMacroObject();
		auto instObj = macroObj.getBase<MacroObject>()->executeMacro();
		CommandInstancePtr inst = instObj.getBase<CommandInstance>();
		commandSystemProvider.waitForInstance( inst );
		{
			PropertyAccessor counter = klass_->bindProperty("counter", objHandle );
			int value = 0;
			Variant variant = controller.getValue( counter );
			CHECK(variant.tryCast( value ));
			CHECK_EQUAL(TEST_VALUE, value);
		}
	}
}


TEST_F( TestCommandFixture, threadCommands )
{
	// This test attempts to verify commands do not deadlock.
	// TODO: waitForInstance need to take a timeout to properly handle when this test fails
	auto & commandManager = getCommandSystemProvider();
	
	auto command = commandManager.queueCommand( TestThreadCommand::generateId( CommandThreadAffinity::UI_THREAD ).c_str() );
	commandManager.waitForInstance( command );

	command = commandManager.queueCommand( TestThreadCommand::generateId( CommandThreadAffinity::COMMAND_THREAD ).c_str() );
	commandManager.waitForInstance( command );

	command = commandManager.queueCommand( TestThreadCommand::generateId( CommandThreadAffinity::ANY_THREAD ).c_str() );
	commandManager.waitForInstance( command );
}


TEST_F( TestCommandFixture, compoundCommands )
{
	// This test attempts to verify commands do not deadlock.
	// TODO: waitForInstance need to take a timeout to properly handle when this test fails
	auto & commandManager = getCommandSystemProvider();

	auto command = commandManager.queueCommand( TestCompoundCommand::generateId( 1, CommandThreadAffinity::UI_THREAD ).c_str() );
	commandManager.waitForInstance( command );

	command = commandManager.queueCommand( TestCompoundCommand::generateId( 1, CommandThreadAffinity::COMMAND_THREAD ).c_str() );
	commandManager.waitForInstance( command );

	command = commandManager.queueCommand( TestCompoundCommand::generateId( 1, CommandThreadAffinity::ANY_THREAD ).c_str() );
	commandManager.waitForInstance( command );

	command = commandManager.queueCommand( TestCompoundCommand::generateId( 4, CommandThreadAffinity::UI_THREAD ).c_str() );
	commandManager.waitForInstance( command );

	command = commandManager.queueCommand( TestCompoundCommand::generateId( 4, CommandThreadAffinity::COMMAND_THREAD ).c_str() );
	commandManager.waitForInstance( command );

	command = commandManager.queueCommand( TestCompoundCommand::generateId( 4, CommandThreadAffinity::ANY_THREAD ).c_str() );
	commandManager.waitForInstance( command );
}

TEST_F( TestCommandFixture, alternatingCompoundCommands )
{
	// This test attempts to verify commands do not deadlock.
	// TODO: waitForInstance need to take a timeout to properly handle when this test fails
	auto & commandManager = getCommandSystemProvider();

	auto command = commandManager.queueCommand( TestAlternatingCompoundCommand::generateId( 1, CommandThreadAffinity::UI_THREAD ).c_str() );
	commandManager.waitForInstance( command );

	command = commandManager.queueCommand( TestAlternatingCompoundCommand::generateId( 1, CommandThreadAffinity::COMMAND_THREAD ).c_str() );
	commandManager.waitForInstance( command );

	command = commandManager.queueCommand( TestAlternatingCompoundCommand::generateId( 1, CommandThreadAffinity::ANY_THREAD ).c_str() );
	commandManager.waitForInstance( command );

	command = commandManager.queueCommand( TestAlternatingCompoundCommand::generateId( 4, CommandThreadAffinity::UI_THREAD ).c_str() );
	commandManager.waitForInstance( command );

	command = commandManager.queueCommand( TestAlternatingCompoundCommand::generateId( 4, CommandThreadAffinity::COMMAND_THREAD ).c_str() );
	commandManager.waitForInstance( command );

	command = commandManager.queueCommand( TestAlternatingCompoundCommand::generateId( 4, CommandThreadAffinity::ANY_THREAD ).c_str() );
	commandManager.waitForInstance( command );
}
} // end namespace wgt
