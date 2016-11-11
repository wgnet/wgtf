#include "core_generic_plugin/interfaces/i_application.hpp"
#include "core_generic_plugin/generic_plugin.hpp"
#include "core_dependency_system/i_interface.hpp"

namespace wgt
{
class A;

void fcn(A*);

class A
{
public:
	virtual void f() = 0;
	A()
	{
		fcn(this);
	}
};

class B : A
{
	void f()
	{
	}
};

void fcn(A* p)
{
	p->f();
}

class RunTimeErrorTestPluginApplication : public Implements<IApplication>
{
public:
	int startApplication() override
	{
		// The declaration below invokes class B's constructor, which
		// first calls class A's constructor, which calls fcn. Then
		// fcn calls A::f, which is a pure virtual function, and
		// this causes the run-time error. B has not been constructed
		// at this point, so the B::f cannot be called. You would not
		// want it to be called because it could depend on something
		// in B that has not been initialized yet.
		B b;
		return 0;
	}

	void quitApplication() override
	{
	}
};

/**
 *	A plugin which does a run-time error on purpose to check if it is detected
 *	by the automated testing framework.
 *
 *	Test case is from MSDN:
 *	https://support.microsoft.com/en-us/kb/125749
 *
 * @ingroup plugins
 */
class RunTimeErrorTestPluginTestPlugin : public PluginMain
{
public:
	RunTimeErrorTestPluginTestPlugin(IComponentContext& contextManager)
	{
	}

	bool PostLoad(IComponentContext& contextManager) override
	{
		contextManager.registerInterface(new RunTimeErrorTestPluginApplication());
		return true;
	}
};

PLG_CALLBACK_FUNC(RunTimeErrorTestPluginTestPlugin)

} // end namespace wgt
