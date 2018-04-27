#ifndef TEST_LIST_MODEL_HPP
#define TEST_LIST_MODEL_HPP

#include "core_data_model/abstract_item_model.hpp"

#include <memory>

namespace wgt
{
class TestListModel : public AbstractListModel
{
public:
	TestListModel(bool shortList = false);
	TestListModel(const TestListModel& rhs);
	virtual ~TestListModel();

	TestListModel& operator=(const TestListModel& rhs);

	virtual Variant getData(int row, int column, ItemRole::Id roleId) const override;
	virtual bool setData(int row, int column, ItemRole::Id roleId, const Variant& data) override;

	virtual AbstractItem* item(int row) const override;
	virtual int index(const AbstractItem* item) const override;

	virtual int rowCount() const override;
	virtual int columnCount() const override;

	virtual bool insertRows(int row, int count) override;
	virtual bool removeRows(int row, int count) override;
	virtual bool moveRows(int sourceRow, int count, int destinationRow) override;

	void iterateRoles(const std::function<void(const char*)>& iterFunc) const override;
	virtual std::vector<std::string> roles() const override;

	virtual Connection connectPreItemDataChanged(DataCallback callback) override;
	virtual Connection connectPostItemDataChanged(DataCallback callback) override;
	virtual Connection connectPreRowsMoved(MoveCallback callback) override;
	virtual Connection connectPostRowsMoved(MoveCallback callback) override;
	virtual Connection connectPreRowsInserted(RangeCallback callback) override;
	virtual Connection connectPostRowsInserted(RangeCallback callback) override;
	virtual Connection connectPreRowsRemoved(RangeCallback callback) override;
	virtual Connection connectPostRowsRemoved(RangeCallback callback) override;

private:
	struct Implementation;
	std::unique_ptr<Implementation> impl_;
};
} // end namespace wgt
#endif
