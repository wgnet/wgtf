#include "core_generic_plugin/generic_plugin.hpp"

#include "../interfaces_test/test_interface.hpp"
#include "core_common/ngt_windows.hpp"

namespace wgt
{
class TestClassA : public Implements<INTERFACE_VERSION(TestInterface, 0, 0)> // TestInterface v0.0
{
public:
	TestClassA(int value) : value_(value)
	{
	}

	void test()
	{
		char buffer[256];
		sprintf(buffer, "%s - %d\n", typeid(*this).name(), value_);
		::OutputDebugStringA(buffer);
	}
	virtual ~TestClassA()
	{
	}

private:
	int value_;
};

class TestClassA_0_1 : public Implements<INTERFACE_VERSION(TestInterface, 0, 1)> // TestInterface v0.1
{
public:
	TestClassA_0_1(int value) : value_(value)
	{
	}
	virtual ~TestClassA_0_1()
	{
	}
	void test()
	{
		char buffer[256];
		sprintf(buffer, "%s - %d\n", typeid(*this).name(), value_);
		::OutputDebugStringA(buffer);
	}

	virtual void test2()
	{
		char buffer[256];
		sprintf(buffer, "%s::test2() - %d\n", typeid(*this).name(), value_);
		::OutputDebugStringA(buffer);
	}

private:
	int value_;
};

class TestClassC : public Implements<INTERFACE_VERSION(TestInterface, 1, 0)> // TestInterface v1.0
{
public:
	TestClassC()
	{
	}
	virtual ~TestClassC()
	{
	}
	void test(int value)
	{
		char buffer[256];
		sprintf(buffer, "%s - %d\n", typeid(*this).name(), value);
		::OutputDebugStringA(buffer);
	}
};

/**
* A plugin which tests using multiple registered interfaces of the same type
* (including major and minor versions) by outputting test data to the debug window
*
* @ingroup plugins
* @note Requires Plugins:
*       - FileSystemPlugin
*       - ReflectionPlugin
*       - CommandSystemPlugin
*       - MainTestPlugin
*/
class TestPlugin3 : public PluginMain
{
public:
	//==========================================================================
	TestPlugin3(IComponentContext& contextManager)
	{
	}

	//==========================================================================
	bool PostLoad(IComponentContext& contextManager)
	{
		contextManager.registerInterface(new TestClassA(10));
		contextManager.registerInterface(new TestClassA(20));
		contextManager.registerInterface(new TestClassA(30));
		contextManager.registerInterface(new TestClassA_0_1(40));
		contextManager.registerInterface(new TestClassC());
		return true;
	}

	//==========================================================================
	void Initialise(IComponentContext& contextManager)
	{
	}
};

PLG_CALLBACK_FUNC(TestPlugin3)
} // end namespace wgt
