#include "platform_dbg.hpp"

#include <ctime>
#include <sstream>
#include <fstream>

#ifdef __APPLE__
#include <stdio.h>
#include <errno.h>
#include <wchar.h>
#include <dlfcn.h>
#include <string.h>
#endif

namespace wgt
{
std::string getNewDebugFileName(const std::string& name)
{
	time_t t = time(0);
	const tm* now = localtime(&t);
	std::ostringstream stream;
	stream << name << "-"
		<< (now->tm_year + 1900) << "-"
		<< (now->tm_mon + 1) << "-"
		<< now->tm_mday << "-"
		<< now->tm_hour << "-"
		<< now->tm_min << "-"
		<< now->tm_sec << ".txt";
	return stream.str();
}

void OutputDebugToFile(const std::string& name, const std::string& debug, bool openOnSave)
{
	const auto filename = getNewDebugFileName(name);
	std::fstream file(filename.c_str(), std::ios_base::out);
	if(file.is_open())
	{
		file.write(&debug.front(), debug.size());
		file.close();
	}

	if(openOnSave)
	{
#ifdef _WIN32
		system(("start " + filename).c_str());
#elif __APPLE__
		// Not implemented
#endif
	}
}

void FlushDebugString()
{
#ifdef _WIN32
	// Do nothing
#elif __APPLE__
	fflush(stdout);
#endif
}

bool FormatLastErrorMessage(std::string& errorMsg)
{
#ifdef _WIN32
	// Must be called first
	const unsigned int lastError = GetLastError();

	static const size_t errorMsgLength = 4096;
	errorMsg.resize(errorMsgLength);

	bool hadError = false;

	if (lastError != ERROR_SUCCESS)
	{
		FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, 0, lastError, 
			0, &errorMsg.front(), (unsigned int)errorMsgLength, 0);
		hadError = true;
	}
	return hadError;
#elif __APPLE__
	const char* dlerr = dlerror();

	if (dlerr)
	{
		errorMsg = dlerr;
	}
	else if (errno)
	{
		errorMsg = strerror(errno);
	}

	return !errorMsg.empty();
#endif
}
} // end namespace wgt

#ifdef __APPLE__
void OutputDebugString(const char* s)
{
	printf("%s", s);
}

void OutputDebugString(const wchar_t* s)
{
	wprintf(L"%ls", s);
}

void OutputDebugStringA(const char* s)
{
	printf("%s", s);
}
#endif // __APPLE__
