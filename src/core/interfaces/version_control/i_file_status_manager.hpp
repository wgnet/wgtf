#pragma once
/*
	This class manages the file status of any file you have open
	It should be a centralized control for all the files (so whenever there is an edit
	on a file it should update the file's status on this class)
*/

#include <functional>

namespace wgt
{
class Connection;

enum class FileEditState
{
	// The file has not changed since open.
	Clean,
	// The user has changed a read only file but decides to not track the state, the file is not savable.
	Scratched,
	// The file is in a changeable state and has been changed.
	Dirty
};

class IFileStatusManager
{
public:
	using FileChanged = void(const char * filename);
	using FileChangedCallback = std::function<FileChanged>;

	using PerFileInfoFn = void(const char * filename, const FileEditState& editState);
	using FileInfoCallback = std::function<PerFileInfoFn>;

	virtual ~IFileStatusManager() {}
	virtual FileEditState requestEdit(const char * filename) = 0;
	virtual FileEditState getFileState(const char * filename) const = 0;
	virtual void removeFileStatus(const char * filename) = 0;
	virtual Connection connectOnFileChanged(const FileChangedCallback & callback) = 0;
	virtual void notifyFileChange(const char * filename) = 0;
	virtual void enumerateFileStates(const FileInfoCallback & callback) = 0;
};
} // end namespace wgt