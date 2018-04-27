#pragma once

#include "core_data_model_cmds/interfaces/i_item_model_controller.hpp"
#include "core_dependency_system/i_interface.hpp"

namespace wgt
{
class ItemModelController : public Implements<IItemModelController>
{
public:
	ItemModelController();
	virtual ~ItemModelController();

	virtual bool setValue(AbstractItemModel& model, const AbstractItemModel::ItemIndex& index, size_t roleId,
	                      const Variant& data) override;
	virtual bool setModelData(AbstractItemModel& model, int row, int column, size_t roleId,
	                          const Variant& data) override;

	virtual bool insertRows(AbstractItemModel& model, int row, int count, const AbstractItem* pParent) override;
	virtual bool insertColumns(AbstractItemModel& model, int column, int count, const AbstractItem* pParent) override;

	virtual bool removeRows(AbstractItemModel& model, int row, int count, const AbstractItem* pParent) override;
	virtual bool removeColumns(AbstractItemModel& model, int column, int count, const AbstractItem* pParent) override;

	virtual bool moveRows(AbstractItemModel& model, const AbstractItem* sourceParent, int sourceRow, int count,
	                      const AbstractItem* destParent, int destRow) override;

	virtual bool insertItem(CollectionModel& model, const Variant& key) override;
	virtual bool insertItem(CollectionModel& model, const Variant& key, const Variant& data) override;
	virtual bool removeItem(CollectionModel& model, const Variant& key) override;

private:
	struct Impl;
	std::unique_ptr<Impl> impl_;
};

} // end namespace wgt
