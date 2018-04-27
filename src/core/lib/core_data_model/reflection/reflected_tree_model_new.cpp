#include "reflected_tree_model_new.hpp"

#include "reflected_object_item_new.hpp"

#include "core_common/assert.hpp"
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

namespace ReflectedTreeModelDetails
{
static const std::string s_RolesArr[] = {
	ItemRole::valueName, ItemRole::valueTypeName, ItemRole::keyName, ItemRole::keyTypeName, ItemRole::isCollectionName,
	ItemRole::elementValueTypeName, ItemRole::elementKeyTypeName, ItemRole::readOnlyName, ItemRole::enabledName,
	ItemRole::multipleValuesName,
	// DEPRECATED
	EnumModelRole::roleName_, DefinitionRole::roleName_, DefinitionModelRole::roleName_, ObjectRole::roleName_,
	RootObjectRole::roleName_, MinValueRole::roleName_, MaxValueRole::roleName_, StepSizeRole::roleName_,
	DecimalsRole::roleName_, IndexPathRole::roleName_, UrlIsAssetBrowserRole::roleName_, UrlDialogTitleRole::roleName_,
	UrlDialogDefaultFolderRole::roleName_, UrlDialogNameFiltersRole::roleName_,
	UrlDialogSelectedNameFilterRole::roleName_, IsReadOnlyRole::roleName_, IsEnumRole::roleName_,
	IsThumbnailRole::roleName_, IsSliderRole::roleName_, IsColorRole::roleName_, IsActionRole::roleName_,
	IsUrlRole::roleName_, DescriptionRole::roleName_, ThumbnailRole::roleName_
};
static const std::vector<std::string> s_RolesVec(&s_RolesArr[0],
                                                 &s_RolesArr[0] + std::extent<decltype(s_RolesArr)>::value);
} // end namespace ReflectedTreeModelDetails

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

class ReflectedTreeModelNew::Implementation : public Depends<IDefinitionManager>
{
public:
	Implementation(const ReflectedTreeModelNew& model, const ObjectHandle& object);
	~Implementation();

	ReflectedTreeItemNew* getItemInternal(size_t index, const ReflectedTreeItemNew* parent) const;
	size_t getIndexInternal(const ReflectedTreeItemNew* item) const;
	int getChildCountInternal(const ReflectedTreeItemNew* item) const;

	std::unique_ptr<ReflectedTreeItemNew> rootItem_;

	std::shared_ptr<PropertyAccessorListener> listener_;
};

ReflectedTreeModelNew::Implementation::Implementation(const ReflectedTreeModelNew& model, const ObjectHandle& object)
    : rootItem_(new ReflectedObjectItemNew(object, model)),
      listener_(new ReflectedTreeModelPropertyListener(*rootItem_.get()))
{
	get<IDefinitionManager>()->registerPropertyAccessorListener(listener_);
}

ReflectedTreeModelNew::Implementation::~Implementation()
{
	get<IDefinitionManager>()->deregisterPropertyAccessorListener(listener_);
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

	TF_ASSERT(item == rootItem_.get());
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

ReflectedTreeModelNew::ReflectedTreeModelNew(const ObjectHandle& object)
    : AbstractTreeModel(), impl_(new Implementation(*this, object))
{
}

ReflectedTreeModelNew::~ReflectedTreeModelNew()
{
}

AbstractItem* ReflectedTreeModelNew::item(const AbstractTreeModel::ItemIndex& index) const /* override */
{
	auto reflectedParent = dynamic_cast<const ReflectedTreeItemNew*>(index.parent_);
	TF_ASSERT(index.parent_ == nullptr || reflectedParent != nullptr);

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
	TF_ASSERT(reflectedItem != nullptr);

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
		TF_ASSERT(parentIndex.parent_ == nullptr || parent != nullptr);
	}
	return AbstractTreeModel::ItemIndex(row, parent);
}

int ReflectedTreeModelNew::rowCount(const AbstractItem* item) const /* override */
{
	auto reflectedItem = static_cast<const ReflectedTreeItemNew*>(item);
	TF_ASSERT(item == nullptr || reflectedItem != nullptr);

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
	TF_ASSERT(item == nullptr || reflectedItem != nullptr);

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

//------------------------------------------------------------------------------
void ReflectedTreeModelNew::iterateRoles(const std::function<void(const char*)>& iterFunc) const
{
	for (auto&& role : ReflectedTreeModelDetails::s_RolesVec)
	{
		iterFunc(role.c_str());
	}
}

//------------------------------------------------------------------------------
std::vector<std::string> ReflectedTreeModelNew::roles() const
{
	return ReflectedTreeModelDetails::s_RolesVec;
}

bool ReflectedTreeModelNew::hasController() const /* override */
{
	return true;
}

#define CONNECT_METHOD(method, connection, callbackType)                                              \
	Connection ReflectedTreeModelNew::method(AbstractTreeModel::callbackType callback) /* override */ \
	{                                                                                                 \
		return connection.connect(callback);                                                          \
	}

CONNECT_METHOD(connectPreItemDataChanged, preItemDataChanged_, DataCallback)
CONNECT_METHOD(connectPostItemDataChanged, postItemDataChanged_, DataCallback)

CONNECT_METHOD(connectPreRowsInserted, preRowsInserted_, RangeCallback)
CONNECT_METHOD(connectPostRowsInserted, postRowsInserted_, RangeCallback)

CONNECT_METHOD(connectPreRowsRemoved, preRowsRemoved_, RangeCallback)
CONNECT_METHOD(connectPostRowsRemoved, postRowsRemoved_, RangeCallback)

#undef CONNECT_METHOD
} // end namespace wgt
