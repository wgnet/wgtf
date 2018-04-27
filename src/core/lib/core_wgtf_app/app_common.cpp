#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "interfaces/common_include/i_static_initializer.hpp"
#include "core_generic_plugin/interfaces/i_memory_allocator.hpp"
#include "core_common/platform_std.hpp"
#include <cassert>
#include <cstddef>

namespace wgt
{
// Do not call anything from this namespace directly!
// Call the bound function pointers instead.
namespace AppCommonPrivate
{
void staticInitPlugin();
IComponentContext* initContext();
} // end namespace AppCommonPrivate

typedef IComponentContext* (*GetContextFunc)();
static GetContextFunc s_GetContextFunc = AppCommonPrivate::initContext;

typedef void (*StaticInitPluginFunc)();
static StaticInitPluginFunc s_StaticInitPluginFunc = AppCommonPrivate::staticInitPlugin;

// Do not call anything from this namespace directly!
// Call the bound function pointers instead.
namespace AppCommonPrivate
{
static IComponentContext* s_Context = nullptr;

IComponentContext* getContext()
{
	return s_Context;
}

IComponentContext* initContext()
{
	if (s_StaticInitPluginFunc != nullptr)
	{
		s_StaticInitPluginFunc();
	}
	s_GetContextFunc = getContext;
	return s_GetContextFunc();
}

struct StaticInitHelper
{
	StaticInitHelper()
	{
		if (s_StaticInitPluginFunc != nullptr)
		{
			s_StaticInitPluginFunc();
		}
	}
};

StaticInitHelper s_Helper;
} // end namespace AppCommonPrivate

void registerStatic(
	const std::shared_ptr< IStaticInitalizer::ExecFunc > & execFunc,
	const std::shared_ptr< IStaticInitalizer::FiniFunc > & finiFunc )
{
	auto context = s_GetContextFunc();
	if (context)
	{
		static IStaticInitalizer* staticInitializer = context->queryInterface<IStaticInitalizer>();
		staticInitializer->registerStaticInitializer(execFunc, finiFunc );
		staticInitializer->initStatics(*context);
	}
}

} // end namespace wgt
