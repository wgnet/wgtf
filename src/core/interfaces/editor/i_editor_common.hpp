#ifndef I_EDITOR_COMMON_HPP
#define I_EDITOR_COMMON_HPP

#include "core_environment_system/i_env_system.hpp"
#include "core_variant/variant.hpp"
#include <vector>
#include <unordered_map>

namespace wgt
{
typedef std::vector<struct ViewportDefinition> ViewportDefinitions;
typedef std::unordered_map<std::string, std::vector<Variant>> SettingsMap;
using IGameObjectPtr = std::shared_ptr<class IGameObject>;
using GameObjects = std::vector<IGameObjectPtr>;
class ObjectHandle;
using Asset = ObjectHandle;
class Connection;
class IGameRenderView;
class IWGSPerforceConnection;
enum ViewportLayout;
class IView;

struct AssetQueueItem
{
	std::string FileName;
	Variant ResourcesToLoad;
	bool MakeActive = false;
};

class IEditorCommon
{
public:
	virtual ~IEditorCommon() = default;
	virtual void assetsOpen(const std::vector<std::string>& assets, bool activateOpeningFile = true) = 0;
	virtual void assetLoaded(const std::string& assetPath, Asset& asset) = 0;
	virtual void assetUnloaded(Asset& asset) = 0;

	virtual void queueAssetLoad(const std::vector<AssetQueueItem>& assets) = 0;
	virtual AssetQueueItem makeAssetQueueItem(const char* filename) const = 0;

	virtual void createDefaultViewport() = 0;
	virtual void createDockedViewport() = 0;
	virtual EnvironmentId createViewportEnvironment(const char* name = "", const char* id = "") = 0;
	virtual IGameRenderView* getGameRenderView() const = 0;
	virtual IGameRenderView* getGameRenderView(const EnvironmentId& id) const = 0;
	virtual IView* getDockedViewport() const = 0;

	virtual ViewportLayout getGameRenderViewLayout() const = 0;
	virtual void setGameRenderViewLayout(ViewportLayout layout, const ViewportDefinitions& viewports) = 0;

	virtual void updateView() = 0;
	virtual void setCurrentViewportName(const char* name) = 0;
	virtual void setCurrentViewportDirty(bool dirty) = 0;
	virtual bool currentViewportDirty() const = 0;
	virtual bool hasDirtyViewport() const = 0;
	virtual void addToRecentFiles(const char* filePath) = 0;
	virtual void releaseCurrentViewportFocus() = 0;
	virtual bool getViewportCursorPlacementPosition(Vector3& position) const = 0;

	virtual void setSelectedSceneNodeIds(const std::vector<uintptr_t>& ids) = 0;
	virtual const std::vector<uintptr_t>& getSelectedSceneNodeIds() const = 0;

	virtual const GameObjects& getViewportSelection() const = 0;

	using loadAsset = void(const char* assetPath, bool activateLoadingFile, const Variant& loadedResources);
	using LoadAssetCallback = std::function<loadAsset>;
	virtual Connection connectLoadAsset(LoadAssetCallback callback) = 0;

	using getAssetLoadedResources = void(const char* assetPath, Variant& resources);
	using GetAssetLoadedResourcesCallback = std::function<getAssetLoadedResources>;
	virtual Connection connectGetAssetLoadedResources(GetAssetLoadedResourcesCallback callback) = 0;

	using onViewportSelectionChanged = void(const GameObjects& viewportSelection);
	using ViewportSelectionChangedCallback = std::function<onViewportSelectionChanged>;
	virtual Connection connectOnViewportSelectionChanged(ViewportSelectionChangedCallback callback) = 0;

	using onViewportSelectionDeleted = void();
	using ViewportSelectionDeletedCallback = std::function<onViewportSelectionDeleted>;
	virtual Connection connectOnViewportSelectionDeleted(ViewportSelectionDeletedCallback callback) = 0;

	using CollectObjectsSig = void(GameObjects& objects);
	using CollectObjectsCallback = std::function<CollectObjectsSig>;
	virtual void setActiveObjects(CollectObjectsCallback collectObjects, bool allowAsyncGet = false) = 0;
	virtual const GameObjects& getActiveObjects() = 0;
	virtual void removeFromActiveObjects(const GameObjects& objects) = 0;
	virtual void addToActiveObjects(const GameObjects& objects) = 0;

	using onActiveObjectsChanged = void();
	using ActiveObjectsChangedCallback = std::function<onActiveObjectsChanged>;
	virtual Connection connectOnActiveObjectsChanged(ActiveObjectsChangedCallback callback) = 0;

	using assetLoad = void(const std::string&, Asset&);
	using AssetLoadCallback = std::function<assetLoad>;
	virtual Connection connectOnAssetLoaded(AssetLoadCallback) = 0;
	using assetUnload = void(Asset&);
	using AssetUnloadCallback = std::function<assetUnload>;
	virtual Connection connectOnAssetUnloaded(AssetUnloadCallback) = 0;
	using postAssetsLoad = void();
	using PostAssetsLoadCallback = std::function<postAssetsLoad>;
	virtual Connection connectPostAssetLoaded(PostAssetsLoadCallback callback) = 0;

	virtual void setViewportContextMenu(const char* contextMenuId) = 0;
	virtual std::string getViewportContextMenu() const = 0;
	virtual bool isViewportContextMenuActive() const = 0;
};
}
#endif // I_EDITOR_COMMON_HPP