#ifndef NGT_WINDOWS_HPP_INCLUDED
#define NGT_WINDOWS_HPP_INCLUDED

#if defined(_WIN32)
//#define WIN32_LEAN_AND_MEAN
#if !defined(NOMINMAX)
#define NOMINMAX
#endif
#include <objbase.h>

#pragma warning(push)
#pragma warning(disable : 4091)
#include <DbgHelp.h>
#pragma warning(pop)

#if (_MSC_VER < 1900)
#if !defined(HAVE_SNPRINTF) && !defined(snprintf)
#define HAVE_SNPRINTF
#define snprintf _snprintf
#define vsnprintf _vsnprintf
#endif
#endif
typedef unsigned __int64 __uint64;
#endif

#include "platform_dbg.hpp"
#include "platform_dll.hpp"
#include "platform_path.hpp"
#include "platform_std.hpp"

#ifdef __APPLE__
#include <stddef.h>
#include <inttypes.h>
#include <time.h>
#include <cstdarg>

#define WCHAR wchar_t
#define _TRUNCATE 0
#define WINAPI
#define _Inout_
#define _In_opt_
#define _Out_opt_
#define _Out_
#define _In_
#define INVALID_HANDLE_VALUE 0
#define ERROR_SUCCESS 0
#define FORMAT_MESSAGE_FROM_SYSTEM 0
#define __stdcall
#define TRUE true
#define restrict
#define GMEM_MOVEABLE 0x0002
#define CF_TEXT 1
#define STDMETHODCALLTYPE
#define FALSE 0

#define SYMOPT_LOAD_LINES 0x1
#define SYMOPT_DEFERRED_LOADS 0x2
#define SYMOPT_UNDNAME 0x4

typedef void VOID;
typedef void* PVOID;
typedef char* LPTSTR;
typedef char* LPSTR;
typedef const char* LPCTSTR;
typedef const char* LPCSTR;
typedef char* PSTR;
typedef wchar_t* LPWSTR;
typedef const wchar_t* LPCWSTR;
typedef const char* PCSTR;
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef DWORD* PDWORD;
typedef bool BOOL;
typedef int64_t __int64;
typedef uint64_t __uint64;
typedef __uint64 DWORD64, *PDWORD64;
typedef __uint64 ULONG64, *PULONG64;
typedef unsigned short USHORT;
typedef unsigned long ULONG;
typedef long LONG;
typedef unsigned int UINT;
typedef ULONG* PULONG;
typedef DWORD SIZE_T;
typedef void* LPVOID;
typedef const void* LPCVOID;
typedef LONG HRESULT;

#define FAILED(hr) (((HRESULT)(hr)) < 0)

typedef struct _GUID
{
	DWORD Data1;
	WORD Data2;
	WORD Data3;
	BYTE Data4[8];
} GUID;

typedef struct _SYMBOL_INFO
{
	ULONG SizeOfStruct;
	ULONG MaxNameLen;
	char Name[1];
	ULONG NameLen;

} SYMBOL_INFO, *PSYMBOL_INFO;

typedef struct _IMAGEHLP_LINE64
{
	ULONG SizeOfStruct;
	const char* FileName;
	int LineNumber;
} IMAGEHLP_LINE64, *PIMAGEHLP_LINE64;

struct WIN32_FIND_DATA
{
	const wchar_t* cFileName;
};

typedef struct _MEMORY_BASIC_INFORMATION
{
	PVOID BaseAddress;
	PVOID AllocationBase;
	DWORD AllocationProtect;
	DWORD RegionSize;
	DWORD State;
	DWORD Protect;
	DWORD Type;
} MEMORY_BASIC_INFORMATION, *PMEMORY_BASIC_INFORMATION;

typedef struct _SECURITY_ATTRIBUTES
{
	DWORD nLength;
	LPVOID lpSecurityDescriptor;
	BOOL bInheritHandle;
} SECURITY_ATTRIBUTES, *PSECURITY_ATTRIBUTES, *LPSECURITY_ATTRIBUTES;

