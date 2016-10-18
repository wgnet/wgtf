#ifndef __PLATFORM_PATH_HPP__
#define __PLATFORM_PATH_HPP__

#if defined( _WIN32 )
#include <windows.h>
#include <shlwapi.h>
#include <shellapi.h>
#endif

#ifdef __APPLE__
#include <sys/syslimits.h>
#define MAX_PATH PATH_MAX
#define _MAX_PATH PATH_MAX
#endif

namespace wgt
{
void AddDllExtension( wchar_t* file );
bool GetUserDirectoryPath( char (&path) [MAX_PATH] );
bool CreateDirectoryPath( const char* path );
} // end namespace wgt

#ifdef __APPLE__

bool PathIsRelative(const char* path);
bool PathIsRelative(const wchar_t* path);

bool PathCanonicalize(wchar_t*  dst, const wchar_t* src);
bool PathCanonicalizeW(wchar_t*  dst, const wchar_t* src);

void PathRemoveExtension(wchar_t* path);
bool PathAddExtension(wchar_t* path, const wchar_t* ext);

bool PathRemoveFileSpecA(char* path);
bool PathRemoveFileSpecW(wchar_t* path);
bool PathRemoveFileSpec(char* path);
bool PathRemoveFileSpec(wchar_t* path);

bool PathAppend(wchar_t* path, const wchar_t* more);
bool PathAppendW(wchar_t* path, const wchar_t* more);

namespace wgt
{
void PathFileName(wchar_t* file, const wchar_t* path);
} // end namespace wgt

#endif // __APPLE__
#endif // __PLATFORM_PATH_HPP__
