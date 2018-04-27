#include "custom_model_extension.hpp"

#include "core_common/assert.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_data_model/abstract_item_model.hpp"
#include "core_data_model/i_model_extension.hpp"
#include "core_qt_common/models/qt_item_model.hpp"
#include "core_qt_common/models/qt_abstract_item_model.hpp"
#include "core_qt_common/qt_framework_common.hpp"
#include "core_qt_common/qt_script_object.hpp"
#include "core_qt_common/controls/wg_item_view.hpp"

#include <QModelIndex>
#include <QAbstractItemModel>
#include <QAbstractProxyModel>

#include <array>
#include <unordered_map>
#include "core_object/managed_object.hpp"

namespace wgt
{
struct CustomModelExtension::Implementation : public IModelExtension::IExtensionData
{
	using ModelIndex = IModelExtension::ModelIndex;
	using Orientation = IModelExtension::Orientation;
	using HeaderSection = std::unordered_map<ItemRole::Id, Variant>;
	using HeaderData = std::unordered_map<int, HeaderSection>;

	struct QtModelIndexData : public IModelExtension::IModelIndexData
	{
		QtModelIndexData(const QModelIndex& index = QModelIndex())
		{
			index_ = index;
		}

		int row() const override
		{
			return index_.row();
		}

		int column() const override
		{
			return index_.column();
		}

		void* pointer() const override
		{
			return index_.internalPointer();
		}

		const void* model() const override
		{
			return index_.model();
		}

		QPersistentModelIndex index_;
	};

	Implementation(CustomModelExtension& self);

	Variant data(const ModelIndex& index, ItemRole::Id roleId) const override;
	bool setData(const ModelIndex& index, const Variant& value, ItemRole::Id roleId) override;
	void dataChanged(const ModelIndex& from, const ModelIndex& to, std::vector<ItemRole::Id> roleIds) const override;
	Variant headerData(int section, Orientation orientation, ItemRole::Id roleId) const override;
	bool setHeaderData(int section, Orientation orientation, const Variant& value, ItemRole::Id roleId) override;
	void headerDataChanged(Orientation orientation, int from, int to) const override;

	ModelIndex toModelIndex(const QModelIndex& index) const;
	QModelIndex toQModelIndex(const ModelIndex& index) const;