void ZeroMemory(PVOID Destination, DWORD Length);

int mbstowcs_s(size_t* pReturnValue, wchar_t* wcstr, size_t sizeInWords, const char* mbstr, size_t count);

int strncpy_s(char* restrict dest, DWORD destsz, const char* restrict src, DWORD count);

int sprintf_s(char* buffer, size_t sizeOfBuffer, const char* format, ...);

void SetDllDirectoryA(const char* d);

HANDLE WINAPI GetCurrentProcess(void);

HMODULE WINAPI GetModuleHandleW(_In_opt_ const wchar_t* lpModuleName);

SIZE_T WINAPI VirtualQuery(_In_opt_ LPCVOID lpAddress, _Out_ PMEMORY_BASIC_INFORMATION lpBuffer, _In_ SIZE_T dwLength);

HANDLE WINAPI FindFirstFileW(_In_ const wchar_t* lpFileName, _Out_ WIN32_FIND_DATA* lpFindFileData);

HANDLE WINAPI FindNextFile(HANDLE lpFileName, _Out_ WIN32_FIND_DATA* lpFindFileData);

DWORD GetLastError();

DWORD WINAPI FormatMessageA(_In_ DWORD dwFlags, _In_opt_ const void* lpSource, _In_ DWORD dwMessageId,
                            _In_ DWORD dwLanguageId, _Out_ LPTSTR lpBuffer, _In_ DWORD nSize,
                            _In_opt_ va_list* Arguments);

BOOL WINAPI OpenClipboard(_In_opt_ HWND hWndNewOwner);

BOOL WINAPI EmptyClipboard(void);

HGLOBAL WINAPI GlobalAlloc(_In_ UINT uFlags, _In_ SIZE_T dwBytes);

BOOL WINAPI CloseClipboard(void);

LPVOID WINAPI GlobalLock(_In_ HGLOBAL hMem);

BOOL WINAPI GlobalUnlock(_In_ HGLOBAL hMem);

HANDLE WINAPI SetClipboardData(_In_ UINT uFormat, _In_opt_ HANDLE hMem);

HGLOBAL WINAPI GlobalFree(_In_ HGLOBAL hMem);

HANDLE WINAPI GetClipboardData(_In_ UINT uFormat);

SIZE_T WINAPI GlobalSize(_In_ HGLOBAL hMem);

BOOL WINAPI IsClipboardFormatAvailable(_In_ UINT format);

HRESULT CoCreateGuid(_Out_ GUID* pguid);

DWORD WINAPI GetCurrentDirectory(_In_ DWORD nBufferLength, _Out_ wchar_t* lpBuffer);

BOOL WINAPI SetCurrentDirectory(_In_ const wchar_t* lpPathName);

LPWSTR WINAPI GetCommandLineW(void);

LPWSTR* CommandLineToArgvW(_In_ LPCWSTR lpCmdLine, _Out_ int* pNumArgs);

DWORD WINAPI GetEnvironmentVariableA(_In_opt_ LPCTSTR lpName, _Out_opt_ LPTSTR lpBuffer, _In_ DWORD nSize);

BOOL WINAPI SetEnvironmentVariableA(_In_ LPCTSTR lpName, _In_opt_ LPCTSTR lpValue);

HANDLE WINAPI CreateEvent(_In_opt_ LPSECURITY_ATTRIBUTES lpEventAttributes, _In_ BOOL bManualReset,
                          _In_ BOOL bInitialState, _In_opt_ LPCTSTR lpName);

DWORD WINAPI WaitForSingleObject(_In_ HANDLE hHandle, _In_ DWORD dwMilliseconds);

VOID WINAPI Sleep(_In_ DWORD dwMilliseconds);

bool MoveFileA(const char* path, const char* new_path);

#endif // __APPLE__
#endif // NGT_WINDOWS_HPP_INCLUDED
