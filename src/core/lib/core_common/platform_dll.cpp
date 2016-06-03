#include "platform_dll.hpp"

#ifdef __APPLE__

#include <dlfcn.h>
#include <locale>
#include <codecvt>
#include <cstring>

void* GetProcAddress(void* handle, const char* symbol)
{
	return dlsym(handle, symbol);
}

uint GetModuleFileNameA(HMODULE hModule, char* filename, uint size)
{
	Dl_info info;
	if (!dladdr( hModule != nullptr ? hModule : reinterpret_cast<void*>(GetModuleFileNameA), &info ))
	{
		return 0;
	}
	strcpy(filename, info.dli_fname);
	return 1;
}

uint GetModuleFileNameW(HMODULE hModule, wchar_t* filename, uint size)
{
	char path[size];
	if (!GetModuleFileNameA( hModule, path, size ))
	{
		return 0;
	}
	std::wstring_convert< std::codecvt_utf8<wchar_t> > conv;
	wcscpy(filename, conv.from_bytes(path).c_str());
	return 1;
}

uint GetModuleFileName(HMODULE hModule, wchar_t* filename, uint size)
{
	return GetModuleFileNameW(hModule, filename, size);
}

HMODULE LoadLibraryW(const wchar_t* fileName)
{
	std::wstring_convert< std::codecvt_utf8<wchar_t> > conv;
	return LoadLibraryA(conv.to_bytes(fileName).c_str());
}

HMODULE LoadLibraryA(const char* fileName)
{
	return dlopen(fileName, RTLD_LAZY);
}

bool FreeLibrary(HMODULE hModule)
{
	return dlclose(hModule);
}

#endif // __APPLE__
