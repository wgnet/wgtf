#include "core_generic_plugin/generic_plugin.hpp"

#include "../interfaces_test/test_interface.hpp"
#include "../interfaces_test/auto_populate.hpp"
#include "core_dependency_system/di_ref.hpp"

namespace wgt
{
class TestClassF
	: public Implements< AutoPopulate >
{
public:
	TestClassF( IComponentContext & contextManager )
		: testClassA_( contextManager )
		, testClassB_( contextManager )
	{
	}
	virtual ~TestClassF() {}

	InterfaceA * getInterfaceA() { return testClassA_.get(); }
	std::vector< InterfaceB * > getInterfaceBs()
	{
		std::vector< InterfaceB * > returnValue;
		testClassB_.get( returnValue );
		return returnValue;
	}

private:
	//Auto populated references
	DIRef< InterfaceA > testClassA_;
	DIRef< InterfaceB > testClassB_;
};

//==============================================================================
class TestPlugin4
	: public PluginMain
{
public:
	//==========================================================================
	TestPlugin4( IComponentContext & contextManager ) {}

	//==========================================================================
	bool PostLoad( IComponentContext & contextManager )
	{
		contextManager.registerInterface( new TestClassF( contextManager ) );
		return true;
	}
};

PLG_CALLBACK_FUNC( TestPlugin4 )
} // end namespace wgt