	CustomModelExtension& self_;
	ManagedObjectPtr extension_;
	QObject* view_;
	QVariant qmlExtension_;
	ModelIndexCreationFunction createIndexFunction_;
};

CustomModelExtension::Implementation::Implementation(CustomModelExtension& self)
    : self_(self), view_(nullptr), extension_(nullptr)
{
}

Variant CustomModelExtension::Implementation::data(const IModelExtension::ModelIndex& index, ItemRole::Id roleId) const
{
	QVariant result = self_.extensionData_->data(toQModelIndex(index), roleId);
	return self_.frameworkCommon().toVariant(result);
}

bool CustomModelExtension::Implementation::setData(const IModelExtension::ModelIndex& index, const Variant& value,
                                                   ItemRole::Id roleId)
{
	QVariant qValue = self_.frameworkCommon().toQVariant(value, view_);
	return self_.extensionData_->setData(toQModelIndex(index), qValue, roleId);
}

void CustomModelExtension::Implementation::dataChanged(const ModelIndex& from, const ModelIndex& to,
                                                       std::vector<ItemRole::Id> roleIds) const
{
	QVector<ItemRole::Id> roles;

	for (auto& id : roleIds)
	{
		roles.append(id);
	}

	self_.extensionData_->dataChanged(toQModelIndex(from), toQModelIndex(to), roles);
}

Variant CustomModelExtension::Implementation::headerData(int section, Orientation orientation,
                                                         ItemRole::Id roleId) const
{
	Qt::Orientation qtOrientation =
	orientation == Orientation::HORIZONTAL ? Qt::Orientation::Horizontal : Qt::Orientation::Vertical;

	QVariant result = self_.extensionData_->headerData(section, qtOrientation, roleId);
	return self_.frameworkCommon().toVariant(result);
}

bool CustomModelExtension::Implementation::setHeaderData(int section, Orientation orientation, const Variant& value,
                                                         ItemRole::Id roleId)
{
	Qt::Orientation qtOrientation =
	orientation == Orientation::HORIZONTAL ? Qt::Orientation::Horizontal : Qt::Orientation::Vertical;

	QVariant qValue = self_.frameworkCommon().toQVariant(value, view_);
	return self_.extensionData_->setHeaderData(section, qtOrientation, qValue, roleId);
}

void CustomModelExtension::Implementation::headerDataChanged(Orientation orientation, int from, int to) const
{
	Qt::Orientation qtOrientation =
	orientation == Orientation::HORIZONTAL ? Qt::Orientation::Horizontal : Qt::Orientation::Vertical;

	self_.extensionData_->headerDataChanged(qtOrientation, from, to);
}

CustomModelExtension::Implementation::ModelIndex CustomModelExtension::Implementation::toModelIndex(
const QModelIndex& index) const
{
	if (!index.isValid())
	{
		return ModelIndex();
	}

	return ModelIndex(std::make_shared<QtModelIndexData>(index));
}

QModelIndex CustomModelExtension::Implementation::toQModelIndex(const ModelIndex& index) const
{
	if (index.model() == nullptr || !index.isValid())
	{
		return QModelIndex();
	}

	auto indexData = index.data().get();
	TF_ASSERT(indexData);
	return static_cast<QtModelIndexData*>(indexData)->index_;
}

CustomModelExtension::CustomModelExtension() : impl_(new Implementation(*this))
{
}

CustomModelExtension::~CustomModelExtension()
{
	impl_.reset();
}

void CustomModelExtension::initialise(QtFrameworkCommon& frameworkCommon, QObject* view, ManagedObjectPtr&& object)
{
	setFramework(frameworkCommon);
	impl_->view_ = view;

	impl_->extension_ = std::move(object);
	auto handle = impl_->extension_->getHandleT<IModelExtension>();
	TF_ASSERT(handle != nullptr);
	impl_->qmlExtension_ = frameworkCommon.toQVariant(handle, view);
	handle->setExtensionData(impl_.get());

	for (auto& role: handle->roles())
	{
		roles_.push_back(role);
	}
}

QVariant CustomModelExtension::data(const QModelIndex& index, ItemRole::Id roleId) const
{
	auto handle = impl_->extension_->getHandleT<IModelExtension>();
	Variant result = handle->data(impl_->toModelIndex(index), roleId);
	return frameworkCommon().toQVariant(result, impl_->view_);
}

bool CustomModelExtension::setData(const QModelIndex& index, const QVariant& value, ItemRole::Id roleId)
{
	auto handle = impl_->extension_->getHandleT<IModelExtension>();
	return handle->setData(impl_->toModelIndex(index), frameworkCommon().toVariant(value), roleId);
}

QVariant CustomModelExtension::headerData(int section, Qt::Orientation orientation, ItemRole::Id roleId) const
{
	auto handle = impl_->extension_->getHandleT<IModelExtension>();
	IModelExtension::Orientation extensionOrientation = orientation == Qt::Orientation::Horizontal ?
	IModelExtension::Orientation::HORIZONTAL :
	IModelExtension::Orientation::VERTICAL;
	Variant result = handle->headerData(section, extensionOrientation, roleId);
	return frameworkCommon().toQVariant(result, impl_->view_);
}

bool CustomModelExtension::setHeaderData(int section, Qt::Orientation orientation, const QVariant& value,
                                         ItemRole::Id roleId)
{
	auto handle = impl_->extension_->getHandleT<IModelExtension>();
	IModelExtension::Orientation extensionOrientation = orientation == Qt::Orientation::Horizontal ?
	IModelExtension::Orientation::HORIZONTAL :
	IModelExtension::Orientation::VERTICAL;
	return handle->setHeaderData(section, extensionOrientation, frameworkCommon().toVariant(value), roleId);
}

void CustomModelExtension::setCreateIndexFunction(const ModelIndexCreationFunction& function)
{
	impl_->createIndexFunction_ = function;
}

void CustomModelExtension::onDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight,
                                         const QVector<int>& roles)
{
	std::vector<ItemRole::Id> roleIds;
	ItemRole::Id roleId;

	for (int role : roles)
	{
		if (!extensionData_->decodeRole(role, roleId))
		{
			continue;
		}

		roleIds.push_back(roleId);
	}

	auto handle = impl_->extension_->getHandleT<IModelExtension>();
	handle->onDataChanged(impl_->toModelIndex(topLeft), impl_->toModelIndex(bottomRight), roleIds);
}

