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

#include <stdio.h>
#include <stdlib.h>
#include <copyfile.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>

namespace wgt
{
using namespace FileAttributes;

namespace
{
IFileInfoPtr CreateFileInfo()
{
	return std::make_shared<FileInfo>(0, 0, 0, 0, "", "", None);
}

IFileInfoPtr CreateFileInfo(struct stat& fileStat, const char* path)
{
	unsigned int attributes = FileAttributes::None;

	if (S_ISDIR(fileStat.st_mode))
		attributes |= FileAttributes::Directory;

	if (S_ISREG(fileStat.st_mode))
		attributes |= FileAttribute::Normal;

	char absolutePath[MAX_PATH];
	realpath(path, absolutePath);

	if (access(absolutePath, W_OK) != 0)
		attributes |= FileAttribute::ReadOnly;

	size_t separator = std::string(absolutePath).rfind(FilePath::kDirectorySeparator);
	if (separator != std::string::npos && absolutePath[separator + 1] == '.')
		attributes |= FileAttribute::Hidden;

	return std::make_shared<FileInfo>(fileStat.st_size, fileStat.st_mtimespec.tv_sec, fileStat.st_mtimespec.tv_sec,
	                                  fileStat.st_atimespec.tv_sec, std::string(path), absolutePath,
	                                  static_cast<FileAttribute>(attributes));
}

} // namespace

struct FileSystem::Implementation
{
	Implementation(FileSystem& self)
		: self_(self)
	{
	}

	FileSystem& self_;
};

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
	return copyfile(path, new_path, nullptr, 0) == 0;
}

bool FileSystem::remove(const char* path)
{
	return ::remove(path) == 0;
}

bool FileSystem::exists(const char* path) const
{
	struct stat fileStat;
	return stat(path, &fileStat) == 0;
}

void FileSystem::enumerate(const char* dir, EnumerateCallback callback) const
{
	DIR* directory = opendir(dir);
	if (directory == nullptr)
		return;

	struct dirent* entry = nullptr;
	while ((entry = readdir(directory)) != nullptr)
	{
		std::string filePath = dir;
		filePath.reserve(filePath.size() + strlen(entry->d_name) + 1);
		filePath.append(1, FilePath::kDirectorySeparator).append(entry->d_name);

		struct stat fileStat;
		if (stat(filePath.c_str(), &fileStat) == 0)
		{
			if (callback(CreateFileInfo(fileStat, filePath.c_str())) == false)
				break;
		}
	}

	closedir(directory);
}

IFileSystem::FileType FileSystem::getFileType(const char* path) const
{
	struct stat fileStat;
	if (stat(path, &fileStat) < 0)
		return IFileSystem::NotFound;

	if (S_ISDIR(fileStat.st_mode))
		return IFileSystem::Directory;

	return IFileSystem::File;
}

IFileInfoPtr FileSystem::getFileInfo(const char* path) const
{
	struct stat fileStat;
	if (stat(path, &fileStat) < 0)
		return CreateFileInfo();

	return CreateFileInfo(fileStat, path);
}

bool FileSystem::move(const char* path, const char* new_path)
{
	return ::rename(path, new_path) == 0;
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
	bool success = mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == 0 || errno == EEXIST;

	if (!success)
	{
		return false;
	}

	pathChanged(path);
	return true;
}

bool FileSystem::removeDirectory(const char* path)
{
	if (rmdir(path) != 0)
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

Connection FileSystem::listenForChanges(PathChangedCallback& callback)
{
	// Not implemented;
	return Connection();
}

void FileSystem::pathChanged(const char* path) const
{
	// Not implemented;
}
} // end namespace wgt
