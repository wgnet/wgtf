#include "reflected_tree_model_new.hpp"
#include "reflected_object_item_new.hpp"

#include "core_data_model/i_item_role.hpp"
#include "core_data_model/common_data_roles.hpp"

#include "core_reflection/property_accessor_listener.hpp"

#include <vector>

namespace wgt
{
ITEMROLE(key)
ITEMROLE(keyType)
ITEMROLE(isCollection)
ITEMROLE(elementValueType)
ITEMROLE(elementKeyType)
ITEMROLE(readOnly)
ITEMROLE(enabled)
ITEMROLE(multipleValues)
namespace
{
/**
 *	Refreshes UI when properties are changed via undo/redo or by a control
 *	on a different panel.
 */
class ReflectedTreeModelPropertyListener : public PropertyAccessorListener
{
public:
	ReflectedTreeModelPropertyListener(ReflectedTreeItemNew& item);

	// PropertyAccessorListener
	void preSetValue(const PropertyAccessor& accessor, const Variant& value) override;
	void postSetValue(const PropertyAccessor& accessor, const Variant& value) override;

	void preInsert(const PropertyAccessor& accessor, size_t index, size_t count) override;
	void postInserted(const PropertyAccessor& accessor, size_t index, size_t count) override;

	void preErase(const PropertyAccessor& accessor, size_t index, size_t count) override;
	void postErased(const PropertyAccessor& accessor, size_t index, size_t count) override;

private:
	ReflectedTreeItemNew& rootItem_;
};

ReflectedTreeModelPropertyListener::ReflectedTreeModelPropertyListener(ReflectedTreeItemNew& item) : rootItem_(item)
{
}

void ReflectedTreeModelPropertyListener::preSetValue(const PropertyAccessor& accessor, const Variant& value)
{
	rootItem_.preSetValue(accessor, value);
}

void ReflectedTreeModelPropertyListener::postSetValue(const PropertyAccessor& accessor, const Variant& value)
{
	rootItem_.postSetValue(accessor, value);
}

void ReflectedTreeModelPropertyListener::preInsert(const PropertyAccessor& accessor, size_t index, size_t count)
{
	rootItem_.preInsert(accessor, index, count);
}

void ReflectedTreeModelPropertyListener::postInserted(const PropertyAccessor& accessor, size_t index, size_t count)
{
	rootItem_.postInserted(accessor, index, count);
}

void ReflectedTreeModelPropertyListener::preErase(const PropertyAccessor& accessor, size_t index, size_t count)
{
	rootItem_.preErase(accessor, index, count);
}

void ReflectedTreeModelPropertyListener::postErased(const PropertyAccessor& accessor, size_t index, size_t count)
{
	rootItem_.postErased(accessor, index, count);
}

} // namespace

class ReflectedTreeModelNew::Implementation
{
public:
	Implementation(const ReflectedTreeModelNew& model, IComponentContext& contextManager, const ObjectHandle& object);
	~Implementation();

	ReflectedTreeItemNew* getItemInternal(size_t index, const ReflectedTreeItemNew* parent) const;
	size_t getIndexInternal(const ReflectedTreeItemNew* item) const;
	int getChildCountInternal(const ReflectedTreeItemNew* item) const;

	std::unique_ptr<ReflectedTreeItemNew> rootItem_;

