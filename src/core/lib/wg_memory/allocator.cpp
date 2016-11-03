#include <memory.h>
#include <cassert>
#include <cstdlib>
#include <memory>
#include <unordered_map>
#include <vector>
#include <mutex>

#include "wg_types/hash_utilities.hpp"

#include "core_common/ngt_windows.hpp"
#include "core_common/thread_local_value.hpp"

#include "allocator.hpp"
#include <algorithm>
#include <string>
#include <thread>
#include <cwchar>


namespace wgt
{
static bool ALLOCATOR_DEBUG_OUTPUT = false;
static bool ALLOCATOR_STACK_TRACES = false;
static bool ALLOCATOR_LEAK_DETECTION = false;

// Windows stack helper function definitions
typedef USHORT (__stdcall* RtlCaptureStackBackTraceFuncType)(ULONG FramesToSkip, ULONG FramesToCapture, PVOID* BackTrace, PULONG BackTraceHash);

// DbgHelp functions definitions
typedef BOOL	(__stdcall *SymInitializeFuncType)(HANDLE hProcess, PSTR UserSearchPath, BOOL fInvadeProcess);
typedef BOOL (__stdcall *SymFromAddrFuncType)(HANDLE hProcess, DWORD64 Address, PDWORD64 Displacement, PSYMBOL_INFO Symbol);
typedef DWORD	(__stdcall *SymSetOptionsFuncType)(DWORD SymOptions);
typedef BOOL (__stdcall *SymSetSearchPathFuncType)(  HANDLE hProcess, PCSTR SearchPath );
typedef BOOL(__stdcall *SymGetLineFromAddr64FuncType)(HANDLE hProcess, DWORD64 qwAddr, PDWORD pdwDisplacement, PIMAGEHLP_LINE64 Line64);

RtlCaptureStackBackTraceFuncType	RtlCaptureStackBackTraceFunc;
SymFromAddrFuncType					SymFromAddrFunc;
SymSetOptionsFuncType				SymSetOptionsFunc;
SymInitializeFuncType				SymInitializeFunc;
SymSetSearchPathFuncType			SymSetSearchPathFunc;
SymGetLineFromAddr64FuncType		SymGetLineFromAddr64Func;

#ifdef __APPLE__
namespace mem_debug
{
	USHORT __stdcall RtlCaptureStackBackTrace(ULONG FramesToSkip, ULONG FramesToCapture, PVOID* BackTrace, PULONG BackTraceHash)
	{
		return 0;
	}

	BOOL __stdcall SymInitialize(HANDLE hProcess, PSTR UserSearchPath, BOOL fInvadeProcess)
	{
		return true;
	}

	BOOL __stdcall SymFromAddr(HANDLE hProcess, DWORD64 Address, PDWORD64 Displacement, PSYMBOL_INFO Symbol)
	{
		return true;
	}

	DWORD __stdcall SymSetOptions(DWORD SymOptions)
	{
		return 0;
	}

	BOOL __stdcall SymSetSearchPath(  HANDLE hProcess, PCSTR SearchPath )
	{
		return true;
	}

	BOOL __stdcall SymGetLineFromAddr64(HANDLE hProcess, DWORD64 qwAddr, PDWORD pdwDisplacement, PIMAGEHLP_LINE64 Line64)
	{
		return true;
	}
}
#endif // __APPLE__

namespace NGTAllocator
{

class MemoryContext
{
	static const size_t numFramesToCapture_ = 25;

private:
	template<class T>
	class UntrackedAllocator
	{
	public:
		typedef T				    value_type;

		typedef value_type          * pointer;
		typedef value_type          & reference;
		typedef const value_type    * const_pointer;
		typedef const value_type    & const_reference;

		typedef size_t      size_type;
		typedef ptrdiff_t   difference_type;

		template <class Other>
		struct rebind
		{
			typedef UntrackedAllocator<Other> other;
		};


		UntrackedAllocator()
		{
		}

		template <typename Other>
		UntrackedAllocator( const UntrackedAllocator< Other > & )
		{
		}

