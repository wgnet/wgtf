#ifndef BW_MEMORY_OPERATIONS_HPP
#define BW_MEMORY_OPERATIONS_HPP

#include <cstddef>
#include <new>
#include "allocator.hpp"

#ifdef __APPLE__
#define NOEXCEPT noexcept
#else
#define NOEXCEPT
#endif // __APPLE__

#ifdef NGT_ALLOCATOR

void * operator new( std::size_t size )
{																			
	return wgt::NGTAllocator::allocate( size );
}																			

void * operator new ( std::size_t size, const std::nothrow_t& )	NOEXCEPT
{																			
	return wgt::NGTAllocator::allocate( size );
}																			

void * operator new[]( std::size_t size )									
{																			
	return wgt::NGTAllocator::allocate( size );
}																			

void * operator new[]( std::size_t size, const std::nothrow_t & throwable )	NOEXCEPT
{																			
	return wgt::NGTAllocator::allocate( size );
}																			

void operator delete( void* ptr ) NOEXCEPT
{																			
	wgt::NGTAllocator::deallocate( ptr );
}																			

void operator delete( void* ptr, const std::nothrow_t & throwable )	NOEXCEPT
{																			
	wgt::NGTAllocator::deallocate( ptr );
}																			

void operator delete[]( void* ptr ) NOEXCEPT
{																			
	wgt::NGTAllocator::deallocate( ptr );
}																			

void operator delete[]( void* ptr, const std::nothrow_t & throwable ) NOEXCEPT
{																			
	wgt::NGTAllocator::deallocate( ptr );
}

#endif //NGT_ALLOCATOR
#endif // BW_MEMORY_OPERATIONS_HPP
