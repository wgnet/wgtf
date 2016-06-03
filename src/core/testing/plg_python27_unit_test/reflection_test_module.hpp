#ifndef PYTHON_REFLECTION_MODULE_HPP
#define PYTHON_REFLECTION_MODULE_HPP


class TestResult;

namespace wgt
{
class IComponentContext;

class ReflectionTestModule
{
public:


	/**
	 *	Register this module with Python.
	 */
	ReflectionTestModule( IComponentContext & context,
		const char * testName,
		TestResult & result );


	/**
	 *	Module cannot be de-registered with Python.
	 *	Only destroy on shutdown.
	 */
	~ReflectionTestModule();


	/// For use by module after it's registered
	IComponentContext & context_;
	const char * testName_;
	TestResult & result_;
};
} // end namespace wgt
#endif // PYTHON_REFLECTION_MODULE_HPP
