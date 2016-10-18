#ifndef NGT_ALLOCATOR_HPP
#define NGT_ALLOCATOR_HPP

#include "wg_memory_dll.hpp"

namespace wgt
{
namespace NGTAllocator
{
	WG_MEMORY_DLL void * allocate( size_t size );
	WG_MEMORY_DLL void deallocate( void* ptr );
	WG_MEMORY_DLL void * createMemoryContext( const wchar_t * name );
	WG_MEMORY_DLL void destroyMemoryContext( void * );
	WG_MEMORY_DLL void pushMemoryContext( void * );
	WG_MEMORY_DLL void popMemoryContext();
	WG_MEMORY_DLL void cleanupContext( void * );

	WG_MEMORY_DLL void enableDebugOutput( bool enable );
	WG_MEMORY_DLL void enableStackTraces( bool enable );
    WG_MEMORY_DLL void enableLeakDetection(bool enable);
}
} // end namespace wgt
#endif // NGT_ALLOCATOR_HPP
