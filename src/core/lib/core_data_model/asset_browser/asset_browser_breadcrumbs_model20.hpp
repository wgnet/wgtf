#ifndef ASSET_BROWSER_BREADCRUMBS_MODEL20_HPP
#define ASSET_BROWSER_BREADCRUMBS_MODEL20_HPP

#include "core_data_model/i_breadcrumbs_model_new.hpp"

namespace wgt
{
class IDefinitionManager;
class AbstractTreeModel;

namespace AssetBrowser20
{
class IAssetObjectItem;
//------------------------------------------------------------------------------
// AssetBrowserBreadcrumbsModel
//
// An implementation of the IBreadcrumbsModel that is specific to the
// Asset Browser and manipulating IAssetObjectItems.
//------------------------------------------------------------------------------

class AssetBrowserBreadcrumbsModel : public IBreadcrumbsModel
{
public:
	//
	// Lifecycle
	//

	AssetBrowserBreadcrumbsModel(IDefinitionManager& definitionManager);

	virtual ~AssetBrowserBreadcrumbsModel() override;

	//
	// IBreadcrumbsModel Implementation
	//

	virtual AbstractListModel* getBreadcrumbs() const override;

	virtual const char* getPath() const override;

	virtual Variant getItemAtIndex(unsigned int index, int childIndex) override;

	virtual void clear() override;

	virtual size_t size() const override;

	//
	// Asset Browser Breadcrumbs Implementation
	//

	virtual void generateBreadcrumbs(const AbstractItem* item, const AbstractTreeModel* model);

	virtual void setPath(const char* path);

private:
	//
	// Private Implementation Principle
	//

	struct Implementation;
	std::unique_ptr<Implementation> impl_;
};
} // end namespace AssetBrowser20

} // end namespace wgt
#endif //ASSET_BROWSER_BREADCRUMBS_MODEL20_HPP
