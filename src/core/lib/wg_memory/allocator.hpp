#ifndef NGT_ALLOCATOR_HPP
#define NGT_ALLOCATOR_HPP

#include "wg_memory_dll.hpp"
#include <cstddef>
#include <functional>

namespace wgt
{
namespace NGTAllocator
{
typedef void* (*allocateFn)(size_t);
typedef void (*deallocateFn)(void*);

WG_MEMORY_DLL void* allocate(size_t size);
WG_MEMORY_DLL void deallocate(void* ptr);
WG_MEMORY_DLL void* createMemoryContext(const wchar_t* name);
WG_MEMORY_DLL void destroyMemoryContext(void*);
WG_MEMORY_DLL void pushMemoryContext(void*);
WG_MEMORY_DLL void popMemoryContext();
WG_MEMORY_DLL void cleanupContext(void*);

WG_MEMORY_DLL void enableDebugOutput(bool enable);
WG_MEMORY_DLL void enableStackTraces(bool enable);
WG_MEMORY_DLL void enableLeakDetection(bool enable);
WG_MEMORY_DLL void enableLogging(bool enable);

typedef std::function<void(const char*)> PrintFn;
WG_MEMORY_DLL void printCallstack(size_t framesToSkip, PrintFn fn);

WG_MEMORY_DLL void setHandles(allocateFn allocator, deallocateFn deallocator, allocateFn untrackedAllocator,
                              deallocateFn untrackedDeallocator);
}
} // end namespace wgt
#endif // NGT_ALLOCATOR_HPP
