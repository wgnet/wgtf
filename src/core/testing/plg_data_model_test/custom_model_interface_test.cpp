#include "custom_model_interface_test.hpp"

#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_logging/logging.hpp"
#include "core_reflection/reflected_object.hpp"
#include "core_reflection/reflection_macros.hpp"
#include "core_reflection/metadata/meta_types.hpp"
#include "core_reflection/function_property.hpp"
#include "core_reflection/utilities/reflection_function_utilities.hpp"
#include "core_qt_common/i_qt_framework.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/i_view.hpp"
#include "core_ui_framework//interfaces/i_view_creator.hpp"
#include "core_data_model/file_system/file_system_model.hpp"
#include "core_serialization/i_file_system.hpp"

namespace wgt
{
class ICustomModelInterface
{
	DECLARE_REFLECTED

public:
	ICustomModelInterface()
	{

	}

protected:
	ICustomModelInterface( int numeric, std::string string )
		: numeric_( numeric )
		, string_( string )
	{

	}

	void incrementNumeric( double value )
	{
		numeric_ += static_cast< int >( value );
	}

	void undoIncrementNumeric( const ObjectHandle& params, Variant result )
	{
		double value = (*params.getBase<ReflectedMethodParameters>())[0].cast<double>();
		numeric_ -= static_cast< int >( value );
	}

	void redoIncrementNumeric( const ObjectHandle& params, Variant result )
	{
		double value = (*params.getBase<ReflectedMethodParameters>())[0].cast<double>();
		numeric_ += static_cast< int >( value );
	}

private:
	int numeric_;
	std::string string_;
};

class CustomModelImplementation1 : public ICustomModelInterface
{
public:
	CustomModelImplementation1()
		: ICustomModelInterface( 1, "Implementation 1" )
	{

	}
};

class CustomModelImplementation2 : public ICustomModelInterface
{
public:
	CustomModelImplementation2()
		: ICustomModelInterface( 2, "Implementation 2" )
	{

	}
};

class CustomModelImplementation3 : public ICustomModelInterface
{
public:
	CustomModelImplementation3()
		: ICustomModelInterface( 3, "Implementation 3" )
	{

	}
};

class TestFixture
{
	DECLARE_REFLECTED

public:
	void init( IDefinitionManager * defManager, IFileSystem * fileSystem )
	{
		auto def = defManager->getDefinition< ICustomModelInterface >();
		implementation1_ = std::unique_ptr< ICustomModelInterface >(
			new CustomModelImplementation1 );
		implementation2_ = std::unique_ptr< ICustomModelInterface >(
			new CustomModelImplementation2 );
		implementation3_ = std::unique_ptr< ICustomModelInterface >(
			new CustomModelImplementation3 );
		fileSystemModel_ = std::unique_ptr< AbstractTreeModel >(
			new FileSystemModel( *fileSystem, "c:/" ) );
	}

	ICustomModelInterface * implementation1() const
	{
		return implementation1_.get();
	}

	ICustomModelInterface * implementation2() const
	{
		return implementation2_.get();
	}

	ICustomModelInterface * implementation3() const
	{
		return implementation3_.get();
	}

	AbstractTreeModel * fileSystemModel() const
	{
		return fileSystemModel_.get();
	}

private:
	std::unique_ptr< ICustomModelInterface > implementation1_;
	std::unique_ptr< ICustomModelInterface > implementation2_;
	std::unique_ptr< ICustomModelInterface > implementation3_;
	std::unique_ptr< AbstractTreeModel > fileSystemModel_;
};


BEGIN_EXPOSE( ICustomModelInterface, MetaNone() )
	EXPOSE( "numeric", numeric_, MetaNone() )
	EXPOSE( "string", string_, MetaNone() )
	EXPOSE_METHOD( "incrementNumeric", incrementNumeric, undoIncrementNumeric, redoIncrementNumeric )
END_EXPOSE()

BEGIN_EXPOSE( TestFixture, MetaNone() )
	EXPOSE( "Implementation1", implementation1, MetaNone() )
	EXPOSE( "Implementation2", implementation2, MetaNone() )
	EXPOSE( "Implementation3", implementation3, MetaNone() )
	EXPOSE( "fileSystemModel", fileSystemModel, MetaNone() )
END_EXPOSE()

CustomModelInterfaceTest::CustomModelInterfaceTest(IComponentContext & context )
	: Depends( context )
{

}

CustomModelInterfaceTest::~CustomModelInterfaceTest()
{

}

void CustomModelInterfaceTest::initialise( IComponentContext & contextManager )
{
	auto defManager = contextManager.queryInterface< IDefinitionManager >();
	auto fileSystem = contextManager.queryInterface< IFileSystem >();
	if (defManager == nullptr || fileSystem == nullptr)
	{
		return;
	}

	defManager->registerDefinition< TypeClassDefinition< ICustomModelInterface > >();
	defManager->registerDefinition< TypeClassDefinition< TestFixture > >();

	auto testFixture = defManager->create< 
		TestFixture >();
	testFixture->init( defManager, fileSystem );

	auto viewCreator = get< IViewCreator >();
	if (viewCreator)
	{
		viewCreator->createView(
			"plg_data_model_test/custom_model_interface_test_panel.qml",
			testFixture, testView_ );
	}
}

void CustomModelInterfaceTest::fini( IComponentContext & contextManager )
{
	auto uiApplication = contextManager.queryInterface< IUIApplication >();
	if (uiApplication == nullptr)
	{
		return;
	}

	if (testView_ != nullptr)
	{
		uiApplication->removeView( *testView_ );
		testView_.reset();
	}
}
} // end namespace wgt
