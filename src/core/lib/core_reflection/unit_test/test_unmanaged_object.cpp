#include "pch.hpp"
#include "core_unit_test/unit_test.hpp"
#include <chrono>
#include <thread>

#include "core_unit_test/test_application.hpp"

#include "core_reflection/definition_manager.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_reflection/object_manager.hpp"
#include "core_reflection/reflected_object.hpp"
#include "core_reflection/reflection_macros.hpp"
#include "core_reflection/function_property.hpp"
#include "core_reflection/utilities/reflection_function_utilities.hpp"
#include "core_reflection/metadata/meta_types.hpp"
#include "core_reflection/reflected_types.hpp"

#include "core_reflection_utils/reflection_controller.hpp"
#include "core_reflection_utils/commands/set_reflectedproperty_command.hpp"
#include "core_reflection_utils/commands/metadata/set_reflectedproperty_command.mpp"
#include "core_reflection_utils/serializer/reflection_serializer.hpp"

#include "core_data_model/reflection/reflected_list.hpp"
#include "core_data_model/i_item_role.hpp"

#include "core_command_system/command_system.hpp"
#include "core_command_system/command_manager.hpp"
#include "core_command_system/env_system.hpp"

#include "core_serialization/serializer/serialization_manager.hpp"


namespace wgt
{
class TestObjectHandleFixture
{
public:
	TestApplication application_;
	ObjectManager objManager;
	DefinitionManager defManager;
	EnvManager envManager_;
	CommandManager commandManager;
	ReflectionController reflectionController;
	SetReflectedPropertyCommand setReflectedPropertyCmd;

	TestObjectHandleFixture()
		: defManager( objManager )
		, commandManager( defManager )
		, setReflectedPropertyCmd( defManager )
	{
		objManager.init( &defManager );

		Reflection::initReflectedTypes( defManager );
		CommandSystem::initReflectedTypes( defManager );
		IDefinitionManager & definitionManager = defManager;
		REGISTER_DEFINITION( ReflectedPropertyCommandArgument );

		commandManager.init( application_, envManager_, nullptr, nullptr );
		commandManager.registerCommand( &setReflectedPropertyCmd );
		reflectionController.init( commandManager );
	}

	~TestObjectHandleFixture()
	{
		commandManager.fini();
	}
};

class Test1
{
	DECLARE_REFLECTED
public:
	Test1() : value_(0) {}
	Test1(int v) : value_(v) {}

	int value_;
};

BEGIN_EXPOSE( Test1, MetaNone() )
	EXPOSE( "Value", value_, MetaNone() )
END_EXPOSE()

class Test2
{
	DECLARE_REFLECTED
public:
	Test2() : value_(0) {}
	Test2(int v) : value_(v), test1_(v) {}

	int value_;
	Test1 test1_;
};

BEGIN_EXPOSE( Test2, MetaNone() )
	EXPOSE( "Value", value_, MetaNone() )
	EXPOSE( "Test1", test1_, MetaNone() )
END_EXPOSE()

class Test3
{
	DECLARE_REFLECTED
public:
	Test3() : value_(0) {}
	Test3(int v) : value_(v), test2_(v), vector_(1, Test1( v ) ) {}

