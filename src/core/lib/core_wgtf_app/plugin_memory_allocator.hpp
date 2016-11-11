#ifndef PLUGIN_MEMORY_ALLOCATOR_HPP
#define PLUGIN_MEMORY_ALLOCATOR_HPP

#include "core_generic_plugin/interfaces/i_memory_allocator.hpp"
#include "core_dependency_system/i_interface.hpp"

namespace wgt
{
class PluginMemoryAllocator : public Implements<IMemoryAllocator>
{
public:
	PluginMemoryAllocator(const wchar_t* name);
	~PluginMemoryAllocator();

	void fini() override;
	void* mem_new(size_t size);
	void* mem_new(size_t size, const std::nothrow_t& throwable);
	void* mem_new_array(size_t size);
	void* mem_new_array(size_t size, const std::nothrow_t& throwable);
	void mem_delete(void* ptr);
	void mem_delete(void* ptr, const std::nothrow_t& throwable);
	void mem_delete_array(void* ptr);
	void mem_delete_array(void* ptr, const std::nothrow_t& throwable);

private:
	void* memoryContext_;
};
} // end namespace wgt
#endif // PLUGIN_MEMORY_ALLOCATOR_HPP