void CustomModelExtension::onHeaderDataChanged(Qt::Orientation orientation, int first, int last)
{
	auto handle = impl_->extension_->getHandleT<IModelExtension>();
	IModelExtension::Orientation extensionOrientation = orientation == Qt::Orientation::Horizontal ?
	IModelExtension::Orientation::HORIZONTAL :
	IModelExtension::Orientation::VERTICAL;
	handle->onHeaderDataChanged(extensionOrientation, first, last);
}

void CustomModelExtension::onLayoutAboutToBeChanged(const QList<QPersistentModelIndex>& parents,
                                                    QAbstractItemModel::LayoutChangeHint hint)
{
	std::vector<Implementation::ModelIndex> extensionParents;
	IModelExtension::LayoutHint extensionHint;

	for (auto& index : parents)
	{
		extensionParents.push_back(impl_->toModelIndex(index));
	}

	switch (hint)
	{
	case QAbstractItemModel::LayoutChangeHint::HorizontalSortHint:
		extensionHint = IModelExtension::LayoutHint::HORIZONTAL_SORT;
		break;
	case QAbstractItemModel::LayoutChangeHint::VerticalSortHint:
		extensionHint = IModelExtension::LayoutHint::VERTICAL_SORT;
		break;
	case QAbstractItemModel::LayoutChangeHint::NoLayoutChangeHint:
		extensionHint = IModelExtension::LayoutHint::NO_SORT;
		break;
	}

	auto handle = impl_->extension_->getHandleT<IModelExtension>();
	handle->onLayoutAboutToBeChanged(extensionParents, extensionHint);
}

void CustomModelExtension::onLayoutChanged(const QList<QPersistentModelIndex>& parents,
                                           QAbstractItemModel::LayoutChangeHint hint)
{
	std::vector<Implementation::ModelIndex> extensionParents;
	IModelExtension::LayoutHint extensionHint;

	for (auto& index : parents)
	{
		extensionParents.push_back(impl_->toModelIndex(index));
	}

	switch (hint)
	{
	case QAbstractItemModel::LayoutChangeHint::HorizontalSortHint:
		extensionHint = IModelExtension::LayoutHint::HORIZONTAL_SORT;
		break;
	case QAbstractItemModel::LayoutChangeHint::VerticalSortHint:
		extensionHint = IModelExtension::LayoutHint::VERTICAL_SORT;
		break;
	case QAbstractItemModel::LayoutChangeHint::NoLayoutChangeHint:
		extensionHint = IModelExtension::LayoutHint::NO_SORT;
		break;
	}

	auto handle = impl_->extension_->getHandleT<IModelExtension>();
	handle->onLayoutChanged(extensionParents, extensionHint);
}

void CustomModelExtension::onRowsAboutToBeInserted(const QModelIndex& parent, int first, int last)
{
	auto handle = impl_->extension_->getHandleT<IModelExtension>();
	handle->onRowsAboutToBeInserted(impl_->toModelIndex(parent), first, last);
}

