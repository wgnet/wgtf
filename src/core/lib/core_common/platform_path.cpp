#include "platform_path.hpp"
#include "core_string_utils/string_utils.hpp"
#include "core_string_utils/file_path.hpp"
#include "core_common/platform_env.hpp"

#if defined(_WIN32)
#include <shlwapi.h>
#pragma warning(push)
#pragma warning(\
disable : 4091) // warning C4091: 'typedef ': ignored on left of 'tagGPFIDL_FLAGS' when no variable is declared
#include <ShlObj.h>
#pragma warning(pop)
#include <direct.h>
#elif __APPLE__
#include <sys/types.h>
#include <sys/stat.h>
#endif

namespace wgt
{
void AddDllExtension(wchar_t* file)
{
#ifdef _WIN32
	PathAddExtensionW(file, L".dll");
#elif __APPLE__
	PathAddExtension(file, L".dylib");
#endif
}

bool GetUserDirectoryPath(char (&path)[MAX_PATH])
{
	memset(path, '\0', MAX_PATH);
#ifdef _WIN32
	PWSTR userDirectory = nullptr;
	if (!FAILED(SHGetKnownFolderPath(FOLDERID_Documents, 0, 0, &userDirectory)))
	{
		strcpy(path, StringUtils::to_string(userDirectory).c_str());
		return true;
	}
#elif __APPLE__
	if (Environment::getValue<MAX_PATH>("HOME", path) || Environment::getValue<MAX_PATH>("HOMEDRIVE", path))
	{
		return true;
	}
#endif
	return false;
}

bool CreateDirectoryPath(const char* path)
{
#ifdef _WIN32
	return _mkdir(path) == 0 || errno == EEXIST;
#elif __APPLE__
	return mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == 0 || errno == EEXIST;
#endif
}

} // end namespace wgt
#ifdef __APPLE__

#include <codecvt>
#include <cwchar>
#include <locale>
#include <assert.h>
#include <stdlib.h>

bool PathIsRelative(const char* path)
{
	assert(path);
	return *path != '/';
}

bool PathIsRelative(const wchar_t* path)
{
	assert(path);
	return *path != L'/';
}

bool PathCanonicalizeW(wchar_t* dst, const wchar_t* src)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
	char path[PATH_MAX];
	const char* r = realpath(conv.to_bytes(src).c_str(), path);
	wcscpy(dst, conv.from_bytes(path).c_str());
	return r != nullptr;
}

bool PathCanonicalize(wchar_t* dst, const wchar_t* src)
{
	return PathCanonicalizeW(dst, src);
}

void PathRemoveExtension(wchar_t* path)
{
	wchar_t* wc = path;
	while (wchar_t* p = wcschr(wc, L'.')) wc = p + 1;

	wchar_t* sc = path;
	while (wchar_t* p = wcschr(sc, L'/')) sc = p + 1;

	if (wc > sc)
		*(--wc) = 0;
}

bool PathAddExtension(wchar_t* path, const wchar_t* ext)
{
	wcscat(path, ext);
	return true;
}

bool PathRemoveFileSpecA(char* path)
{
	char* c = path;
	while (char* p = strchr(c, '/')) c = p + 1;

	*c = 0;
	return true;
}

bool PathRemoveFileSpecW(wchar_t* path)
{
	wchar_t* wc = path;
	while (wchar_t* p = wcschr(wc, L'/')) wc = p + 1;

	*wc = 0;
	return true;
}

bool PathRemoveFileSpec(char* path)
{
	return PathRemoveFileSpecA(path);
}

bool PathRemoveFileSpec(wchar_t* path)
{
	return PathRemoveFileSpecW(path);
}

bool PathAppendW(wchar_t* path, const wchar_t* more)
{
	wcscat(path, more);
	return true;
}

bool PathAppend(wchar_t* path, const wchar_t* more)
{
	return PathAppendW(path, more);
}

namespace wgt
{
void PathFileName(wchar_t* file, const wchar_t* path)
{
	const wchar_t* f = path;
	while (wchar_t* p = wcschr(f, L'/')) f = p + 1;
	wcscpy(file, f);
}
} // end namespace wgt

#endif // __APPLE__
