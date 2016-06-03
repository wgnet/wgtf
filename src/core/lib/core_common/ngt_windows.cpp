#include "ngt_windows.hpp"

#ifdef __APPLE__

void ZeroMemory(PVOID Destination, DWORD Length)
{

}

int mbstowcs_s(size_t *pReturnValue, wchar_t *wcstr, size_t sizeInWords, const char *mbstr, size_t count)
{
	return 0;
}

int sprintf_s(char *buffer, size_t sizeOfBuffer, const char *format, ...)
{
	return 0;
}

void SetDllDirectoryA(const char* d)
{

}

HANDLE WINAPI FindFirstFileW(_In_ const wchar_t* lpFileName, _Out_ WIN32_FIND_DATA* lpFindFileData)
{
	return nullptr;
}

HANDLE WINAPI FindNextFile(HANDLE lpFileName, _Out_ WIN32_FIND_DATA* lpFindFileData)
{
	return nullptr;
}

DWORD GetLastError()
{
	return 0;
}

BOOL WINAPI OpenClipboard(_In_opt_ HWND hWndNewOwner)
{
	return true;
}

BOOL WINAPI EmptyClipboard(void)
{
	return true;
}

HGLOBAL WINAPI GlobalAlloc(_In_ UINT uFlags, _In_ SIZE_T dwBytes)
{
	return nullptr;
}

BOOL WINAPI CloseClipboard(void)
{
	return true;
}

LPVOID WINAPI GlobalLock(_In_ HGLOBAL hMem)
{
	return nullptr;
}

BOOL WINAPI GlobalUnlock(_In_ HGLOBAL hMem)
{
	return true;
}

HANDLE WINAPI SetClipboardData(_In_ UINT uFormat, _In_opt_ HANDLE hMem)
{
	return nullptr;
}

HGLOBAL WINAPI GlobalFree(_In_ HGLOBAL hMem)
{
	return nullptr;
}

HANDLE WINAPI GetClipboardData(_In_ UINT uFormat)
{
	return nullptr;
}

SIZE_T WINAPI GlobalSize(_In_ HGLOBAL hMem)
{
	return 0u;
}

BOOL WINAPI IsClipboardFormatAvailable(_In_ UINT format)
{
	return true;
}

HRESULT CoCreateGuid(_Out_ GUID *pguid)
{
	return 0;
}

DWORD WINAPI GetCurrentDirectory(_In_ DWORD nBufferLength, _Out_ wchar_t* lpBuffer)
{
	return 0;
}

BOOL WINAPI SetCurrentDirectory(_In_ const wchar_t* lpPathName)
{
	return true;
}

LPWSTR WINAPI GetCommandLineW(void)
{
	return nullptr;
}

LPWSTR* CommandLineToArgvW(_In_ LPCWSTR lpCmdLine, _Out_ int *pNumArgs)
{
	return nullptr;
}

DWORD WINAPI GetEnvironmentVariableA(_In_opt_ LPCTSTR lpName, _Out_opt_ LPTSTR lpBuffer, _In_ DWORD nSize)
{
	return 0;
}

BOOL WINAPI SetEnvironmentVariableA(_In_ LPCTSTR lpName, _In_opt_ LPCTSTR lpValue)
{
	return false;
}

int strncpy_s(char *restrict dest, DWORD destsz,
                  const char *restrict src, DWORD count)
{
	return 0;
}

HANDLE WINAPI GetCurrentProcess(void)
{
	return nullptr;
}

HMODULE WINAPI GetModuleHandleW(
  _In_opt_ const wchar_t* lpModuleName
)
{
	return nullptr;
}

HANDLE WINAPI CreateEvent(
  _In_opt_ LPSECURITY_ATTRIBUTES lpEventAttributes,
  _In_     BOOL                  bManualReset,
  _In_     BOOL                  bInitialState,
  _In_opt_ LPCTSTR               lpName
)
{
	return nullptr;
}

DWORD WINAPI WaitForSingleObject(
  _In_ HANDLE hHandle,
  _In_ DWORD  dwMilliseconds
)
{
	return 0;
}

SIZE_T WINAPI VirtualQuery(
  _In_opt_ LPCVOID                   lpAddress,
  _Out_    PMEMORY_BASIC_INFORMATION lpBuffer,
  _In_     SIZE_T                    dwLength
)
{
	return 0u;
}

VOID WINAPI Sleep(
	_In_ DWORD dwMilliseconds
)
{
	struct timespec ts = { dwMilliseconds / 1000, (dwMilliseconds % 1000) * 1000 * 1000 };
	nanosleep(&ts, NULL);
}

bool MoveFileA(const char* path, const char* new_path)
{
	return true;
}

#endif // __APPLE__
