#ifndef CUSTOM_MODEL_INTERFACE_TEST__HPP
#define CUSTOM_MODEL_INTERFACE_TEST__HPP

#include <memory>
#include "core_dependency_system/depends.hpp"

namespace wgt
{
class IComponentContext;
class IView;
class IViewCreator;

class CustomModelInterfaceTest
	: public Depends< IViewCreator >
{
public:
	CustomModelInterfaceTest(IComponentContext & );
	~CustomModelInterfaceTest();

	void initialise( IComponentContext & contextManager );
	void fini( IComponentContext & contextManager );

private:
	std::unique_ptr< IView > testView_;
};
} // end namespace wgt
#endif//CUSTOM_MODEL_INTERFACE_TEST__HPP