		typename std::allocator<T>::pointer allocate(
			typename std::allocator<T>::size_type n, typename std::allocator<void>::const_pointer = 0 )
		{
			return (typename std::allocator<T>::pointer) ::malloc( n * sizeof( T ) );
		}

		void deallocate( typename std::allocator<T>::pointer p, typename std::allocator<T>::size_type n )
		{
			::free( p );
		}

		template<typename Arg>
		void construct( pointer p, Arg && val ) const
		{
			new ((void*)p) T( std::forward<Arg>(val) );
		}


		void destroy( pointer p ) const
		{
			p->~T();
		}

		size_type max_size() const
		{
			size_type _Count = (size_type)(-1) / sizeof (T);
			return (0 < _Count ? _Count : 1);
		}
	};

	typedef std::basic_string<char, std::char_traits<char>, UntrackedAllocator<char>> UntrackedString;

public:
	MemoryContext()
		: allocId_( 0 )
		, parentContext_( nullptr )
	{
		wcscpy( name_, L"root" );
#ifdef _WIN32
		HMODULE kernel32 = ::LoadLibraryA( "kernel32.dll" );
		assert( kernel32 );
		RtlCaptureStackBackTraceFunc = ( RtlCaptureStackBackTraceFuncType )
			::GetProcAddress( kernel32, "RtlCaptureStackBackTrace" );
#elif __APPLE__
		RtlCaptureStackBackTraceFunc = mem_debug::RtlCaptureStackBackTrace;
#endif
	}

	MemoryContext( const wchar_t * name, MemoryContext * parentContext )
		: allocId_( 0 )
		, parentContext_( parentContext )
	{
		assert( parentContext_ != nullptr );
		wcscpy( name_, name );

		std::lock_guard< std::mutex > childContextsGuard(parentContext_->childContextsLock_);
		parentContext_->childContexts_.push_back( this );
	}


	~MemoryContext()
	{
		const auto success = printLeaks();
		assert((ALLOCATOR_LEAK_DETECTION ? success : true) && "Memory leaks detected");
		if (parentContext_ != nullptr)
		{
			std::lock_guard< std::mutex > childContextsGuard(parentContext_->childContextsLock_);
			auto& childContexts = parentContext_->childContexts_;
			auto foundIt = std::find( childContexts.cbegin(),
				childContexts.cend(),
				this );
			assert( foundIt != childContexts.cend() );
			childContexts.erase( foundIt );
		}
	}


	void * allocate( size_t size )
	{
		AllocationPtr allocation = AllocationPtr();

		{
			std::lock_guard< std::mutex > allocationPoolGuard(allocationPoolLock_);
			if (!allocationPool_.empty())
			{
				allocation = std::move(allocationPool_.back());
				allocationPool_.pop_back();
			}
		}

		if(allocation == nullptr)
		{
			allocation.reset(new Allocation());
		}

		allocation->frames_ = 0;
		if (ALLOCATOR_STACK_TRACES)
		{
			allocation->frames_ =
				RtlCaptureStackBackTraceFunc(3, numFramesToCapture_, allocation->addrs_, NULL);
		}

		auto ptr = ::malloc( size );

		{
			std::lock_guard< std::mutex > allocationGuard(allocationLock_);
			allocation->allocId_ = allocId_++;
			liveAllocations_.insert( std::make_pair( ptr, std::move( allocation ) ) );
		}

		if (ALLOCATOR_DEBUG_OUTPUT)
		{
			std::hash<std::thread::id> h;
			wprintf(L"alloc ptr %#zx context %ls %#zx (thread %#zx)\n", (size_t)ptr, name_, (size_t)this, h(std::this_thread::get_id()));
		}

		return ptr;
	}


	void deallocate( void* ptr )
	{
		// try to deallocate using this context or its children
		if (deallocate( ptr, nullptr ))
		{
			return;
		}

		// climb up to root context
		auto parentContext = this;
		while (parentContext->parentContext_)
		{
			parentContext = parentContext->parentContext_;
		}

		// try to deallocate using root context or its children, except this one
		if (parentContext->deallocate( ptr, this ))
		{
			return;
		}

		// failed to find a proper context
		char msg[ 128 ];
		snprintf( msg, 128, "deallocate: failed to find memory context for %p\n", ptr );
		::OutputDebugStringA( msg );

		::free( ptr );
	}

