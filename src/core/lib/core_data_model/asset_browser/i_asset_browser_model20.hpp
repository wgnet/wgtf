#ifndef I_ASSET_BROWSER_MODEL20_HPP
#define I_ASSET_BROWSER_MODEL20_HPP

#include "core_reflection/reflected_object.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_variant/variant.hpp"

namespace wgt
{
class AbstractListModel;
class AbstractTreeModel;
class IActiveFiltersModel;
class AbstractItem;
class IComponentContext;
class IListModel;
class IValueChangeNotifier;

typedef std::vector<std::string> AssetPaths;

namespace AssetBrowser20
{
class IAssetObjectItem;
//------------------------------------------------------------------------------
// IAssetBrowserModel
//
// Represents the data model for the WGAssetBrowser control. Developers must
// implement their own versions depending on the type of system their tools
// use to access assets be it a loose file or resource pack file system.
//------------------------------------------------------------------------------
class IAssetBrowserModel
{
	DECLARE_REFLECTED

public:
	//-------------------------------------
	// Lifecycle
	//-------------------------------------
	virtual ~IAssetBrowserModel()
	{
	}

	//-------------------------------------
	// QML Exposed Methods/Properties
	//-------------------------------------

	// Retrieves the contents of the selected folder
	// Expected: IListModel
	virtual AbstractListModel* getFolderContents() const
	{
		return nullptr;
	}

	// Retrieves the model for the folder tree view
	// Expected: ITreeModel
	virtual AbstractTreeModel* getFolderTreeModel() const
	{
		return nullptr;
	}

	// Retrieve the active filters model
	// Expected: IActiveFiltersModel
	virtual IActiveFiltersModel* getActiveFiltersModel() const
	{
		return nullptr;
	}

	// QML-exposed method to locate the asset with the designated path and wrap it in a Variant for QML usage
	// Expected: Variant (intptr_t of an IItem*)
	virtual Variant findAssetWithPath(std::string path)
	{
		return Variant();
	}

	//-------------------------------------
	// Public Methods
	//-------------------------------------

	// Initializes the data model.
	virtual void initialise(IComponentContext& /*contextManager*/, IDefinitionManager& /*definitionManager*/)
	{
	}

	// Finalises the data model.
	virtual void finalise(){};

	// Populate the folderContents list given the specified item
	virtual void populateFolderContents(AbstractItem* item) = 0;

	// Asset path accessor/mutator
	virtual const AssetPaths& assetPaths() const = 0;

	// Retrieves the data at the specified index
	virtual IAssetObjectItem* getFolderContentsAtIndex(const int& index) const = 0;

	// Retrieves the model for custom content filters
	// Expected: IListModel
	// Note: Feature likely to be removed once active filters and more robust filter handling is introduced
	virtual IListModel* getCustomContentFilters() const = 0;
	virtual IValueChangeNotifier* customContentFilterIndexNotifier() const = 0;
	virtual const int& currentCustomContentFilter() const = 0;
	virtual void currentCustomContentFilter(const int& index) = 0;
	virtual void setFolderContentsFilter(const std::string filter) = 0;

	// Internal/native method to retrieve an IAssetObjectItem in the ITreeModel by its path
	virtual IAssetObjectItem* getAssetAtPath(const char* path, IAssetObjectItem* parent = nullptr) const = 0;

	// Get the icon size
	virtual const int& getIconSize() const = 0;

	// Set the icon size
	virtual void setIconSize(const int&) = 0;
};
} // end namespace AssetBrowser20

} // end namespace wgt
#endif // I_ASSET_BROWSER_MODEL20_HPP
