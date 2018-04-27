//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  file_system.hpp
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#ifndef FILE_SYSTEM_H_
#define FILE_SYSTEM_H_

#pragma once

#include "serialization_dll.hpp"
#include "core_dependency_system/i_interface.hpp"
#include "core_serialization/i_file_system.hpp"

#include <typeinfo>
#include <memory>

namespace wgt
{
#pragma warning(push)
#pragma warning(disable : 4275) // * non dll-interface class '*' used as base for dll-interface class '*'

class SERIALIZATION_DLL FileSystem : public Implements<IFileSystem>
{
public:
	FileSystem();
	virtual ~FileSystem();

	virtual bool copy(const char* path, const char* new_path) override;
	virtual bool remove(const char* path) override;
	virtual bool exists(const char* path) const override;
	virtual void enumerate(const char* dir, EnumerateCallback callback) const override;
	virtual FileType getFileType(const char* path) const override;
	virtual IFileInfoPtr getFileInfo(const char* path) const override;
	virtual bool move(const char* path, const char* new_path) override;
	virtual IStreamPtr readFile(const char* path, std::ios::openmode mode) const override;
	virtual bool writeFile(const char* path, const void* data, size_t len, std::ios::openmode mode) override;
	virtual bool createDirectory(const char* path) override;
	virtual bool removeDirectory(const char* path) override;
	virtual bool makeWritable(const char* path) override;
	virtual void invalidateFileInfo(const char* path) override;
	virtual Connection listenForChanges(PathChangedCallback& callback) override;

protected:
	void pathChanged(const char* path) const;

private:
	struct Implementation;
	friend Implementation;
	std::unique_ptr<Implementation> impl_;
};

#pragma warning(pop)
} // end namespace wgt
#endif // FILE_SYSTEM_H_