	UntrackedString resolveSymbol(HANDLE currentProcess, void* ptr)
	{
		auto findIt = stackCache_.find(ptr);
		if (findIt != stackCache_.end())
		{
			return findIt->second;
		}

		UntrackedString builder;

		// Allocate a buffer large enough to hold the symbol information on the stack and get
		// a pointer to the buffer.  We also have to set the size of the symbol structure itself
		// and the number of bytes reserved for the name.
		const int MaxSymbolNameLength = 1024;
		ULONG64 buffer[(sizeof(SYMBOL_INFO) + MaxSymbolNameLength + sizeof(ULONG64) - 1) / sizeof(ULONG64)] = { 0 };
		SYMBOL_INFO* info = (SYMBOL_INFO*)buffer;
		info->SizeOfStruct = sizeof(SYMBOL_INFO);
		info->MaxNameLen = MaxSymbolNameLength;

		// Attempt to get information about the symbol and add it to our output parameter.
		DWORD64 displacement64 = 0;
		DWORD displacement = 0;

		char nameBuf[1024] = { 0 };
		if (SymFromAddrFunc(
		    currentProcess,
		    (DWORD64)ptr,
		    &displacement64, info))
		{
			strncat(nameBuf, info->Name, info->NameLen);
		}
		else
		{
			// Unable to find the name, so lets get the module or address
			MEMORY_BASIC_INFORMATION mbi;
			char fullPath[MAX_PATH];
			if (VirtualQuery(ptr, &mbi, sizeof(mbi)) &&
			    GetModuleFileNameA((HMODULE)mbi.AllocationBase, fullPath, sizeof(fullPath)))
			{
				// Get base name of DLL
				char* filename = strrchr(fullPath, '\\');
				strncpy_s(nameBuf, sizeof(nameBuf), filename == NULL ? fullPath : (filename + 1), _TRUNCATE);
			}
			else
			{
				sprintf_s(nameBuf, sizeof(nameBuf), "0x%p", ptr);
			}
		}

		IMAGEHLP_LINE64 source_info;
		char lineBuffer[1024] = { 0 };
		::ZeroMemory(&source_info, sizeof(IMAGEHLP_LINE64));
		source_info.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
		if (SymGetLineFromAddr64Func(
		    currentProcess,
		    (DWORD64)ptr,
		    &displacement, &source_info))
		{
			sprintf(lineBuffer, "%s(%d)", source_info.FileName, source_info.LineNumber);
		}
		char outputBuffer[2048] = { 0 };
		sprintf(outputBuffer, "%s : %s\n", lineBuffer, nameBuf);
		builder.append(outputBuffer);

		stackCache_.insert(std::make_pair(ptr, builder));

		return builder;
	}

