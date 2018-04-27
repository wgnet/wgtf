#include "depends.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "wgt_static_init.hpp"

namespace wgt
{
namespace
{
static IComponentContext* s_DependsContext = nullptr;

WGTStaticExecutor s_Initializer(
	[](IComponentContext& context) { s_DependsContext = &context; },
	[]() { s_DependsContext = nullptr; });
}

//------------------------------------------------------------------------------
DependsBase::DependsBase()
{
	auto context = getContext();
	if (context)
	{
		connections_ += context->registerListener(*this);
	}
}

//------------------------------------------------------------------------------
DependsBase::~DependsBase()
{
}

//------------------------------------------------------------------------------
bool DependsBase::hasContext()
{
	return s_DependsContext != nullptr;
}

//------------------------------------------------------------------------------
IComponentContext* DependsBase::getContext() const
{
	return s_DependsContext;
}

//------------------------------------------------------------------------------
DependsBase::operator IComponentContext&()
{
	return *getContext();
}

} // namespace wgt