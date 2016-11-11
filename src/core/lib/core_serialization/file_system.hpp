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

#include <typeinfo>

#include <core_dependency_system/i_interface.hpp>
#include "core_serialization/i_file_system.hpp"

namespace wgt
{
#pragma warning(push)
#pragma warning(disable : 4275) // * non dll-interface class '*' used as base for dll-interface class '*'

class SERIALIZATION_DLL FileSystem : public Implements<IFileSystem>
{
public:
	virtual bool copy(const char* path, const char* new_path) override;
	virtual bool remove(const char* path) override;
	virtual bool exists(const char* path) const override;
	virtual void enumerate(const char* dir, EnumerateCallback callback) const override;
	virtual FileType getFileType(const char* path) const override;
	virtual IFileInfoPtr getFileInfo(const char* path) const override;
	virtual bool move(const char* path, const char* new_path) override;
	virtual IStreamPtr readFile(const char* path, std::ios::openmode mode) const override;
	virtual bool writeFile(const char* path, const void* data, size_t len, std::ios::openmode mode) override;
};

#pragma warning(pop)
} // end namespace wgt
#endif // FILE_SYSTEM_H_
