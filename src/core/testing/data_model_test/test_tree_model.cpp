#include "test_tree_model.hpp"
#include "test_tree_item.hpp"

#include "core_data_model/i_item_role.hpp"
#include "core_data_model/common_data_roles.hpp"
#include "testing/data_model_test/test_data.hpp"

#include <unordered_map>
#include <vector>
#include <algorithm>
#include <array>
#include <string>
#include <memory>
#include <thread>

namespace wgt
{
struct TestTreeModel::Implementation
{
	Implementation(TestTreeModel& main);
	~Implementation();

	const std::vector<TestTreeItem*>& getSection(const TestTreeItem* parent);
	char* copyString(const std::string& s) const;
	void generateData(const TestTreeItem* parent, size_t level);

	TestTreeModel& main_;
	std::unordered_map<const TestTreeItem*, std::vector<TestTreeItem*>> data_;
	StringList dataSource_;
	std::string headerText_;
	std::string footerText_;
	Signal<TestTreeModel::DataSignature> preDataChanged_;
	Signal<TestTreeModel::DataSignature> postDataChanged_;

	Signal<AbstractTreeModel::MoveSignature> preRowsMoved_;
	Signal<AbstractTreeModel::MoveSignature> postRowsMoved_;

	static const size_t NUMBER_OF_GROUPS = 5;
	static const size_t NUMBER_OF_LEVELS = 5;
};

TestTreeModel::Implementation::Implementation(TestTreeModel& main)
    : main_(main), headerText_("Random Words"), footerText_("The End")
{
	generateData(nullptr, 0);
}

TestTreeModel::Implementation::~Implementation()
{
	for (auto itr = data_.begin(); itr != data_.end(); ++itr)
	{
		auto items = itr->second;
		size_t max = items.size();

		for (size_t i = 0; i < max; ++i)
		{
			delete items[i];
		}
	}

	data_.clear();
}

const std::vector<TestTreeItem*>& TestTreeModel::Implementation::getSection(const TestTreeItem* parent)
{
	auto itr = data_.find(parent);
	assert(itr != data_.end());
	return itr->second;
}

void TestTreeModel::Implementation::generateData(const TestTreeItem* parent, size_t level)
{
	for (size_t i = 0; i < NUMBER_OF_GROUPS; ++i)
	{
		auto&& dataString = dataSource_.next();
		TestTreeItem* item = new TestTreeItem(dataString, parent);
		auto& items = data_[parent];
		items.push_back(item);

		const int row = static_cast<int>(items.size() - 1);
		const auto& preModelDataChanged = preDataChanged_;
		const auto preData = [row, parent, &preModelDataChanged](int column, ItemRole::Id role, const Variant& value) {
			const TestTreeModel::ItemIndex index(row, parent);
			preModelDataChanged(index, column, role, value);
		};
		const auto preDataChanged = item->connectPreDataChanged(preData);

		const auto& postModelDataChanged = postDataChanged_;
		const auto postData = [row, parent, &postModelDataChanged](int column, ItemRole::Id role,
		                                                           const Variant& value) {
			const TestTreeModel::ItemIndex index(row, parent);
			postModelDataChanged(index, column, role, value);
		};
		const auto postDataChanged = item->connectPostDataChanged(postData);

		if (level < NUMBER_OF_LEVELS)
		{
			generateData(item, level + 1);
		}

		data_[item];
	}
}

TestTreeModel::TestTreeModel() : impl_(new Implementation(*this))
{
}

TestTreeModel::TestTreeModel(const TestTreeModel& rhs) : impl_(new Implementation(*this))
{
}

TestTreeModel::~TestTreeModel()
{
}

TestTreeModel& TestTreeModel::operator=(const TestTreeModel& rhs)
{
	if (this != &rhs)
	{
		impl_.reset(new Implementation(*this));
	}

	return *this;
}

AbstractItem* TestTreeModel::item(const ItemIndex& index) const
{
	auto temp = static_cast<const TestTreeItem*>(index.parent_);
	const auto& section = impl_->getSection(temp);
	assert(index.row_ < static_cast<int>(section.size()));
	return section[index.row_];
}

AbstractTreeModel::ItemIndex TestTreeModel::index(const AbstractItem* item) const
{
	auto temp = static_cast<const TestTreeItem*>(item);
	temp = static_cast<const TestTreeItem*>(temp->getParent());
	ItemIndex index(0, temp);

	auto items = impl_->getSection(temp);
	auto itr = std::find(items.begin(), items.end(), item);
	assert(itr != items.end());

	index.row_ = static_cast<int>(itr - items.begin());
	return index;
}

int TestTreeModel::rowCount(const AbstractItem* item) const
{
	auto temp = static_cast<const TestTreeItem*>(item);
	return static_cast<int>(impl_->getSection(temp).size());
}

int TestTreeModel::columnCount() const
{
	return 1;
}

Variant TestTreeModel::getData(int row, int column, ItemRole::Id roleId) const
{
	if ((column < 0) || (column >= this->columnCount()))
	{
		return Variant();
	}

	// Header/footer data only
	// This function is only for data associated with the model, not items
	auto role = static_cast<int>(roleId);
	if (role == headerTextRole::roleId_)
	{
		return impl_->headerText_.c_str();
	}
	else if (role == footerTextRole::roleId_)
	{
		return impl_->footerText_.c_str();
	}

	return AbstractTreeModel::getData(row, column, roleId);
}

bool TestTreeModel::setData(int row, int column, ItemRole::Id roleId, const Variant& data) /* override */
{
	if ((column < 0) || (column >= this->columnCount()))
	{
		return false;
	}

	// Header/footer data only
	// This function is only for data associated with the model, not items
	auto role = static_cast<int>(roleId);
	if (role == headerTextRole::roleId_)
	{
		impl_->preDataChanged_(row, column, roleId, data);
		const auto result = data.tryCast(impl_->headerText_);
		impl_->postDataChanged_(row, column, roleId, data);
		return result;
	}
	else if (role == footerTextRole::roleId_)
	{
		impl_->preDataChanged_(row, column, roleId, data);
		const auto result = data.tryCast(impl_->footerText_);
		impl_->postDataChanged_(row, column, roleId, data);
		return result;
	}

	return AbstractTreeModel::setData(row, column, roleId, data);
}

bool TestTreeModel::moveRows(const AbstractItem* sourceParent, int sourceRow, int count, const AbstractItem* destParent,
                             int destRow)
{
	if (destRow >= sourceRow && destRow <= sourceRow + count && sourceParent == destParent)
	{
		return false;
	}

	if (sourceParent == nullptr || destParent == nullptr)
	{
		return false;
	}

	int sourceCount = rowCount(sourceParent);
	int destCount = rowCount(destParent);

	if (sourceRow < 0 || sourceRow >= sourceCount || destRow < 0 || destRow >= destCount)
	{
		return false;
	}

	auto sourceItr = impl_->data_.find((TestTreeItem*)sourceParent);
	if (sourceItr == impl_->data_.end())
	{
		return false;
	}
	auto& sourceSection = sourceItr->second;

	auto destItr = impl_->data_.find((TestTreeItem*)destParent);
	if (destItr == impl_->data_.end())
	{
		return false;
	}
	auto& destSection = destItr->second;

	if (sourceParent != destParent)
	{
		TestTreeItem* parentCheck = (TestTreeItem*)destParent;
		while (parentCheck != nullptr)
		{
			for (int i = sourceRow; i < sourceRow + count; ++i)
			{
				if ((TestTreeItem*)sourceSection[i] == parentCheck)
				{
					return false;
				}
			}
			parentCheck = (TestTreeItem*)parentCheck->getParent();
		}
	}

	impl_->preRowsMoved_(index(sourceParent), sourceRow, sourceRow + count - 1, index(destParent), destRow);

	std::vector<TestTreeItem*> movedItems;

	for (int i = sourceRow; i < sourceRow + count; ++i)
	{
		movedItems.push_back(sourceSection[i]);
		movedItems.back()->setParent((TestTreeItem*)destParent);
	}

	const auto firstItr = sourceSection.cbegin() + sourceRow;
	const auto lastItr = firstItr + count;
	sourceSection.erase(firstItr, lastItr);
	if (destRow > sourceRow + count && sourceParent == destParent)
	{
		destSection.insert(destItr->second.begin() + destRow - count, movedItems.begin(), movedItems.end());
	}
	else
	{
		destSection.insert(destItr->second.begin() + destRow, movedItems.begin(), movedItems.end());
	}

	impl_->postRowsMoved_(index(sourceParent), sourceRow, sourceRow + count - 1, index(destParent), destRow);

	return true;
}

std::vector<std::string> TestTreeModel::roles() const
{
	std::vector<std::string> roles;
	roles.push_back(ItemRole::valueName);
	roles.push_back(ItemRole::headerTextName);
	roles.push_back(ItemRole::footerTextName);
	return roles;
}

Connection TestTreeModel::connectPreItemDataChanged(DataCallback callback) /* override */
{
	return impl_->preDataChanged_.connect(callback);
}

Connection TestTreeModel::connectPostItemDataChanged(DataCallback callback) /* override */
{
	return impl_->postDataChanged_.connect(callback);
}

Connection TestTreeModel::connectPreRowsMoved(MoveCallback callback) /* override */
{
	return impl_->preRowsMoved_.connect(callback);
}

Connection TestTreeModel::connectPostRowsMoved(MoveCallback callback) /* override */
{
	return impl_->postRowsMoved_.connect(callback);
}

} // end namespace wgt
