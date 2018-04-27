#pragma once

#include "core_common/signal.hpp"

#include <string>

namespace wgt
{
class ObjectHandle;
class IDocument;

/**
* Interface that handles environment creation by editor apps.
*/
class IEditor
{
public:
	virtual ~IEditor() = default;

	/**
	* Creates environment with given name and make it current.
	* Param resourceName name of new resource.
	* Param resourceType type of the new resource
	* @return the newly created document. If the document was already opened, or failed to open, returns nullptr.
	*/
	virtual IDocument* create(const char* resourceName = nullptr, const char* resourceType = nullptr, bool activateNewFile = true) = 0;

	/**
	* Creates environment with given file name and make it current.
	* Param filePath path to file being opened.
	* @return the newly opened document. If the document was already opened, or failed to open, returns nullptr.
	*/
	virtual IDocument* open(const char* filePath, bool activateOpeningFile = true) = 0;

	/**
	* Closes current environment.
	*/
	virtual void close() = 0;

	/**
	* Removes indication that file has unsaved changes.
	*/
	virtual void save() = 0;

	/**
	* Removes indication that file has unsaved changes and change current title.
	*/
	virtual void saveAs(const char* filePath = nullptr) = 0;

	/**
	* Sets saved/unsaved status.
	*/
	virtual void setDirty(bool dirty) = 0;

	/**
	* Updates the edit state associated with an ObjectHandle.
	*/
	virtual void updateEditState(const ObjectHandle& handle) const = 0;

	/**
	* Updates the edit state associated with a file.
	*/
	virtual void updateEditState(const char* filePath) const = 0;

	/**
	* get active IDocument pointer, it may be nullptr
	*/
	virtual IDocument* getActiveDocument() const = 0;

};
}