	DIRef<IDefinitionManager> definitionManager_;
	std::shared_ptr<PropertyAccessorListener> listener_;
};

ReflectedTreeModelNew::Implementation::Implementation(const ReflectedTreeModelNew& model,
                                                      IComponentContext& contextManager, const ObjectHandle& object)
    : rootItem_(new ReflectedObjectItemNew(contextManager, object, model)), definitionManager_(contextManager),
      listener_(new ReflectedTreeModelPropertyListener(*rootItem_.get()))
{
	if (definitionManager_ != nullptr)
	{
		definitionManager_->registerPropertyAccessorListener(listener_);
	}
}

ReflectedTreeModelNew::Implementation::~Implementation()
{
	if (definitionManager_ != nullptr)
	{
		definitionManager_->deregisterPropertyAccessorListener(listener_);
	}
}

ReflectedTreeItemNew* ReflectedTreeModelNew::Implementation::getItemInternal(size_t index,
                                                                             const ReflectedTreeItemNew* parent) const
{
	if (parent == nullptr)
	{
		return index == 0 ? rootItem_.get() : nullptr;
	}

	return parent->getChild(index);
}

size_t ReflectedTreeModelNew::Implementation::getIndexInternal(const ReflectedTreeItemNew* item) const
{
	if (item == nullptr)
	{
		return 0;
	}

	auto parent = item->getParent();
	if (parent != nullptr)
	{
		return item->getIndex();
	}

	assert(item == rootItem_.get());
	return 0;
}

/**
 *	Get number of children in tree, at one level, under the given item.
 *	Including hidden items.
 *	E.g.
 *	> group1
 *	>> group2 <- hidden
 *	>>> item1 - count
 *	>>> item2 - count
 *	>>> item3 - count
 *	>> item4 - count
 *	getChildCountInternal( group1 ) == 2 // group2 and item4
 */
int ReflectedTreeModelNew::Implementation::getChildCountInternal(const ReflectedTreeItemNew* item) const
{
	if (item == nullptr)
	{
		return 1;
	}

	return item->rowCount();
}

ReflectedTreeModelNew::ReflectedTreeModelNew(IComponentContext& contextManager, const ObjectHandle& object)
    : AbstractTreeModel(), impl_(new Implementation(*this, contextManager, object))
{
}

ReflectedTreeModelNew::~ReflectedTreeModelNew()
{
}

AbstractItem* ReflectedTreeModelNew::item(const AbstractTreeModel::ItemIndex& index) const /* override */
{
	auto reflectedParent = dynamic_cast<const ReflectedTreeItemNew*>(index.parent_);
	assert(index.parent_ == nullptr || reflectedParent != nullptr);

	auto itemCount = impl_->getChildCountInternal(reflectedParent);
	auto row = index.row_;
	for (int i = 0; i < itemCount; ++i)
	{
		auto item = impl_->getItemInternal(i, reflectedParent);
		if (item != nullptr && item->isInPlace())
		{
			auto childItemCount = rowCount(item);
			if (row < childItemCount)
			{
				const AbstractTreeModel::ItemIndex parentIndex(row, item);
				return this->item(parentIndex);
			}
			row -= childItemCount;
		}
		else
		{
			if (row == 0)
			{
				return item;
			}
			--row;
		}
	}
	return nullptr;
}

AbstractTreeModel::ItemIndex ReflectedTreeModelNew::index(const AbstractItem* item) const /* override */
{
	if (item == nullptr)
	{
		return AbstractTreeModel::ItemIndex();
	}

	auto reflectedItem = dynamic_cast<const ReflectedTreeItemNew*>(item);
	assert(reflectedItem != nullptr);

	int row = 0;
	auto parent = reflectedItem->getParent();
	auto indexInternal = impl_->getIndexInternal(reflectedItem);
	for (size_t i = 0; i < indexInternal; ++i)
	{
		auto itemInternal = impl_->getItemInternal(i, parent);
		row += itemInternal != nullptr && itemInternal->isInPlace() ? this->rowCount(itemInternal) : 1;
	}

	if (parent != nullptr && parent->isInPlace())
	{
		auto parentIndex = this->index(parent);
		row += parentIndex.row_;
		parent = const_cast<ReflectedTreeItemNew*>(dynamic_cast<const ReflectedTreeItemNew*>(parentIndex.parent_));
		assert(parentIndex.parent_ == nullptr || parent != nullptr);
	}
	return AbstractTreeModel::ItemIndex(row, parent);
}

int ReflectedTreeModelNew::rowCount(const AbstractItem* item) const /* override */
{
	auto reflectedItem = static_cast<const ReflectedTreeItemNew*>(item);
	assert(item == nullptr || reflectedItem != nullptr);

	int count = 0;
	auto childCount = impl_->getChildCountInternal(reflectedItem);
	for (int i = 0; i < childCount; ++i)
	{
		auto childItem = impl_->getItemInternal(i, reflectedItem);
		count += childItem != nullptr && childItem->isInPlace() ? this->rowCount(childItem) : 1;
	}
	return count;
}

int ReflectedTreeModelNew::columnCount() const /* override */
{
	return 1;
}

bool ReflectedTreeModelNew::hasChildren(const AbstractItem* item) const /* override */
{
	auto reflectedItem = static_cast<const ReflectedTreeItemNew*>(item);
	assert(item == nullptr || reflectedItem != nullptr);

	auto childCount = impl_->getChildCountInternal(reflectedItem);
	for (int i = 0; i < childCount; ++i)
	{
		auto childItem = impl_->getItemInternal(i, reflectedItem);
		if (childItem != nullptr && childItem->isInPlace())
		{
			if (!hasChildren(childItem))
			{
				continue;
			}
		}
		return true;
	}
	return false;
}

std::vector<std::string> ReflectedTreeModelNew::roles() const
{
	std::vector<std::string> roles;
	roles.push_back(ItemRole::valueName);
	roles.push_back(ItemRole::valueTypeName);
	roles.push_back(ItemRole::keyName);
	roles.push_back(ItemRole::keyTypeName);
	roles.push_back(ItemRole::isCollectionName);
	roles.push_back(ItemRole::elementValueTypeName);
	roles.push_back(ItemRole::elementKeyTypeName);
	roles.push_back(ItemRole::readOnlyName);
	roles.push_back(ItemRole::enabledName);
	roles.push_back(ItemRole::multipleValuesName);
	// DEPRECATED
	roles.push_back(EnumModelRole::roleName_);
	roles.push_back(DefinitionRole::roleName_);
	roles.push_back(DefinitionModelRole::roleName_);
	roles.push_back(ObjectRole::roleName_);
	roles.push_back(RootObjectRole::roleName_);
	roles.push_back(MinValueRole::roleName_);
	roles.push_back(MaxValueRole::roleName_);
	roles.push_back(StepSizeRole::roleName_);
	roles.push_back(DecimalsRole::roleName_);
	roles.push_back(IndexPathRole::roleName_);
	roles.push_back(UrlIsAssetBrowserRole::roleName_);
	roles.push_back(UrlDialogTitleRole::roleName_);
	roles.push_back(UrlDialogDefaultFolderRole::roleName_);
	roles.push_back(UrlDialogNameFiltersRole::roleName_);
	roles.push_back(UrlDialogSelectedNameFilterRole::roleName_);
	roles.push_back(IsReadOnlyRole::roleName_);
	roles.push_back(IsEnumRole::roleName_);
	roles.push_back(IsThumbnailRole::roleName_);
	roles.push_back(IsSliderRole::roleName_);
	roles.push_back(IsColorRole::roleName_);
	roles.push_back(IsActionRole::roleName_);
	roles.push_back(IsUrlRole::roleName_);
	roles.push_back(DescriptionRole::roleName_);
	roles.push_back(ThumbnailRole::roleName_);
	return roles;
}

bool ReflectedTreeModelNew::hasController() const /* override */
{
	return true;
}

#define CONNECT_METHOD(method, connection, callbackType)                                   \
	\
Connection                                                                          \
	ReflectedTreeModelNew::method(AbstractTreeModel::callbackType callback) /* override */ \
	\
{                                                                                   \
		return connection.connect(callback);                                               \
	\
}

CONNECT_METHOD(connectPreItemDataChanged, preItemDataChanged_, DataCallback)
CONNECT_METHOD(connectPostItemDataChanged, postItemDataChanged_, DataCallback)

CONNECT_METHOD(connectPreRowsInserted, preRowsInserted_, RangeCallback)
CONNECT_METHOD(connectPostRowsInserted, postRowsInserted_, RangeCallback)

CONNECT_METHOD(connectPreRowsRemoved, preRowsRemoved_, RangeCallback)
CONNECT_METHOD(connectPostRowsRemoved, postRowsRemoved_, RangeCallback)

#undef CONNECT_METHOD
} // end namespace wgt