	void initSymbols()
	{
#ifdef _WIN32
		HMODULE dbghelp  = ::LoadLibraryA( "dbghelp.dll" );
		assert( dbghelp );
		SymFromAddrFunc = ( SymFromAddrFuncType )
			::GetProcAddress( dbghelp, "SymFromAddr" );
		SymSetOptionsFunc = ( SymSetOptionsFuncType )
			::GetProcAddress( dbghelp, "SymSetOptions" );
		SymInitializeFunc = ( SymInitializeFuncType )
			::GetProcAddress( dbghelp, "SymInitialize" );
		SymSetSearchPathFunc = ( SymSetSearchPathFuncType )
			::GetProcAddress( dbghelp, "SymSetSearchPath" );
		SymGetLineFromAddr64Func = ( SymGetLineFromAddr64FuncType )
			::GetProcAddress( dbghelp, "SymGetLineFromAddr64" );
#elif __APPLE__
		SymFromAddrFunc = mem_debug::SymFromAddr;
		SymSetOptionsFunc = mem_debug::SymSetOptions;
		SymInitializeFunc = mem_debug::SymInitialize;
		SymSetSearchPathFunc = mem_debug::SymSetSearchPath;
		SymGetLineFromAddr64Func = mem_debug::SymGetLineFromAddr64;
#endif

		auto currentProcess = ::GetCurrentProcess();

		static bool symbolsLoaded = false;
		if (symbolsLoaded)
		{
			return;
		}

		UntrackedString builder;

		// build PDB path that should be the same as executable path
		{
			char path[_MAX_PATH] = { 0 };
			wcstombs(path, name_, wcslen(name_));

			// check that the pdb actually exists and is accessible, if it doesn't then SymInitialize will raise an obscure error dialog
			// so just disable complete callstacks if it is not there
			char* pend = nullptr;
			if ((pend = strrchr(path, '.')))
				*pend = 0;
			strcat(path, ".pdb");
			FILE* f = fopen(path, "rb");
			if (f == NULL)
			{
				return;
			}
			fclose(f);

			if ((pend = strrchr(path, '\\')))
			{
				*pend = 0;
			}
			else if ((pend = strrchr(path, '/')))
			{
				*pend = 0;
			}

			builder.append(path);
		}

		// append the working directory.
		builder.append(";.\\");

		// append %SYSTEMROOT% and %SYSTEMROOT%\system32.
		char* env = getenv("SYSTEMROOT");
		if (env)
		{
			builder.append(";");
			builder.append(env);
		}

		// append %_NT_SYMBOL_PATH% and %_NT_ALT_SYMBOL_PATH%.
		if ((env = getenv("_NT_SYMBOL_PATH")))
		{
			builder.append(";");
			builder.append(env);
		}
		if ((env = getenv("_NT_ALT_SYMBOL_PATH")))
		{
			builder.append(";");
			builder.append(env);
		}
		builder.append(";");
		builder.append("http://msdl.microsoft.com/download/symbols");

		SymSetOptionsFunc(SYMOPT_LOAD_LINES | SYMOPT_DEFERRED_LOADS | SYMOPT_UNDNAME);
		SymInitializeFunc(currentProcess, (PSTR)builder.c_str(), TRUE);

		builder.clear();

		symbolsLoaded = true;
	}

	void cleanup()
	{
		initSymbols();
		wchar_t contextName[ 2048 ];
		swprintf( contextName, 2048, L"Destroying memory context for %s\n", name_ );
		::OutputDebugString( contextName );

		auto currentProcess = ::GetCurrentProcess();

		{
			std::lock_guard< std::mutex > allocationGuard(allocationLock_);
			for( auto & liveAllocation : liveAllocations_)
			{
				const auto & allocStack = liveAllocation.second;
				for( size_t i = 0; i < allocStack->frames_; ++i )
				{
					resolveSymbol(currentProcess, liveAllocation.second->addrs_[i]);
				}
			}
		}
	}

