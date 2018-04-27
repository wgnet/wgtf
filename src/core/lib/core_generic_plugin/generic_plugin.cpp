#include "generic_plugin.hpp"

#include "core_common/assert.hpp"
#include "core_common/platform_env.hpp"
#include "core_common/platform_std.hpp"
#include "core_common/shared_library.hpp"
#include "env_context.hpp"
#include "interfaces/i_memory_allocator.hpp"
#include "common_include/i_static_initializer.hpp"

#include <cstdint>
#include <memory>

#include "core_wgtf_app/app_common.cpp"

namespace wgt
{
namespace AppCommonPrivate
{
void staticInitPlugin()
{
	auto delegate = getPluginInitDelegate();
	TF_ASSERT(delegate != nullptr);
	(*delegate)([](IComponentContext& context) { s_Context = &context; });
	s_StaticInitPluginFunc = nullptr;
    }
}

namespace
{
//==============================================================================
class DefaultMemoryAllocator : public IMemoryAllocator
{
	virtual void* mem_new(size_t size) override
	{
		return malloc(size);
	}

	virtual void* mem_new(size_t size, const std::nothrow_t& throwable) override
	{
		return malloc(size);
	}

	virtual void* mem_new_array(size_t size) override
	{
		return malloc(size);
	}

	virtual void* mem_new_array(size_t size, const std::nothrow_t& throwable) override
	{
		return malloc(size);
	}

	virtual void mem_delete(void* ptr) override
	{
		free(ptr);
	}

	virtual void mem_delete(void* ptr, const std::nothrow_t& throwable) override
	{
		free(ptr);
	}

	virtual void mem_delete_array(void* ptr) override
	{
		free(ptr);
	}

	virtual void mem_delete_array(void* ptr, const std::nothrow_t& throwable) override
	{
		free(ptr);
	}
};

IMemoryAllocator* getDefaultAllocator()
{
	static DefaultMemoryAllocator s_defaultAllocator;
	return &s_defaultAllocator;
}

IMemoryAllocator* getMemoryAllocator()
{
	static IMemoryAllocator* s_allocator = nullptr;
	if (s_allocator == nullptr)
	{
		IComponentContext* context = s_GetContextFunc();
		if (context != nullptr)
		{
			s_allocator = context->queryInterface<IMemoryAllocator>();
		}
		if (s_allocator == nullptr)
		{
			s_allocator = getDefaultAllocator();
			return s_allocator;
		}
	}
	return s_allocator;
}

}

//==============================================================================
PluginMain::PluginMain() : name_(nullptr)
{
}

//==============================================================================
void PluginMain::init(const char* name)
{
	name_ = name;
}

} // end namespace wgt

namespace wgt
{
#ifdef NGT_ALLOCATOR
} // end namespace wgt
//==============================================================================
void* operator new(std::size_t size)
{
	wgt::IMemoryAllocator* memAlloc = wgt::getMemoryAllocator();
	if (memAlloc == nullptr)
	{
		return malloc(size);
	}
	return memAlloc->mem_new(size);
}

//==============================================================================
void* operator new(std::size_t size, const std::nothrow_t& throwable) NOEXCEPT
{
	wgt::IMemoryAllocator* memAlloc = wgt::getMemoryAllocator();
	if (memAlloc == nullptr)
	{
		return malloc(size);
	}
	return memAlloc->mem_new(size, throwable);
}

//==============================================================================
void* operator new[](std::size_t size)
{
	wgt::IMemoryAllocator* memAlloc = wgt::getMemoryAllocator();
	if (memAlloc == nullptr)
	{
		return malloc(size);
	}
	return wgt::getMemoryAllocator()->mem_new_array(size);
}

//==============================================================================
void* operator new[](std::size_t size, const std::nothrow_t& throwable) NOEXCEPT
{
	wgt::IMemoryAllocator* memAlloc = wgt::getMemoryAllocator();
	if (memAlloc == nullptr)
	{
		return malloc(size);
	}
	return memAlloc->mem_new_array(size, throwable);
}

//==============================================================================
void operator delete(void* ptr)NOEXCEPT
{
	wgt::IMemoryAllocator* memAlloc = wgt::getMemoryAllocator();
	if (memAlloc == nullptr)
	{
		free(ptr);
		return;
	}
	memAlloc->mem_delete(ptr);
}

//==============================================================================
void operator delete(void* ptr, const std::nothrow_t& throwable)NOEXCEPT
{
	wgt::IMemoryAllocator* memAlloc = wgt::getMemoryAllocator();
	if (memAlloc == nullptr)
	{
		free(ptr);
		return;
	}
	memAlloc->mem_delete(ptr, throwable);
}

//==============================================================================
void operator delete[](void* ptr) NOEXCEPT
{
	wgt::IMemoryAllocator* memAlloc = wgt::getMemoryAllocator();
	if (memAlloc == nullptr)
	{
		free(ptr);
		return;
	}
	memAlloc->mem_delete_array(ptr);
}

//==============================================================================
void operator delete[](void* ptr, const std::nothrow_t& throwable) NOEXCEPT
{
	wgt::IMemoryAllocator* memAlloc = wgt::getMemoryAllocator();
	if (memAlloc == nullptr)
	{
		free(ptr);
		return;
	}
	memAlloc->mem_delete_array(ptr, throwable);
}

namespace wgt
{
#endif // NGT_ALLOCATOR

PluginMain* createPlugin(IComponentContext& contextManager);

//==============================================================================
EXPORT bool __cdecl PLG_CALLBACK(GenericPluginLoadState loadState)
{
	static PluginMain* s_pluginMain = nullptr;
	auto contextManager = s_GetContextFunc();
	TF_ASSERT(contextManager);
	auto staticInitializer = contextManager->queryInterface<IStaticInitalizer>();
	if (staticInitializer)
	{
		staticInitializer->initStatics(*contextManager);
	}
	switch (loadState)
	{
	case GenericPluginLoadState::Create:
		s_pluginMain = createPlugin(*contextManager);
		return true;

	case GenericPluginLoadState::PostLoad:
		return s_pluginMain->PostLoad(*contextManager);

	case GenericPluginLoadState::Initialise:
		s_pluginMain->Initialise(*contextManager);
		return true;

	case GenericPluginLoadState::Finalise:
		return s_pluginMain->Finalise(*contextManager);

	case GenericPluginLoadState::Unload:
		s_pluginMain->Unload(*contextManager);
		return true;

	case GenericPluginLoadState::Destroy:
		if (staticInitializer)
		{
			staticInitializer->destroy();
		}
		delete s_pluginMain;
		s_pluginMain = nullptr;
		return true;

	default:
		break;
	}
	return false;
}
} // end namespace wgt
