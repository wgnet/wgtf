#include "core_generic_plugin/generic_plugin.hpp"

#include "../interfaces_test/test_interface.hpp"
#include "core_common/ngt_windows.hpp"

namespace wgt
{
class TestClassB : public Implements<TestInterface> // Always implement latest version
{
public:
	TestClassB(float value) : value_(value)
	{
	}
	virtual ~TestClassB()
	{
	}
	void test(int value)
	{
		char buffer[256];
		sprintf(buffer, "%s - %f - %d\n", typeid(*this).name(), value_, value);
		::OutputDebugStringA(buffer);
	}

private:
	float value_;
};

/**
* A plugin which tests using multiple registered interfaces of
* the same type by outputting test data to the debug window
*
* @ingroup plugins
* @note Requires Plugins:
*       - FileSystemPlugin
*       - ReflectionPlugin
*       - CommandSystemPlugin
*       - MainTestPlugin
*/
class TestPlugin2 : public PluginMain
{
public:
	//==========================================================================
	TestPlugin2(IComponentContext& contextManager)
	{
	}

	//==========================================================================
	bool PostLoad(IComponentContext& contextManager)
	{
		contextManager.registerInterface(new TestClassB(0.5f));
		contextManager.registerInterface(new TestClassB(2.5f));
		return true;
	}

	//==========================================================================
	void Initialise(IComponentContext& contextManager)
	{
	}
};

PLG_CALLBACK_FUNC(TestPlugin2)
} // end namespace wgt
