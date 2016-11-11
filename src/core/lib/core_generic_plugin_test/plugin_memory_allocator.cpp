#include "plugin_memory_allocator.hpp"
#include "wg_memory/allocator.hpp"

namespace wgt
{
//==========================================================================
namespace
{
struct ScopedMemoryContext
{
	ScopedMemoryContext(void* memoryContext)
	{
		memoryContext_ = memoryContext;
		if (memoryContext_ == nullptr)
		{
			return;
		}
		NGTAllocator::pushMemoryContext(memoryContext_);
	}

	~ScopedMemoryContext()
	{
		if (memoryContext_ == nullptr)
		{
			return;
		}
		NGTAllocator::popMemoryContext();
	};

	void* memoryContext_;
};
}

//==============================================================================
PluginMemoryAllocator::PluginMemoryAllocator(const wchar_t* name)
    : memoryContext_(NGTAllocator::createMemoryContext(name))
{
}

//==============================================================================
PluginMemoryAllocator::~PluginMemoryAllocator()
{
	NGTAllocator::cleanupContext(memoryContext_);
	NGTAllocator::destroyMemoryContext(memoryContext_);
}

//==============================================================================
void* PluginMemoryAllocator::mem_new(size_t size)
{
	ScopedMemoryContext memoryContext(memoryContext_);
	return NGTAllocator::allocate(size);
}

//==============================================================================
void* PluginMemoryAllocator::mem_new(size_t size, const std::nothrow_t& throwable)
{
	ScopedMemoryContext memoryContext(memoryContext_);
	return NGTAllocator::allocate(size);
}

//==============================================================================
void* PluginMemoryAllocator::mem_new_array(size_t size)
{
	ScopedMemoryContext memoryContext(memoryContext_);
	return NGTAllocator::allocate(size);
}

//==============================================================================
void* PluginMemoryAllocator::mem_new_array(size_t size, const std::nothrow_t& throwable)
{
	ScopedMemoryContext memoryContext(memoryContext_);
	return NGTAllocator::allocate(size);
}

//==============================================================================
void PluginMemoryAllocator::mem_delete(void* ptr)
{
	ScopedMemoryContext memoryContext(memoryContext_);
	NGTAllocator::deallocate(ptr);
}

//==============================================================================
void PluginMemoryAllocator::mem_delete(void* ptr, const std::nothrow_t& throwable)
{
	ScopedMemoryContext memoryContext(memoryContext_);
	NGTAllocator::deallocate(ptr);
}

//==============================================================================
void PluginMemoryAllocator::mem_delete_array(void* ptr)
{
	ScopedMemoryContext memoryContext(memoryContext_);
	NGTAllocator::deallocate(ptr);
}

//==============================================================================
void PluginMemoryAllocator::mem_delete_array(void* ptr, const std::nothrow_t& throwable)
{
	ScopedMemoryContext memoryContext(memoryContext_);
	NGTAllocator::deallocate(ptr);
}
} // end namespace wgt
