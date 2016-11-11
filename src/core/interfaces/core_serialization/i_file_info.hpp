//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  i_file_info.hpp
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifndef I_FILE_INFO_H_
#define I_FILE_INFO_H_

#pragma once

namespace wgt
{
namespace FileAttributes
{
enum FileAttribute
{
	None = 0x00000000,
	ReadOnly = 0x00000001,
	Hidden = 0x00000002,
	System = 0x00000004,
	Directory = 0x00000010,
	Archive = 0x00000020,
	Device = 0x00000040,
	Normal = 0x00000080,
	Temporary = 0x00000100,
	SparseFile = 0x00000200,
	ReparsePoint = 0x00000400,
	Compressed = 0x00000800,
	Offline = 0x00001000,
	NotContentIndexed = 0x00002000,
	Encrypted = 0x00004000,
	IntegrityStream = 0x00008000,
	Virtual = 0x00010000,
	NoScrubData = 0x00020000,

	AppendOnly = 0x00040000,
	NoDump = 0x00080000,
	ExtentFormat = 0x00100000,
	Immutable = 0x00200000,
	DataJournaling = 0x00400000,
	SecureDeletion = 0x00800000,
	NoTailMerging = 0x01000000,
	Undeletable = 0x02000000,
	NoAtimeUpdates = 0x04000000,
	SynchronousDirectory = 0x08000000,
	Updates = 0x10000000,
	SynchronousUpdates = 0x20000000,
	TopOfDirectoryHierarchy = 0x40000000
};
};

class IFileInfo
{
public:
	virtual ~IFileInfo()
	{
	}
	virtual bool isDirectory() const = 0;
	virtual bool isReadOnly() const = 0;
	virtual bool isHidden() const = 0;
	virtual bool isDots() const = 0;

	virtual uint64_t size() const = 0;
	virtual uint64_t created() const = 0;
	virtual uint64_t modified() const = 0;
	virtual uint64_t accessed() const = 0;

	virtual const char* extension() const = 0;
	virtual const char* name() const = 0;
	virtual const char* fullPath() const = 0;

	virtual const FileAttributes::FileAttribute attributes() const = 0;
};
typedef std::shared_ptr<IFileInfo> IFileInfoPtr;
} // end namespace wgt
#endif // I_FILE_INFO_H_
