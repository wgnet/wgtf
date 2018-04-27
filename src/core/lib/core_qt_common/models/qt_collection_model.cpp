#include "qt_collection_model.hpp"

#include "core_data_model/common_data_roles.hpp"
#include "helpers/qt_helpers.hpp"
#include "core_qt_common/qt_script_object.hpp"

#include <iterator>

namespace wgt
{
ITEMROLE(key)
ITEMROLE(keyType)

QtCollectionModel::QtCollectionModel(std::unique_ptr<CollectionModel>&& source)
    : QtItemModel<QtListModel>(*source.get()), model_(std::move(source))
{
}

QHash<int, QByteArray> QtCollectionModel::roleNames() const
{
	auto roleNames = QtListModel::roleNames();
	registerRole(ItemRole::valueName, roleNames);
	registerRole(ItemRole::keyName, roleNames);
	registerRole(ItemRole::valueTypeName, roleNames);
	registerRole(ItemRole::keyTypeName, roleNames);
	return roleNames;
}

const CollectionModel& QtCollectionModel::source() const
{
	return static_cast<const CollectionModel&>(QtItemModel::source());
}

CollectionModel& QtCollectionModel::source()
{
	return static_cast<CollectionModel&>(QtItemModel::source());
}

bool QtCollectionModel::insertRows(int row, int count, const QModelIndex& parent)
{
	auto& collectionModel = this->source();

	// Item already uses the Command System or the Command System is not available
	auto itemModelController = get<IItemModelController>();
	const auto useController = (!collectionModel.hasController() && (itemModelController != nullptr));
	if (!useController)
	{
		// Set property directly
		return collectionModel.insertRows(row, count);
	}

	// Queue with Command System, to register undo/redo data
	auto pParent = parent.isValid() ? reinterpret_cast<AbstractItem*>(parent.internalId()) : nullptr;
	return itemModelController->insertRows(collectionModel, row, count, pParent);
}

bool QtCollectionModel::removeRows(int row, int count, const QModelIndex& parent)
{
	auto& collectionModel = this->source();

	auto itemModelController = get<IItemModelController>();
	// Item already uses the Command System or the Command System is not available
	const auto useController = (!collectionModel.hasController() && (itemModelController != nullptr));
	if (!useController)
	{
		// Set property directly
		return collectionModel.removeRows(row, count);
	}

	// Queue with Command System, to register undo/redo data
	auto pParent = parent.isValid() ? reinterpret_cast<AbstractItem*>(parent.internalId()) : nullptr;
	return itemModelController->removeRows(collectionModel, row, count, pParent);
}

QVariant QtCollectionModel::value(const QVariant& key) const
{
	const auto& collectionModel = this->source();
	auto qtHelpers = get<IQtHelpers>();
	const auto variantKey = qtHelpers->toVariant(key);

	const auto pItem = collectionModel.find(variantKey);
	const auto value = pItem ? pItem->getData(0 /* row */, 0 /* column */, ValueRole::roleId_) : Variant();

	return qtHelpers->toQVariant(value, const_cast<QtCollectionModel*>(this));
}

QVariantList QtCollectionModel::values() const
{
	QVariantList items;
	const auto& collectionModel = this->source();
	auto qtHelpers = get<IQtHelpers>();
	auto count = collectionModel.rowCount();
	for (int i = 0; i < count; ++i)
	{
		const auto pItem = collectionModel.item(i);
		const auto value = pItem ? pItem->getData(0,0,ValueRole::roleId_) : Variant();
		items << qtHelpers->toQVariant(value, const_cast<QtCollectionModel*>(this));
	}
	return items;
}

bool QtCollectionModel::insertItem(const QVariant& key)
{
	auto& collectionModel = this->source();
	auto qtHelpers = get<IQtHelpers>();
	const auto variantKey = qtHelpers->toVariant(key);

	// Item already uses the Command System or the Command System is not available
	auto itemModelController = get<IItemModelController>();
	const auto useController = (!collectionModel.hasController() && (itemModelController != nullptr));

	if (!useController)
	{
		// Set property directly
		return collectionModel.insertItem(variantKey);
	}

	// Queue with Command System, to register undo/redo data
	const int count = 1;
	const AbstractItem* pParent = nullptr;
	return itemModelController->insertItem(collectionModel, variantKey);
}

bool QtCollectionModel::insertValue(const QVariant& key, const QVariant& value)
{
	auto& collectionModel = this->source();
	auto qtHelpers = get<IQtHelpers>();
	const auto variantKey = qtHelpers->toVariant(key);
	const auto variantValue = qtHelpers->toVariant(value);

	auto itemModelController = get<IItemModelController>();
	// Item already uses the Command System or the Command System is not available
	const auto useController = (!collectionModel.hasController() && (itemModelController != nullptr));
	if (!useController)
	{
		// Set property directly
		return collectionModel.insertItem(variantKey, variantValue);
	}

	// Queue with Command System, to register undo/redo data
	return itemModelController->insertItem(collectionModel, variantKey, variantValue);
}

bool QtCollectionModel::removeItem(const QVariant& key)
{
	auto& collectionModel = this->source();
	auto qtHelpers = get<IQtHelpers>();
	const auto variantKey = qtHelpers->toVariant(key);

	auto itemModelController = get<IItemModelController>();
	// Item already uses the Command System or the Command System is not available
	const auto useController = (!collectionModel.hasController() && (itemModelController != nullptr));
	if (!useController)
	{
		// Set property directly
		return collectionModel.removeItem(variantKey);
	}

	// Queue with Command System, to register undo/redo data
	return itemModelController->removeItem(collectionModel, variantKey);
}

bool QtCollectionModel::isMapping() const
{
	auto& collectionModel = this->source();
	return collectionModel.isMapping();
}
} // end namespace wgt