	int value_;
	Test2 test2_;
	std::vector< ObjectHandleT< Test1 > > vector_;
};

BEGIN_EXPOSE( Test3, MetaNone() )
	EXPOSE( "Value", value_, MetaNone() )
	EXPOSE( "Test2", test2_, MetaNone() )
	EXPOSE( "TestVec", vector_, MetaNone() )
END_EXPOSE()

TEST_F(TestObjectHandleFixture, unmanaged_object)
{
	IDefinitionManager& definitionManager = defManager;
	REGISTER_DEFINITION( Test1 );
	REGISTER_DEFINITION( Test2 );
	IClassDefinition* def3 = REGISTER_DEFINITION( Test3 );

	std::unique_ptr<Test3> test = std::unique_ptr<Test3>( new Test3(3) );
	ObjectHandle handle(test.get(), def3);

	CHECK_EQUAL(objManager.getObject(test.get()).isValid(), false);
	CHECK_EQUAL(objManager.getUnmanagedObject(test.get()).isValid(), false);

	reflectionController.setValue( def3->bindProperty("Value", handle), Variant(5) );

	CHECK_EQUAL(objManager.getObject(test.get()).isValid(), false);
	CHECK_EQUAL(objManager.getUnmanagedObject(test.get()).getBase<Test3>(), test.get());

	reflectionController.setValue( def3->bindProperty("Test2.Value", handle), Variant(7) );
	reflectionController.setValue( def3->bindProperty("Test2.Test1.Value", handle), Variant(11) );
	reflectionController.setValue( def3->bindProperty("TestVec[0].Value", handle), Variant(13) );

	// TODO: replace this call with reflectionController.flush
	reflectionController.getValue( def3->bindProperty("TestVec[0].Value", handle) );

	CHECK_EQUAL(5, test->value_);
	CHECK_EQUAL(7, test->test2_.value_);
	CHECK_EQUAL(11, test->test2_.test1_.value_);
	CHECK_EQUAL(13, test->vector_[0]->value_);
}

class Test1Stack
{
	DECLARE_REFLECTED
public:
	Test1Stack() : value_(0) {}
	Test1Stack(int v) : value_(v) {}

private:
	int value_;
};

BEGIN_EXPOSE( Test1Stack, MetaOnStack() )
	EXPOSE( "Value", value_, MetaNone() )
END_EXPOSE()

class Test2Stack
{
	DECLARE_REFLECTED
public:
	Test2Stack() : value_(0) {}
	Test2Stack(int v) : value_(v), test1_(v) {}

private:
	int value_;
	Test1 test1_;
};

BEGIN_EXPOSE( Test2Stack, MetaOnStack() )
	EXPOSE( "Value", value_, MetaNone() )
	EXPOSE( "Test1", test1_, MetaNone() )
END_EXPOSE()

class GListTest
{
public:
	GListTest( IDefinitionManager* defManager ) : gl_(defManager) {}
	GListTest( const GListTest& ) : gl_(nullptr) { assert(false); }

	template <typename T>
	void addItem( T& t ) { gl_.emplace_back( ObjectHandle( t ) ); }

	const IListModel * getList() const { return &gl_; }

	PropertyAccessor bindProperty( size_t index, IClassDefinition* def, const char* name )
	{
		return def->bindProperty( name, gl_[index].cast<ObjectHandle>() );
	}

private:
	ReflectedList gl_;
};

TEST_F(TestObjectHandleFixture, on_stack_object)
{
	IDefinitionManager& definitionManager = defManager;
	REGISTER_DEFINITION( Test1 );
	IClassDefinition* def1 = REGISTER_DEFINITION( Test1Stack );
	IClassDefinition* def2 = REGISTER_DEFINITION( Test2Stack );

	std::unique_ptr<GListTest> glist = std::unique_ptr<GListTest>( new GListTest(&definitionManager) );
	Test1Stack testItem1( 5 );
	Test2Stack testItem2( 58 );
	Test1Stack testItem3( 7 );
	glist->addItem( testItem1 );
	glist->addItem( testItem2 );
	glist->addItem( testItem3 );

	reflectionController.setValue(
		glist->bindProperty(0u, def1, "Value"), Variant(13) );
	reflectionController.setValue(
		glist->bindProperty(1u, def2, "Test1.Value"), Variant(17) );
	reflectionController.setValue(
		glist->bindProperty(2u, def1, "Value"), Variant(19) );

	// TODO: replace this call with reflectionController.flush
	reflectionController.getValue( glist->bindProperty(2u, def1, "Value") );

	CHECK_EQUAL(glist->bindProperty(0u, def1, "Value").getValue(), 13);
	CHECK_EQUAL(glist->bindProperty(1u, def2, "Test1.Value").getValue(), 17);
	CHECK_EQUAL(glist->bindProperty(2u, def1, "Value").getValue(), 19);
}
} // end namespace wgt
