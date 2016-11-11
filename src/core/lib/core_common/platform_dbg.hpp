#ifndef __PLATFORM_DBG_HPP__
#define __PLATFORM_DBG_HPP__

#include <string>

#if defined(_WIN32)
#include <windows.h>
#endif

#ifdef __APPLE__

void OutputDebugString(const char* s);
void OutputDebugStringA(const char* s);
void OutputDebugString(const wchar_t* s);

#endif // __APPLE__

namespace wgt
{
bool FormatLastErrorMessage(std::string& errorMsg);
void FlushDebugString();
} // end namespace wgt
#endif // __PLATFORM_DBG_HPP__
