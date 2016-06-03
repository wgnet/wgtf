#include "generic_plugin.hpp"
#include "env_context.hpp"
#include "interfaces/i_memory_allocator.hpp"
#include "core_common/shared_library.hpp"
#include "core_common/platform_env.hpp"
#include "core_common/platform_std.hpp"

#include <cstdint>

namespace wgt
{
namespace
{
	class DefaultMemoryAllocator : public IMemoryAllocator
	{
		virtual void * mem_new(size_t size) override
		{
			return malloc(size);
		}

		virtual void * mem_new(size_t size, const std::nothrow_t & throwable) override
		{
			return malloc(size);
		}

		virtual void * mem_new_array(size_t size) override
		{
			return malloc(size);
		}

		virtual void * mem_new_array(size_t size, const std::nothrow_t & throwable) override
		{
			return malloc(size);
		}

		virtual void mem_delete(void* ptr) override
		{
			free(ptr);
		}

		virtual void mem_delete(void* ptr, const std::nothrow_t & throwable) override
		{
			free(ptr);
		}

		virtual void mem_delete_array(void* ptr) override
		{
			free(ptr);
		}

		virtual void mem_delete_array(void* ptr, const std::nothrow_t & throwable) override
		{
			free(ptr);
		}

	} s_defaultAllocator;

	IMemoryAllocator * getMemoryAllocator()
	{
		static IMemoryAllocator * s_allocator = nullptr;
		if (s_allocator == nullptr)
		{
			IComponentContext * context = PluginMain::getContext();
			if (context != nullptr)
			{
				s_allocator = context->queryInterface< IMemoryAllocator >();
			}
			if (s_allocator == nullptr)
			{
				s_allocator = &s_defaultAllocator;
				return s_allocator;
			}
		}
		return s_allocator;
	}

	class ContextInitializer
	{
	public:
		ContextInitializer()
		{
			PluginMain::setContext( getEnvContext() );
		}
	};

	static ContextInitializer s_ContextInitializer;
}


//==============================================================================
PluginMain::PluginMain()
	: name_( NULL )
{
}


//==============================================================================
void PluginMain::init( const char * name )
{
	name_ = name;
}


//==============================================================================
namespace Context
{

	bool deregisterInterface( IInterface * pImpl )
	{
		IComponentContext * context = PluginMain::getContext();
		assert(context != nullptr );
		return context->deregisterInterface( pImpl );
	}

	void * queryInterface( const TypeId & name )
	{
		IComponentContext * context = PluginMain::getContext();
		assert(context != nullptr );
		return context->queryInterface( name );
	}

	void queryInterface( const TypeId & name, std::vector< void * > & o_Impls )
	{
		IComponentContext * context = PluginMain::getContext();
		assert(context != nullptr );
		return context->queryInterface( name, o_Impls );
	}

}/* Namespace context*/

#ifdef NGT_ALLOCATOR
} // end namespace wgt
//==============================================================================
void * operator new( std::size_t size )
{
	wgt::IMemoryAllocator * memAlloc = wgt::getMemoryAllocator();
	if (memAlloc == nullptr)
	{
		return malloc( size );
	}
	return memAlloc->mem_new( size );
}


//==============================================================================
void * operator new ( std::size_t size, const std::nothrow_t & throwable ) NOEXCEPT
{
	wgt::IMemoryAllocator * memAlloc = wgt::getMemoryAllocator();
	if (memAlloc == nullptr)
	{
		return malloc( size );
	}
	return memAlloc->mem_new( size, throwable );
}

//==============================================================================
void * operator new[]( std::size_t size )
{
	wgt::IMemoryAllocator * memAlloc = wgt::getMemoryAllocator();
	if (memAlloc == nullptr)
	{
		return malloc( size );
	}
	return wgt::getMemoryAllocator()->mem_new_array( size );
}

//==============================================================================
void * operator new[]( std::size_t size, const std::nothrow_t & throwable ) NOEXCEPT
{
	wgt::IMemoryAllocator * memAlloc = wgt::getMemoryAllocator();
	if (memAlloc == nullptr)
	{
		return malloc( size );
	}
	return memAlloc->mem_new_array( size, throwable );
}


//==============================================================================
void operator delete( void* ptr ) NOEXCEPT
{
	wgt::IMemoryAllocator * memAlloc = wgt::getMemoryAllocator();
	if (memAlloc == nullptr)
	{
		free( ptr );
		return;
	}
	memAlloc->mem_delete( ptr );
}


//==============================================================================
void operator delete( void* ptr, const std::nothrow_t & throwable ) NOEXCEPT
{
	wgt::IMemoryAllocator * memAlloc = wgt::getMemoryAllocator();
	if (memAlloc == nullptr)
	{
		free( ptr );
		return;
	}
	memAlloc->mem_delete( ptr, throwable );
}


//==============================================================================
void operator delete[]( void* ptr ) NOEXCEPT
{
	wgt::IMemoryAllocator * memAlloc = wgt::getMemoryAllocator();
	if (memAlloc == nullptr)
	{
		free( ptr );
		return;
	}
	memAlloc->mem_delete_array( ptr );
}

//==============================================================================
void operator delete[]( void* ptr, const std::nothrow_t & throwable ) NOEXCEPT
{
	wgt::IMemoryAllocator * memAlloc = wgt::getMemoryAllocator();
	if (memAlloc == nullptr)
	{
		free( ptr );
		return;
	}
	memAlloc->mem_delete_array( ptr, throwable );
}
namespace wgt
{
#endif // NGT_ALLOCATOR

PluginMain * createPlugin( IComponentContext & contextManager );

IComponentContext * PluginMain::s_Context_ = nullptr;
bool PluginMain::s_ContextInitialized_ = false;

void PluginMain::setContext( IComponentContext * context )
{
	// Context may be set more than once
	// By ContextInitializer and PluginMain::getContext()
	// Assert that it is always set to the same context
	assert( (s_Context_ == nullptr) || (s_Context_ == context) );
	s_Context_ = context;
	s_ContextInitialized_ = true;
}

IComponentContext * PluginMain::getContext()
{
	if (!s_ContextInitialized_)
	{
		// This can happen if a static variable is initialized before
		// s_ContextInitializer and it tries to access the context.
		PluginMain::setContext( getEnvContext() );
	}
	return s_Context_;
}

//==============================================================================
EXPORT bool __cdecl PLG_CALLBACK( GenericPluginLoadState loadState )
{
	static PluginMain * s_pluginMain = nullptr;
	auto contextManager = PluginMain::getContext();
	assert( contextManager );
	switch (loadState)
	{
	case GenericPluginLoadState::Create:
		s_pluginMain = createPlugin( *contextManager );
		return true;

	case GenericPluginLoadState::PostLoad:
		return s_pluginMain->PostLoad( *contextManager );

	case GenericPluginLoadState::Initialise:
		s_pluginMain->Initialise( *contextManager );
		return true;

	case GenericPluginLoadState::Finalise:
		return s_pluginMain->Finalise( *contextManager );

	case GenericPluginLoadState::Unload:
		s_pluginMain->Unload( *contextManager );
		return true;

	case GenericPluginLoadState::Destroy:
		delete s_pluginMain;
		s_pluginMain = nullptr;
		return true;

	default:
		break;
	}
	return false;
}
} // end namespace wgt
