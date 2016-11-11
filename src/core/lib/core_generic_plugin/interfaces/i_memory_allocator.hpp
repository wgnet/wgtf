#ifndef I_MEMORY_ALLOCATOR_HPP
#define I_MEMORY_ALLOCATOR_HPP

#include <new>

namespace wgt
{
class IMemoryAllocator
{
public:
	virtual ~IMemoryAllocator(){};
	virtual void fini(){};
	virtual void* mem_new(size_t size) = 0;
	virtual void* mem_new(size_t size, const std::nothrow_t& throwable) = 0;
	virtual void* mem_new_array(size_t size) = 0;
	virtual void* mem_new_array(size_t size, const std::nothrow_t& throwable) = 0;
	virtual void mem_delete(void* ptr) = 0;
	virtual void mem_delete(void* ptr, const std::nothrow_t& throwable) = 0;
	virtual void mem_delete_array(void* ptr) = 0;
	virtual void mem_delete_array(void* ptr, const std::nothrow_t& throwable) = 0;
};
} // end namespace wgt
#endif // I_MEMORY_ALLOCATOR_HPP