	/// @return false on failure or if memory leaks were detected
	bool printLeaks()
	{
		initSymbols();
		wchar_t contextName[2048];
		swprintf(contextName, 2048, L"Destroying memory context for %s\n", name_);
		::OutputDebugString(contextName);

		auto currentProcess = ::GetCurrentProcess();

		bool hasLeaks = false;
		{
			UntrackedString builder;
			std::lock_guard<std::mutex> allocationGuard(allocationLock_);

			hasLeaks = !liveAllocations_.empty();

			struct UniqueAlloc
			{
				UntrackedString stackOutput_;

				struct AllocationBasic
				{
					AllocationBasic(size_t allocId, void* address)
					    : allocId_(allocId)
					    , address_(address)
					{
					}

					size_t allocId_;
					void* address_;
				};
				std::vector<AllocationBasic, UntrackedAllocator<AllocationBasic>> allocations_;
			};
			typedef std::unordered_map<
			uint64_t,
			UniqueAlloc,
			std::hash<uint64_t>,
			std::equal_to<uint64_t>,
			UntrackedAllocator<std::pair<const uint64_t, UniqueAlloc>>> UniqueStackCollection;
			UniqueStackCollection uniqueStackInstances;

			for (auto& liveAllocation : liveAllocations_)
			{
				const auto& allocStack = liveAllocation.second;

				uint64_t hash = 0;
				for (size_t i = 0; i < allocStack->frames_; ++i)
				{
					HashUtilities::directCombine(hash, (uint64_t)liveAllocation.second->addrs_[i]);
				}

				auto findIt = uniqueStackInstances.find(hash);
				UniqueAlloc uniqueAlloc;
				if (findIt == uniqueStackInstances.end())
				{
					for (size_t i = 0; i < allocStack->frames_; ++i)
					{
						uniqueAlloc.stackOutput_.append(
						resolveSymbol(currentProcess, liveAllocation.second->addrs_[i]));
					}
					uniqueAlloc.allocations_.emplace_back(UniqueAlloc::AllocationBasic(liveAllocation.second->allocId_, liveAllocation.first));
					uniqueStackInstances.insert(std::make_pair(hash, uniqueAlloc));
				}
				else
				{
					findIt->second.allocations_.emplace_back(UniqueAlloc::AllocationBasic(liveAllocation.second->allocId_, liveAllocation.first));
					continue;
				}
			}
			for (auto& uniqueStack : uniqueStackInstances)
			{
				{
					builder.append("Leaked allocation: [id:address] ");
				}
				for (auto& allocation : uniqueStack.second.allocations_)
				{
					char allocIdBuffer[2048] = { 0 };
					sprintf(allocIdBuffer, "[%lu:0x%p], ",
					        static_cast<unsigned long>(allocation.allocId_),
					        allocation.address_);
					builder.append(allocIdBuffer);
				}
				builder.append("\n");
				builder.append(uniqueStack.second.stackOutput_);
				builder.append( "\n\n" );
				::OutputDebugStringA( builder.c_str() );
				builder.clear();
			}

			for (auto& liveAllocation : liveAllocations_)
			{
				liveAllocation.second.reset();
			}
			liveAllocations_.clear();
		}

		{
			std::lock_guard< std::mutex > allocationPoolGuard(allocationPoolLock_);
			allocationPool_.clear();
		}

		return !hasLeaks;
	}

private:
	struct Allocation
	{
		void *	addrs_[ numFramesToCapture_ ];
		size_t	frames_;
		size_t	allocId_;

		static void* operator new(size_t sz)
		{
			return ::malloc(sz);
		}

		static void operator delete(void* ptr)
		{
			return ::free(ptr);
		}
	};

	typedef std::unique_ptr< Allocation > AllocationPtr;

	wchar_t name_[ 255 ];
	MemoryContext * parentContext_;

	std::mutex allocationPoolLock_;
	std::vector< AllocationPtr, UntrackedAllocator< AllocationPtr > > allocationPool_;

	std::mutex childContextsLock_;
	std::vector< MemoryContext *, UntrackedAllocator< MemoryContext * > > childContexts_;

	std::mutex allocationLock_;
	size_t allocId_;
	std::unordered_map<
		void *,
		AllocationPtr,
		std::hash< void *>,
		std::equal_to< void * >,
		UntrackedAllocator< std::pair< void * const, AllocationPtr > > > liveAllocations_;

	typedef std::unordered_map<
	void*,
	UntrackedString,
	std::hash<void*>,
	std::equal_to<void*>,
	UntrackedAllocator<std::pair<void* const, UntrackedString>>> StackCache;

	StackCache stackCache_;

