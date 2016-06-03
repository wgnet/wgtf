#include "shared_library.hpp"
#include <dlfcn.h>


namespace wgt
{
bool SharedLibrary::load(const char* fileName)
{
	unload();

	lib_ = dlopen(fileName, RTLD_NOW);

	return isValid();
}


void SharedLibrary::unload()
{
	if(!isValid())
	{
		return;
	}

	dlclose(lib_);
	lib_ = nullptr;
}


void* SharedLibrary::findRawSymbol(const char* name) const
{
	if(!isValid())
	{
		return nullptr;
	}

	return dlsym(lib_, name);
}
} // end namespace wgt
