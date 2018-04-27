#include "wg_memory/memory_overrides.hpp"

#ifdef HAVE_CUSTOM_ALLOCATOR
void* InitMemoryOverrides(size_t size)
{
	return wgt::NGTAllocator::allocate(size);
}
#endif