#ifndef BW_MEMORY_OPERATIONS_HPP
#define BW_MEMORY_OPERATIONS_HPP

#include "allocator.hpp"
#include <cstddef>
#include <new>

#ifdef __APPLE__
#define NOEXCEPT noexcept
#else
#define NOEXCEPT
#endif // __APPLE__

#ifdef NGT_ALLOCATOR

#ifdef HAVE_CUSTOM_ALLOCATOR
extern void* InitMemoryOverrides(size_t);
wgt::NGTAllocator::allocateFn s_customAllocFunc = InitMemoryOverrides;
wgt::NGTAllocator::deallocateFn s_customDeallocFunc = wgt::NGTAllocator::deallocate;
#endif

void* operator new(std::size_t size)
{
#ifdef HAVE_CUSTOM_ALLOCATOR
	return s_customAllocFunc(size);
#else
	return wgt::NGTAllocator::allocate(size);
#endif
}

void* operator new(std::size_t size, const std::nothrow_t&) NOEXCEPT
{
#ifdef HAVE_CUSTOM_ALLOCATOR
	return s_customAllocFunc(size);
#else
	return wgt::NGTAllocator::allocate(size);
#endif
}

void* operator new[](std::size_t size)
{
#ifdef HAVE_CUSTOM_ALLOCATOR
	return s_customAllocFunc(size);
#else
	return wgt::NGTAllocator::allocate(size);
#endif
}

void* operator new[](std::size_t size, const std::nothrow_t& throwable) NOEXCEPT
{
#ifdef HAVE_CUSTOM_ALLOCATOR
	return s_customAllocFunc(size);
#else
	return wgt::NGTAllocator::allocate(size);
#endif
}

void operator delete(void* ptr)NOEXCEPT
{
#ifdef HAVE_CUSTOM_ALLOCATOR
	s_customDeallocFunc(ptr);
#else
	return wgt::NGTAllocator::deallocate(ptr);
#endif
}

void operator delete(void* ptr, const std::nothrow_t& throwable)NOEXCEPT
{
#ifdef HAVE_CUSTOM_ALLOCATOR
	s_customDeallocFunc(ptr);
#else
	return wgt::NGTAllocator::deallocate(ptr);
#endif
}

void operator delete[](void* ptr) NOEXCEPT
{
#ifdef HAVE_CUSTOM_ALLOCATOR
	s_customDeallocFunc(ptr);
#else
	return wgt::NGTAllocator::deallocate(ptr);
#endif
}

void operator delete[](void* ptr, const std::nothrow_t& throwable) NOEXCEPT
{
#ifdef HAVE_CUSTOM_ALLOCATOR
	s_customDeallocFunc(ptr);
#else
	return wgt::NGTAllocator::deallocate(ptr);
#endif
}

#endif // NGT_ALLOCATOR
#endif // BW_MEMORY_OPERATIONS_HPP