void CustomModelExtension::onRowsInserted(const QModelIndex& parent, int first, int last)
{
	auto handle = impl_->extension_->getHandleT<IModelExtension>();
	handle->onRowsInserted(impl_->toModelIndex(parent), first, last);
}

void CustomModelExtension::onRowsAboutToBeRemoved(const QModelIndex& parent, int first, int last)
{
	auto handle = impl_->extension_->getHandleT<IModelExtension>();
	handle->onRowsAboutToBeRemoved(impl_->toModelIndex(parent), first, last);
}

void CustomModelExtension::onRowsRemoved(const QModelIndex& parent, int first, int last)
{
	auto handle = impl_->extension_->getHandleT<IModelExtension>();
	handle->onRowsRemoved(impl_->toModelIndex(parent), first, last);
}

void CustomModelExtension::onRowsAboutToBeMoved(const QModelIndex& sourceParent, int sourceFirst, int sourceLast,
                                                const QModelIndex& destinationParent, int destinationRow)
{
	auto handle = impl_->extension_->getHandleT<IModelExtension>();
	handle->onRowsAboutToBeMoved(impl_->toModelIndex(sourceParent), sourceFirst, sourceLast,
	                                        impl_->toModelIndex(destinationParent), destinationRow);
}

void CustomModelExtension::onRowsMoved(const QModelIndex& sourceParent, int sourceFirst, int sourceLast,
                                       const QModelIndex& destinationParent, int destinationRow)
{
	auto handle = impl_->extension_->getHandleT<IModelExtension>();
	handle->onRowsMoved(impl_->toModelIndex(sourceParent), sourceFirst, sourceLast,
	                               impl_->toModelIndex(destinationParent), destinationRow);
}

void CustomModelExtension::onColumnsAboutToBeInserted(const QModelIndex& parent, int first, int last)
{
	auto handle = impl_->extension_->getHandleT<IModelExtension>();
	handle->onColumnsAboutToBeInserted(impl_->toModelIndex(parent), first, last);
}

void CustomModelExtension::onColumnsInserted(const QModelIndex& parent, int first, int last)
{
	auto handle = impl_->extension_->getHandleT<IModelExtension>();
	handle->onRowsAboutToBeInserted(impl_->toModelIndex(parent), first, last);
}

void CustomModelExtension::onColumnsAboutToBeRemoved(const QModelIndex& parent, int first, int last)
{
	auto handle = impl_->extension_->getHandleT<IModelExtension>();
	handle->onColumnsAboutToBeRemoved(impl_->toModelIndex(parent), first, last);
}

void CustomModelExtension::onColumnsRemoved(const QModelIndex& parent, int first, int last)
{
	auto handle = impl_->extension_->getHandleT<IModelExtension>();
	handle->onColumnsRemoved(impl_->toModelIndex(parent), first, last);
}

void CustomModelExtension::onColumnsAboutToBeMoved(const QModelIndex& sourceParent, int sourceFirst, int sourceLast,
                                                   const QModelIndex& destinationParent, int destinationColumn)
{
	auto handle = impl_->extension_->getHandleT<IModelExtension>();
	handle->onColumnsAboutToBeMoved(impl_->toModelIndex(sourceParent), sourceFirst, sourceLast,
	                                           impl_->toModelIndex(destinationParent), destinationColumn);
}

void CustomModelExtension::onColumnsMoved(const QModelIndex& sourceParent, int sourceFirst, int sourceLast,
                                          const QModelIndex& destinationParent, int destinationColumn)
{
	auto handle = impl_->extension_->getHandleT<IModelExtension>();
	handle->onColumnsMoved(impl_->toModelIndex(sourceParent), sourceFirst, sourceLast,
	                                  impl_->toModelIndex(destinationParent), destinationColumn);
}

QVariant CustomModelExtension::getExtension() const
{
	return impl_->qmlExtension_;
}
} // end namespace wgt