#pragma once

#include "core_common/signal.hpp"
#include "core_environment_system/i_env_system.hpp"
#include <string>


namespace wgt
{
class ObjectHandle;
/*
* Interface that encapsulates a single document workspace
*/
class IDocument
{
public:
	virtual ~IDocument() = default;
	virtual const EnvironmentId& getEnvironmentId() const = 0;
	virtual void setFilePath(const std::string& path) = 0;
	virtual const std::string& getFilePath() const = 0;
	virtual void setFileType(const std::string& path) = 0;
	virtual const std::string& getFileType() const = 0;
	virtual bool canDrop(const std::string& filePath) const = 0;
	virtual void setAcceptedDropAssetExtension(const std::vector<std::string>& assetExtensions) = 0;

	virtual void addLoadedFile(const std::string& path) = 0;
	virtual const std::vector<std::string>& getLoadedFiles() const = 0;
	
	virtual void triggerActivate() = 0;
	virtual void triggerSave() = 0;
	virtual void dropAssets(const std::vector<std::string>& assetPaths) = 0;

	virtual void updateEditState(const ObjectHandle& handle) const = 0;
	virtual void updateEditState(const char* filePath) const = 0;

	using DocumentCallbackSig = void(IDocument*);
	using DocumentCallback = std::function<DocumentCallbackSig>;
	virtual Connection connectSave(DocumentCallback callback) = 0;
	virtual Connection connectActivate(DocumentCallback callback) = 0;

	using ObjectHandleCallbackSignature = void(const ObjectHandle&);
	virtual Connection connectUpdateEditState(std::function<ObjectHandleCallbackSignature> callback) = 0;

	using FileCallbackSignature = void(const char* filePath);
	virtual Connection connectUpdateEditState(std::function<FileCallbackSignature> callback) = 0;

	using AssetDroppedSig = void(const std::vector<std::string>&);
	using AssetDroppedCallback = std::function<AssetDroppedSig>;
	virtual Connection connectAssetDropped(AssetDroppedCallback callback) { return Connection(); }

	using CloseSignature = void(void);
	using CloseCallback = std::function<CloseSignature>;
	virtual Connection connectClose(const CloseCallback& closeCallback) = 0;
};

}
