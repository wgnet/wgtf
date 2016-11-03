#include "asset_browser_breadcrumbs_model.hpp"
#include "i_asset_object_item.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_data_model/i_item_role.hpp"

namespace wgt
{
//------------------------------------------------------------------------------
struct AssetBrowserBreadcrumbsModel::Implementation
{
	Implementation(AssetBrowserBreadcrumbsModel& self, IDefinitionManager& definitionManager);

	AssetBrowserBreadcrumbsModel& self_;
	IDefinitionManager& definitionManager_;
	VariantList breadcrumbs_;
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

	breadcrumbs_.push_back(breadcrumb);
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

	const auto subItemVariant = parent->getSubItem(index);
	if (subItemVariant->typeIs<ObjectHandle>())
	{
		ObjectHandle provider;
		if (subItemVariant->tryCast(provider))
		{
			return provider.getBase<BaseBreadcrumbItem>();
		}
	}

	return nullptr;
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

IListModel* AssetBrowserBreadcrumbsModel::getBreadcrumbs() const
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
		auto variant = impl_->breadcrumbs_[index];
		if (variant.typeIs<ObjectHandle>())
		{
			ObjectHandle provider;
			if (variant.tryCast(provider))
			{
				auto breadcrumb = provider.getBase<BaseBreadcrumbItem>();
				if (breadcrumb != nullptr &&
				    childIndex < static_cast<int>(breadcrumb->getSubItems()->size()))
				{
					if (childIndex == -1)
					{
						// No child index specified, return the top-level breadcrumb
						return Variant(reinterpret_cast<uintptr_t>(breadcrumb->getItem()));
					}
					else
					{
						// Get the subitem at the specified childindex
						auto childcrumb = impl_->getSubItem(breadcrumb, childIndex);
						assert(childcrumb != nullptr);
						return Variant(reinterpret_cast<uintptr_t>(childcrumb->getItem()));
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
	impl_->breadcrumbs_.clear();
}

size_t AssetBrowserBreadcrumbsModel::size() const
{
	return impl_->breadcrumbs_.size();
}

void AssetBrowserBreadcrumbsModel::generateBreadcrumbs(const IItem* item, const ITreeModel* model)
{
	if (item == nullptr || model == nullptr)
	{
		return;
	}

	// Setup the model for new breadcrumbs
	clear();
	auto variantPath = item->getData(0, IndexPathRole::roleId_);
	if (variantPath.canCast<std::string>())
	{
		setPath(variantPath.cast<std::string>().c_str());
	}

	// Build a list of breadcrumbs by walking up through the ancestor chain of the provided item.
	std::vector<const IItem*> hierarchy;
	const IItem* parent = item;

	while (parent != nullptr)
	{
		hierarchy.push_back(parent);
		auto index = model->index(parent);
		parent = index.second;
	}

	// Iterate the vector in reverse to generate breadcrumb items in the appropriate order
	std::vector<const IItem*>::reverse_iterator itrRBegin = hierarchy.rbegin();
	std::vector<const IItem*>::reverse_iterator itrREnd = hierarchy.rend();
	for (; itrRBegin != itrREnd; ++itrRBegin)
	{
		const auto assetItem = dynamic_cast<const IAssetObjectItem*>(*itrRBegin);
		if (assetItem != nullptr)
		{
			impl_->addBreadcrumb(assetItem);
		}
	}
}
} // end namespace wgt
