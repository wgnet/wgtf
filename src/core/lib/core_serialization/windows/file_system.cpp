//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  file_system.cpp
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "core_common/ngt_windows.hpp"
#include "core_serialization/file_system.hpp"
#include "core_serialization/file_info.hpp"
#include "core_serialization/file_data_stream.hpp"
#include "core_logging/logging.hpp"
#include "core_string_utils/string_utils.hpp"
#include "core_common/signal.hpp"

#include <array>
#include <direct.h>
#include <mutex>
#include <io.h>

namespace wgt
{
using namespace FileAttributes;

struct FileSystem::Implementation
{
	Implementation(FileSystem& self)
		: self_(self)
	{
	}

	FileSystem& self_;
	Signal<IFileSystem::PathChangedSignature> pathChangedSignal_;
	std::mutex pathChangedMutex_;
};

HANDLE FindFirstFileExAHelper(const char* path, WIN32_FIND_DATAA& findData)
{
	auto handle = FindFirstFileExA(path, FindExInfoBasic, &findData, FindExSearchNameMatch, NULL, 0);
	if (handle == INVALID_HANDLE_VALUE)
	{
		std::string findPath(path);
		std::replace(findPath.begin(), findPath.end(), FilePath::kDirectorySeparator, FilePath::kAltDirectorySeparator);
		if (findPath.length() == 1 && findPath[0] == FilePath::kAltDirectorySeparator)
		{
			// Retrieve the current working directory for path "\"
			char fullPath[MAX_PATH];
			GetFullPathNameA(findPath.c_str(), MAX_PATH, fullPath, NULL);
			findPath = fullPath;
		}
		// Cannot end in trailing backslash
		// https://msdn.microsoft.com/en-us/library/windows/desktop/aa364419(v=vs.85).aspx
		if (*--findPath.end() == FilePath::kAltDirectorySeparator)
			findPath.erase(--findPath.end());
		handle = FindFirstFileExA(findPath.c_str(), FindExInfoBasic, &findData, FindExSearchNameMatch, NULL, 0);
	}
	return handle;
}

FileSystem::FileSystem()
	: impl_(new Implementation(*this))
{
}

FileSystem::~FileSystem()
{
	impl_.reset();
}

bool FileSystem::copy(const char* path, const char* new_path)
{
	return CopyFileA(path, new_path, TRUE) != FALSE;
}

bool FileSystem::remove(const char* path)
{
	return DeleteFileA(path) != FALSE;
}
bool FileSystem::exists(const char* path) const
{
	// _access_s(path, 0) doesn't work for all files (i.e. c:\hyberfil.sys)
	// https://msdn.microsoft.com/en-us/library/a2xs1dts.aspx
	// PathFileExists doesn't work for all files (i.e. c:\hyberfil.sys)
	// https://msdn.microsoft.com/en-us/library/windows/desktop/bb773584(v=vs.85).aspx
	WIN32_FIND_DATAA findData;
	auto handle = FindFirstFileExAHelper(path, findData);
	if (handle != INVALID_HANDLE_VALUE)
		FindClose(handle);
	return handle != INVALID_HANDLE_VALUE;
}
void FileSystem::enumerate(const char* dir, EnumerateCallback callback) const
{
	WIN32_FIND_DATAA findData;
	std::string filter(dir);
	std::replace(filter.begin(), filter.end(), FilePath::kDirectorySeparator, FilePath::kAltDirectorySeparator);
	// Add a wildcard to the search parameter if it doesn't exist
	if (filter.find('*', filter.length() - 1) == std::string::npos)
	{
		// Add a directory separator if it doesn't exist,
		if (filter.find(FilePath::kAltDirectorySeparator, filter.length() - 1) == std::string::npos)
		{
			filter += FilePath::kAltDirectorySeparator;
		}
		filter += '*';
	}
	std::string directory(filter);
	directory.erase(--directory.end());

	// Using the ANSI version of FindFirstFileEx, the name is limited to MAX_PATH characters.
	// To extend this limit to approximately 32,000 wide characters, use the Unicode version of the function and prepend
	// "\\?\" to the path.
	auto handle = FindFirstFileExA(filter.c_str(), FindExInfoBasic, &findData, FindExSearchNameMatch, NULL, 0);
	if (handle != INVALID_HANDLE_VALUE)
	{
		do
		{
			std::string path = directory + findData.cFileName;
			char absolutePath[MAX_PATH];
			GetFullPathNameA(path.c_str(), MAX_PATH, absolutePath, NULL);

			auto info = std::shared_ptr<FileInfo>(new FileInfo(
			uint64_t(findData.nFileSizeHigh) << 32 | findData.nFileSizeLow,
			uint64_t(findData.ftCreationTime.dwHighDateTime) << 32 | findData.ftCreationTime.dwLowDateTime,
			uint64_t(findData.ftLastWriteTime.dwHighDateTime) << 32 | findData.ftLastWriteTime.dwLowDateTime,
			uint64_t(findData.ftLastAccessTime.dwHighDateTime) << 32 | findData.ftLastAccessTime.dwLowDateTime, path,
			absolutePath, static_cast<FileAttribute>(findData.dwFileAttributes)));

			if (!callback(std::move(info)))
				break;
		} while (FindNextFileA(handle, &findData));
		FindClose(handle);
	}
}
IFileSystem::FileType FileSystem::getFileType(const char* path) const
{
	// GetFileAttributes doesn't work for all files (i.e. c:\hyberfil.sys)
	// https://msdn.microsoft.com/en-us/library/windows/desktop/aa364944(v=vs.85).aspx
	auto fileType = IFileSystem::NotFound;
	WIN32_FIND_DATAA findData;
	auto handle = FindFirstFileExAHelper(path, findData);
	if (handle != INVALID_HANDLE_VALUE)
	{
		if (findData.dwFileAttributes & FileAttributes::Directory)
			fileType = IFileSystem::Directory;
		else if (findData.dwFileAttributes & Compressed)
			fileType = IFileSystem::Archive;
		else
			fileType = IFileSystem::File;
		FindClose(handle);
	}
	return fileType;
}
IFileInfoPtr FileSystem::getFileInfo(const char* path) const
{
	// GetFileAttributes doesn't work for all files (i.e. c:\hyberfil.sys)
	// https://msdn.microsoft.com/en-us/library/windows/desktop/aa364944(v=vs.85).aspx
	WIN32_FIND_DATAA findData;
	auto handle = FindFirstFileExAHelper(path, findData);

	char absolutePath[MAX_PATH] = {};
	GetFullPathNameA(path, MAX_PATH, absolutePath, 0);

	if (handle != INVALID_HANDLE_VALUE)
	{
		auto info = std::shared_ptr<FileInfo>(
		new FileInfo(uint64_t(findData.nFileSizeHigh) << 32 | findData.nFileSizeLow,
		             uint64_t(findData.ftCreationTime.dwHighDateTime) << 32 | findData.ftCreationTime.dwLowDateTime,
		             uint64_t(findData.ftLastWriteTime.dwHighDateTime) << 32 | findData.ftLastWriteTime.dwLowDateTime,
		             uint64_t(findData.ftLastAccessTime.dwHighDateTime) << 32 | findData.ftLastAccessTime.dwLowDateTime,
		             path, absolutePath, static_cast<FileAttribute>(findData.dwFileAttributes)));
		FindClose(handle);
		return info;
	}
	return std::shared_ptr<FileInfo>(new FileInfo(0, 0, 0, 0, "", "", None));
}
bool FileSystem::move(const char* path, const char* new_path)
{
	return MoveFileA(path, new_path) != FALSE;
}
IFileSystem::IStreamPtr FileSystem::readFile(const char* path, std::ios::openmode mode) const
{
	return IStreamPtr(new FileDataStream(path, mode));
}
bool FileSystem::writeFile(const char* path, const void* data, size_t len, std::ios::openmode mode)
{
	std::fstream stream(path, mode);
	if (!stream.bad())
	{
		stream.write(reinterpret_cast<const char*>(data), len);
		stream.close();
		return true;
	}
	return false;
}

bool FileSystem::createDirectory(const char* path)
{
	if (_mkdir(path) != 0 && errno != EEXIST)
	{
		return false;
	}

	pathChanged(path);
	return true;
}

bool FileSystem::removeDirectory(const char* path)
{
	// this could potentially fail if the OS races on the state of the file.
	// rmdir used to be used here but there were problems that would show up
	// on some test cases. So far ::RemoveDirectory has been reliable in this
	// regard.
	//
	// SHFileOperation was also tried, this also appeared reliable but
	// but was much slower. If the tests fail again then we'll have to use
	// SHFileOperation, perhaps setting different flags will make it faster

	if (::RemoveDirectoryA(path) != TRUE || GetLastError() == ERROR_DIR_NOT_EMPTY)
	{
		return false;
	}

	pathChanged(path);
	return true;
}

bool FileSystem::makeWritable(const char* path)
{
	struct _stat buf;

	if (_stat(path, &buf) != 0)
	{
		return false;
	}

	_chmod(path, buf.st_mode | _S_IWRITE);
	pathChanged(path);
	return true;
}

void FileSystem::invalidateFileInfo(const char* path)
{
	pathChanged(path);
}

Connection FileSystem::listenForChanges(PathChangedCallback& callback)
{
	std::lock_guard<std::mutex> lock(impl_->pathChangedMutex_);
	return impl_->pathChangedSignal_.connect(callback);
}

void FileSystem::pathChanged(const char* path) const
{
	IFileInfoPtr info = getFileInfo(path);
	std::lock_guard<std::mutex> lock(impl_->pathChangedMutex_);
	impl_->pathChangedSignal_(path, info);
}
} // end namespace wgt
