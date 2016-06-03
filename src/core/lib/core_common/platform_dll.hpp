#ifndef __PLATFORM_DLL_HPP__
#define __PLATFORM_DLL_HPP__

#if defined( _WIN32 )
#include <windows.h>
#endif

#ifdef __APPLE__

typedef void* HMODULE;
typedef void* HINSTANCE;
typedef void* HANDLE;
typedef HANDLE HGLOBAL;
typedef HANDLE HWND;

typedef unsigned int uint;

void* GetProcAddress(void* handle, const char* symbol);

uint GetModuleFileName(HMODULE hModule, wchar_t* filename, uint size);
uint GetModuleFileNameA(HMODULE hModule, char* filename, uint size);
uint GetModuleFileNameW(HMODULE hModule, wchar_t* filename, uint size);

HMODULE LoadLibraryW(const wchar_t* fileName);
HMODULE LoadLibraryA(const char* fileName);
bool FreeLibrary(HMODULE hModule);

#endif // __APPLE__
#endif // __PLATFORM_DLL_HPP__
