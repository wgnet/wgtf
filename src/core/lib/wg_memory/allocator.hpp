#ifndef NGT_ALLOCATOR_HPP
#define NGT_ALLOCATOR_HPP

namespace wgt
{
namespace NGTAllocator
{
	void * allocate( size_t size );
	void deallocate( void* ptr );
	void * createMemoryContext( const wchar_t * name );
	void destroyMemoryContext( void * );
	void pushMemoryContext( void * );
	void popMemoryContext();
	void cleanupContext( void * );

	void enableDebugOutput( bool enable );
	void enableStackTraces( bool enable );
}
} // end namespace wgt
#endif // NGT_ALLOCATOR_HPP
