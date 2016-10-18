#ifndef I_BREADCRUMBS_MODEL_HPP
#define I_BREADCRUMBS_MODEL_HPP

#include "core_reflection/reflected_object.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_data_model/asset_browser/base_asset_object_item20.hpp"
#include "i_item_role.hpp"
#include "i_tree_model.hpp"
#include "variant_list.hpp"
#include <string>
#include "collection_model.hpp"

namespace wgt
{
class AbstractListModel;
class AbstractItem;
namespace AssetBrowser20
{
//------------------------------------------------------------------------------
// BaseBreadcrumbItem
//
// Represents a single breadcrumb item in its most basic form. May be
// overridden for customized functionality within your IBreadcrumbsModel.
//------------------------------------------------------------------------------

class BaseBreadcrumbItem
{
	DECLARE_REFLECTED

public:
	BaseBreadcrumbItem()
	    : fullPath_("")
	    , displayValue_("")
	{
		Collection col(collection_);
		subItems_.setSource(col);
	}

	virtual ~BaseBreadcrumbItem()
	{
	}

	virtual void initialise(const AbstractItem& item)
	{
		item_ = &item;
		assert(item_ != nullptr);

		auto displayValue = item_->getData(0, 0, ItemRole::displayId);
		displayValue.tryCast(displayValue_);

		auto fullPathVar = item_->getData(0, 0, ItemRole::indexPathId);
		fullPathVar.tryCast<std::string>(fullPath_);
	}

	virtual void addSubItem(const ObjectHandle& breadcrumb)
	{
		collection_.push_back(breadcrumb);
	}

	virtual const ObjectHandle getSubItem(unsigned int index) const
	{
		auto item = subItems_.item(static_cast<int>(index));
		return subItems_.find(item);
		return nullptr;
	}

	virtual const std::string& getDisplayValue() const
	{
		return displayValue_;
	}
	virtual const std::string& getFullPath() const
	{
		return fullPath_;
	}
	virtual const AbstractItem* getItem() const
	{
		return item_;
	}
	virtual const AbstractListModel* getSubItems() const
	{
		return &subItems_;
	}

protected:
	std::string fullPath_;
	std::string displayValue_;
	CollectionModel subItems_;
	std::vector<ObjectHandle> collection_;
	const AbstractItem* item_;
};

//------------------------------------------------------------------------------
// IBreadcrumbsModel
//
// Represents the data model for the breadcrumbs control.
//------------------------------------------------------------------------------

class IBreadcrumbsModel
{
	DECLARE_REFLECTED

public:
	//-------------------------------------
	// Lifecycle
	//-------------------------------------

	IBreadcrumbsModel()
	{
		// Just a temporary implementation until type definition registration
		// allows abstract classes.
	}

	virtual ~IBreadcrumbsModel()
	{
	}

	//-------------------------------------
	// Data Model Accessors
	//-------------------------------------

	// Returns the breadcrumb items
	// Expected: IListModel of IBreadcrumbItem objects
	virtual AbstractListModel* getBreadcrumbs() const
	{
		return nullptr;
	}

	// Returns the full path of the breadcrumbs in a format that may be presented as a string
	virtual const char* getPath() const
	{
		return nullptr;
	}

	// Returns the IItem tied to the breadcrumb at the specified index
	virtual Variant getItemAtIndex(unsigned int index, int childIndex)
	{
		return Variant();
	}

	// Clears the current set of breadcrumbs
	virtual void clear()
	{
	}

	// Returns the number of top-level breadcrumbs stored in the model
	virtual size_t size() const
	{
		return 0;
	}
};
} // end namespace AssetBrowser20

} // end namespace wgt
#endif //I_BREADCRUMBS_MODEL_HPP
