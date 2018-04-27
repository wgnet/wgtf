#include "object_manager_creator.hpp"

#include "context_object_manager.hpp"
#include "object_manager.hpp"

#include "core_generic_plugin/interfaces/i_component_context.hpp"

#include <cassert>

namespace wgt
{
class ObjectManagerCreator::Impl
{
public:
	Impl(IComponentContext& componentContext) : componentContext_(componentContext), interface_(nullptr)
	{
		interface_ = componentContext.registerInterface(&objectManager_, false);
	}

	~Impl()
	{
		componentContext_.deregisterInterface(interface_.get());
	}

	ObjectManagerTest objectManager_;

private:
	IComponentContext& componentContext_;
	InterfacePtr interface_;
};

ObjectManagerCreator::ObjectManagerCreator(IComponentContext& componentContext) : impl_(new Impl(componentContext))
{
}

ObjectManagerCreator::~ObjectManagerCreator()
{
}

const char* ObjectManagerCreator::getType() const
{
	return typeid(ContextObjectManagerTest).name();
}

InterfacePtr ObjectManagerCreator::createContext(const wchar_t* contextId)
{
	auto context = new ContextObjectManagerTest(impl_->objectManager_, contextId);
	return std::make_shared<InterfaceHolder<ContextObjectManagerTest>>(context, true);
}
} // end namespace wgt
