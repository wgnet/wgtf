#ifndef ASSET_BROWSER_BREADCRUMBS_MODEL_HPP
#define ASSET_BROWSER_BREADCRUMBS_MODEL_HPP

#include "core_data_model/i_breadcrumbs_model.hpp"

namespace wgt
{
class IDefinitionManager;
class IAssetObjectItem;

//------------------------------------------------------------------------------
// AssetBrowserBreadcrumbsModelOld
//
// An implementation of the IBreadcrumbsModelOld that is specific to the
// Asset Browser and manipulating IAssetObjectItemsOld.
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

	virtual IListModel* getBreadcrumbs() const override;

	virtual const char* getPath() const override;

	virtual Variant getItemAtIndex(unsigned int index, int childIndex) override;

	virtual void clear() override;

	virtual size_t size() const override;

	//
	// Asset Browser Breadcrumbs Implementation
	//

	virtual void generateBreadcrumbs(const IItem* item, const ITreeModel* model);

	virtual void setPath(const char* path);

private:
	//
	// Private Implementation Principle
	//

	struct Implementation;
	std::unique_ptr<Implementation> impl_;
};
} // end namespace wgt
#endif //ASSET_BROWSER_BREADCRUMBS_MODEL_HPP
