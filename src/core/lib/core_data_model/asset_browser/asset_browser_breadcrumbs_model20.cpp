#include "asset_browser_breadcrumbs_model20.hpp"
#include "base_asset_object_item20.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_data_model/i_item_role.hpp"
#include "../abstract_item_model.hpp"
#include "../collection_model.hpp"

namespace wgt
{
namespace AssetBrowser20
{
//------------------------------------------------------------------------------
struct AssetBrowserBreadcrumbsModel::Implementation
{
	Implementation(AssetBrowserBreadcrumbsModel& self, IDefinitionManager& definitionManager);

	AssetBrowserBreadcrumbsModel& self_;
	IDefinitionManager& definitionManager_;
	CollectionModel breadcrumbs_;
	std::vector<ObjectHandle> collection_;
	std::string path_;

	void addBreadcrumb(const IAssetObjectItem* asset);

	void addSubItem(BaseBreadcrumbItem& parent, const IAssetObjectItem* asset);

	BaseBreadcrumbItem* getSubItem(const BaseBreadcrumbItem* parent, unsigned int index);
};

AssetBrowserBreadcrumbsModel::Implementation::Implementation(
AssetBrowserBreadcrumbsModel& self,
IDefinitionManager& definitionManager)
    : self_(self)
    , definitionManager_(definitionManager)
    , path_("")
{
	Collection col(collection_);
	breadcrumbs_.setSource(col);
}

void AssetBrowserBreadcrumbsModel::Implementation::addBreadcrumb(const IAssetObjectItem* asset)
{
	assert(asset != nullptr);

	auto breadcrumb = definitionManager_.create<BaseBreadcrumbItem>();
	breadcrumb->initialise(*asset);

	// Find the children of this asset, if any exist.
	size_t size = asset->size();
	for (size_t i = 0; i < size; ++i)
	{
		// Static cast used here, because we know that we will only ever get IAssetObjectItems from within an
		// IAssetObjectItem's children.
		auto childAssetItem = static_cast<IAssetObjectItem*>((*asset)[i]);
		addSubItem(*breadcrumb, childAssetItem);
	}

	collection_.push_back(breadcrumb);
}

void AssetBrowserBreadcrumbsModel::Implementation::addSubItem(BaseBreadcrumbItem& parent,
                                                              const IAssetObjectItem* asset)
{
	auto subBreadcrumb = definitionManager_.create<BaseBreadcrumbItem>();
	subBreadcrumb->initialise(*asset);
	parent.addSubItem(subBreadcrumb);
}

BaseBreadcrumbItem* AssetBrowserBreadcrumbsModel::Implementation::getSubItem(const BaseBreadcrumbItem* parent,
                                                                             unsigned int index)
{
	assert(parent != nullptr);

	const ObjectHandle provider = parent->getSubItem(index);
	return provider.getBase<BaseBreadcrumbItem>();
}

//------------------------------------------------------------------------------

AssetBrowserBreadcrumbsModel::AssetBrowserBreadcrumbsModel(IDefinitionManager& definitionManager)
    : IBreadcrumbsModel()
    , impl_(new Implementation(*this, definitionManager))
{
}

AssetBrowserBreadcrumbsModel::~AssetBrowserBreadcrumbsModel()
{
}

AbstractListModel* AssetBrowserBreadcrumbsModel::getBreadcrumbs() const
{
	return &impl_->breadcrumbs_;
}

const char* AssetBrowserBreadcrumbsModel::getPath() const
{
	return impl_->path_.c_str();
}

Variant AssetBrowserBreadcrumbsModel::getItemAtIndex(unsigned int index, int childIndex)
{
	if (index < static_cast<unsigned int>(size()))
	{
		auto item = impl_->breadcrumbs_.find(static_cast<int>(index));
		assert(item != nullptr);
		auto variant = item->getData(0, 0, ItemRole::valueId);
		if (variant.typeIs<ObjectHandle>())
		{
			ObjectHandle provider;
			if (variant.tryCast(provider))
			{
				auto breadcrumb = provider.getBase<BaseBreadcrumbItem>();
				if (breadcrumb != nullptr &&
				    childIndex < breadcrumb->getSubItems()->rowCount())
				{
					if (childIndex == -1)
					{
						// No child index specified, return the top-level breadcrumb
						return Variant(breadcrumb->getFullPath());
					}
					else
					{
						// Get the subitem at the specified childindex
						auto childcrumb = impl_->getSubItem(breadcrumb, childIndex);
						assert(childcrumb != nullptr);
						return Variant(childcrumb->getFullPath());
					}
				}
			}
		}
	}

	return Variant();
}

void AssetBrowserBreadcrumbsModel::setPath(const char* path)
{
	impl_->path_ = path;
}

void AssetBrowserBreadcrumbsModel::clear()
{
	impl_->breadcrumbs_.removeRows(0, impl_->breadcrumbs_.rowCount());
}

size_t AssetBrowserBreadcrumbsModel::size() const
{
	return static_cast<size_t>(impl_->breadcrumbs_.rowCount());
}

void AssetBrowserBreadcrumbsModel::generateBreadcrumbs(const AbstractItem* item, const AbstractTreeModel* model)
{
	if (item == nullptr || model == nullptr)
	{
		return;
	}

	// Setup the model for new breadcrumbs
	clear();
	auto variantPath = item->getData(0, 0, ItemRole::indexPathId);
	if (variantPath.canCast<std::string>())
	{
		setPath(variantPath.cast<std::string>().c_str());
	}

	// Build a list of breadcrumbs by walking up through the ancestor chain of the provided item.
	std::vector<const AbstractItem*> hierarchy;
	const AbstractItem* parent = item;

	while (parent != nullptr)
	{
		hierarchy.push_back(parent);
		auto index = model->index(parent);
		parent = index.parent_;
	}

	// Iterate the vector in reverse to generate breadcrumb items in the appropriate order
	std::vector<const AbstractItem*>::reverse_iterator itrRBegin = hierarchy.rbegin();
	std::vector<const AbstractItem*>::reverse_iterator itrREnd = hierarchy.rend();
	for (; itrRBegin != itrREnd; ++itrRBegin)
	{
		const auto assetItem = dynamic_cast<const IAssetObjectItem*>(*itrRBegin);
		if (assetItem != nullptr)
		{
			impl_->addBreadcrumb(assetItem);
		}
	}
}
} // end namespace AssetBrowser20
} // end namespace wgt