	/**
	Deallocate using this context or its children recursively.
	*/
	bool deallocate( void* ptr, MemoryContext* skip )
	{
		if (this == skip)
		{
			return false;
		}

		{
			std::lock_guard< std::mutex > allocationGuard(allocationLock_);
			auto findIt = liveAllocations_.find( ptr );
			if (findIt != liveAllocations_.end())
			{
				if (ALLOCATOR_DEBUG_OUTPUT)
				{
					std::hash<std::thread::id> h;
					wprintf(L"dealloc ptr %#zx context %ls %#zx (thread %#zx)\n", (size_t)ptr, name_, (size_t)this, h(std::this_thread::get_id()));
				}

				{
					std::lock_guard< std::mutex > allocationPoolGuard(allocationPoolLock_);
					allocationPool_.push_back(std::move(findIt->second));
				}

				liveAllocations_.erase( findIt );
				::free( ptr );
				return true;
			}
		}

		std::lock_guard< std::mutex > childContextsGuard(childContextsLock_);
		for (auto context : childContexts_)
		{
			if(context->deallocate( ptr, skip ))
			{
				return true;
			}
		}

		return false;
	}

};


#ifdef WIN32
#pragma warning(disable: 4073)
#pragma init_seg(lib)				// Ensure we get constructed first
#endif // WIN32

MemoryContext					rootContext_;
THREAD_LOCAL( int )				s_MemoryStackPos( 0 );
THREAD_LOCAL( MemoryContext * )	s_MemoryContext[ 20 ];

//------------------------------------------------------------------------------
MemoryContext * getMemoryContext()
{
	int id = THREAD_LOCAL_GET(s_MemoryStackPos);
	MemoryContext* mc =  (id > 0) ? THREAD_LOCAL_GET(s_MemoryContext[ id - 1 ]) : &rootContext_;
	if (!mc)
	{
		if (ALLOCATOR_DEBUG_OUTPUT)
		{
			std::hash<std::thread::id> h;
			printf("ERROR - Thread id %#zx mem context %d\n", h(std::this_thread::get_id()), id);
		}
	}
	assert(mc);
	return mc;
}

//------------------------------------------------------------------------------
void * allocate( size_t size )
{
	auto memoryContext = getMemoryContext();
	return memoryContext->allocate( size );
}


//------------------------------------------------------------------------------
void deallocate( void * ptr )
{
	if (ptr != nullptr)
	{
		auto memoryContext = getMemoryContext();
		memoryContext->deallocate( ptr );
	}
}


//------------------------------------------------------------------------------
void * createMemoryContext( const wchar_t * name )
{
	return new MemoryContext( name, getMemoryContext() );
}


//------------------------------------------------------------------------------
void destroyMemoryContext( void * pContext )
{
	delete static_cast< MemoryContext * >( pContext );
}


//------------------------------------------------------------------------------
void pushMemoryContext( void * pContext )
{
	assert( pContext != nullptr );
	int id = THREAD_LOCAL_GET(s_MemoryStackPos);
	THREAD_LOCAL_SET(s_MemoryContext[ id ], static_cast< MemoryContext * >( pContext ));
	id = THREAD_LOCAL_INC(s_MemoryStackPos);

	if (ALLOCATOR_DEBUG_OUTPUT)
	{
		std::hash<std::thread::id> h;
		printf("PUSH - Thread %#zx mem context id %d (%#zx)\n", h(std::this_thread::get_id()), id, (size_t)pContext);
	}
}


//------------------------------------------------------------------------------
void popMemoryContext()
{
	int id = THREAD_LOCAL_DEC(s_MemoryStackPos);
	assert( id >= 0 );
	void* mc = THREAD_LOCAL_GET(s_MemoryContext[ id ]);
	THREAD_LOCAL_SET(s_MemoryContext[ id ], nullptr);

	if (ALLOCATOR_DEBUG_OUTPUT)
	{
		std::hash<std::thread::id> h;
		printf("POP  - Thread %#zx mem context id %d (%#zx)\n", h(std::this_thread::get_id()), id, (size_t)mc);
	}
}


//------------------------------------------------------------------------------
void cleanupContext( void * pContext )
{
	auto memoryContext = static_cast< MemoryContext * >( pContext );
	memoryContext->cleanup();
}


//------------------------------------------------------------------------------
void enableDebugOutput( bool enable )
{
	ALLOCATOR_DEBUG_OUTPUT = enable;
}


//------------------------------------------------------------------------------
void enableStackTraces( bool enable )
{
	ALLOCATOR_STACK_TRACES = enable;
}

//------------------------------------------------------------------------------
void enableLeakDetection(bool enable)
{
	ALLOCATOR_LEAK_DETECTION = enable;
}
}
} // end namespace wgt
