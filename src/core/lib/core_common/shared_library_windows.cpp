#include "shared_library.hpp"
#include <cstdint>
#include "ngt_windows.hpp"


namespace wgt
{
bool SharedLibrary::load(const char* fileName)
{
	unload();

	if(fileName)
	{
		lib_ = static_cast<void*>(LoadLibraryA(fileName));
	}
	else
	{
		lib_ = static_cast<void*>(GetModuleHandleA(NULL));
		if(isValid())
		{
			// Set lower bit to indicate that we shouldn't unload library.
			// Modules are aligned to memory page boundary so lower bits are normally zeroed.
			reinterpret_cast<uintptr_t&>(lib_) |= 1;
		}
	}

	return isValid();
}


void SharedLibrary::unload()
{
	if(!isValid())
	{
		return;
	}

	if((reinterpret_cast<uintptr_t>(lib_) & 1) == 0)
	{
		FreeLibrary(static_cast<HMODULE>(lib_));
	}

	lib_ = nullptr;
}


void* SharedLibrary::findRawSymbol(const char* name) const
{
	if(!isValid())
	{
		return nullptr;
	}

	HMODULE mod = reinterpret_cast<HMODULE>(reinterpret_cast<uintptr_t>(lib_) & ~1);
	return reinterpret_cast<void*>(GetProcAddress(mod, name));
}
} // end namespace wgt
