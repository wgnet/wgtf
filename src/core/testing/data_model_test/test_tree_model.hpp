#ifndef DATA_MODEL_TEST_TREE_MODEL_HPP
#define DATA_MODEL_TEST_TREE_MODEL_HPP

#include "core_data_model/abstract_item_model.hpp"
#include "core_data_model/i_item_role.hpp"
#include <memory>

namespace wgt
{
/**
* Model for an expandable tree
* There are multiple top-most items which all have a non-existent single root parent
* This root parent is not visible in the view and uses nullptr as its key
*/
class TestTreeModel : public AbstractTreeModel
{
public:
	TestTreeModel();
	TestTreeModel(int groups, int levels);
	virtual ~TestTreeModel();

	virtual AbstractItem* item(const ItemIndex& index) const override;
	virtual ItemIndex index(const AbstractItem* item) const override;
	virtual Variant getData(int row, int column, ItemRole::Id roleId) const override;
	virtual bool setData(int row, int column, ItemRole::Id roleId, const Variant& data) override;
	virtual void revert() override;

	virtual int rowCount(const AbstractItem* item) const override;
	virtual int columnCount() const override;

	virtual bool moveRows(const AbstractItem* sourceParent, int sourceRow, int count,
	                      const AbstractItem* destinationParent, int destinationChild) override;

	void iterateRoles(const std::function<void(const char*)>& iterFunc) const override;
	virtual std::vector<std::string> roles() const override;

	virtual Connection connectPreModelReset(TestTreeModel::VoidCallback callback) override;
	virtual Connection connectPostModelReset(TestTreeModel::VoidCallback callback) override;
	virtual Connection connectPreRowsMoved(TestTreeModel::MoveCallback callback) override;
	virtual Connection connectPostRowsMoved(TestTreeModel::MoveCallback callback) override;
	virtual Connection connectPreItemDataChanged(TestTreeModel::DataCallback callback) override;
	virtual Connection connectPostItemDataChanged(TestTreeModel::DataCallback callback) override;

	virtual MimeData mimeData(std::vector<AbstractItemModel::ItemIndex>& indices) override;
	void iterateMimeTypes(const std::function<void(const char*)>& iterFunc) const override;
	virtual std::vector<std::string> mimeTypes() const override;
	virtual bool canDropMimeData(const MimeData& mimeData, DropAction action, const AbstractItemModel::ItemIndex& index) const override;
	virtual bool dropMimeData(const MimeData& mimeData, DropAction action, const AbstractItemModel::ItemIndex& index) override;

private:
	struct Implementation;
	std::unique_ptr<Implementation> impl_;
};
} // end namespace wgt
#endif
