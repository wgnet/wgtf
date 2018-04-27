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
ITEMROLE(fullPath)
ITEMROLE(indexPath)

namespace TestTreeModelDetails
{
const char* itemMimeKey = "application/test-tree-model-item";
static const size_t DEFAULT_GROUPS = 5;
static const size_t DEFAULT_LEVELS = 5;

static const std::string s_RolesArr[] =
{
	ItemRole::valueName,
	ItemRole::fullPathName,
	ItemRole::indexPathName,
	ItemRole::tooltipName,
	ItemRole::headerTextName, 
	ItemRole::footerTextName 
};

static const std::vector<std::string> s_RolesVec(&s_RolesArr[0],
	&s_RolesArr[0] + std::extent<decltype(s_RolesArr)>::value);

static const std::string s_MimeTypesArr[] = { TestTreeModelDetails::itemMimeKey };
static const std::vector<std::string> s_MimeTypesVec(&s_MimeTypesArr[0],
	&s_MimeTypesArr[0] + std::extent<decltype(s_MimeTypesArr)>::value);

} // end namespace TestTableModelDetails

struct TestTreeModel::Implementation
{
	Implementation(TestTreeModel& main, int groups, int levels);
	~Implementation();

	const std::vector<TestTreeItem*>& getSection(const TestTreeItem* parent);
	char* copyString(const std::string& s) const;
	void generateData(const TestTreeItem* parent, size_t level);

	TestTreeModel& main_;
	std::unordered_map<const TestTreeItem*, std::vector<TestTreeItem*>> data_;
	std::vector<AbstractItemModel::ItemIndex> mimeData_;
	StringList dataSource_;
	std::string headerText_;
	std::string footerText_;
	int groups_ = 0;
	int levels_ = 0;

	Signal<TestTreeModel::DataSignature> preDataChanged_;
	Signal<TestTreeModel::DataSignature> postDataChanged_;
	Signal<TestTreeModel::VoidSignature> preModelReset_;
	Signal<TestTreeModel::VoidSignature> postModelReset_;
	Signal<TestTreeModel::MoveSignature> preRowsMoved_;
	Signal<TestTreeModel::MoveSignature> postRowsMoved_;
};

TestTreeModel::Implementation::Implementation(TestTreeModel& main, int groups, int levels)
    : main_(main)
	, headerText_("Random Words")
	, footerText_("The End")
	, groups_(groups)
	, levels_(levels)
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
	for (int i = 0; i < groups_; ++i)
	{
		auto&& dataString = dataSource_.next();
		TestTreeItem* item = new TestTreeItem(dataString, parent);
		auto& items = data_[parent];
		items.push_back(item);

		const int row = static_cast<int>(items.size() - 1);
		const auto& preModelDataChanged = preDataChanged_;
		const auto preData = [row, parent, &preModelDataChanged](int column, ItemRole::Id role, const Variant& value) 
		{
			const TestTreeModel::ItemIndex index(row, parent);
			preModelDataChanged(index, column, role, value);
		};
		const auto preDataChanged = item->connectPreDataChanged(preData);

		const auto& postModelDataChanged = postDataChanged_;
		const auto postData = [row, parent, &postModelDataChanged](int column, ItemRole::Id role,
		                                                           const Variant& value) 
		{
			const TestTreeModel::ItemIndex index(row, parent);
			postModelDataChanged(index, column, role, value);
		};
		const auto postDataChanged = item->connectPostDataChanged(postData);

		if (static_cast<int>(level) < levels_)
		{
			generateData(item, level + 1);
		}

		data_[item];
	}
}

TestTreeModel::TestTreeModel(int groups, int levels)
	: impl_(new Implementation(*this, groups, levels))
{
}

TestTreeModel::TestTreeModel() 
	: TestTreeModel(TestTreeModelDetails::DEFAULT_GROUPS, TestTreeModelDetails::DEFAULT_LEVELS)
{
}

TestTreeModel::~TestTreeModel()
{
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
	temp = temp->getParent();
	ItemIndex index(0, temp);

	auto items = impl_->getSection(temp);
	auto itr = std::find(items.begin(), items.end(), item);
	assert(itr != items.end());

	index.row_ = static_cast<int>(itr - items.begin());
	return index;
}

int TestTreeModel::rowCount(const AbstractItem* item) const
{
	auto treeItem = static_cast<const TestTreeItem*>(item);
	return static_cast<int>(impl_->getSection(treeItem).size());
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
	if (roleId == headerTextRole::roleId_)
	{
		return impl_->headerText_.c_str();
	}
	else if (roleId == footerTextRole::roleId_)
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

bool TestTreeModel::moveRows(const AbstractItem* sourceParent, 
							 int sourceRow, 
							 int count, 
							 const AbstractItem* destParent,
                             int destRow)
{
	if (destRow >= sourceRow && destRow <= sourceRow + count && sourceParent == destParent)
	{
		return false;
	}

	// Moving between the top-most parent entries not supported
	if (sourceParent == nullptr && destParent == nullptr)
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

	std::vector<TestTreeItem*> movedItems;
	for (int i = sourceRow; i < sourceRow + count; ++i)
	{
		movedItems.push_back(sourceSection[i]);
		movedItems.back()->setParent((TestTreeItem*)destParent);
	}

	// Indexes for this model are relative to their parent,
	// QAbstractItemModel::beginMoveRows requires this not 
	// to be the case, so have to reset the model
	if (destParent == sourceParent)
	{
		impl_->preRowsMoved_(index(sourceParent), sourceRow, sourceRow + count - 1, index(destParent), destRow);
	}
	else
	{
		impl_->preModelReset_();
	}

	const auto firstItr = sourceSection.cbegin() + sourceRow;
	const auto lastItr = firstItr + count;
	sourceSection.erase(firstItr, lastItr);
	if (destRow > sourceRow + count && sourceParent == destParent)
	{
		destSection.insert(destSection.begin() + destRow - count, movedItems.begin(), movedItems.end());
	}
	else
	{
		destSection.insert(destSection.begin() + destRow, movedItems.begin(), movedItems.end());
	}

	if (destParent == sourceParent)
	{
		impl_->postRowsMoved_(index(sourceParent), sourceRow, sourceRow + count - 1, index(destParent), destRow);
	}
	else
	{
		impl_->postModelReset_();
	}
	
	return true;
}

void TestTreeModel::iterateRoles(const std::function<void(const char*)>& iterFunc) const
{
	for (auto&& role : TestTreeModelDetails::s_RolesVec)
	{
		iterFunc(role.c_str());
	}
}

std::vector<std::string> TestTreeModel::roles() const
{
	return TestTreeModelDetails::s_RolesVec;
}

Connection TestTreeModel::connectPreItemDataChanged(DataCallback callback)
{
	return impl_->preDataChanged_.connect(callback);
}

Connection TestTreeModel::connectPostItemDataChanged(DataCallback callback)
{
	return impl_->postDataChanged_.connect(callback);
}

Connection TestTreeModel::connectPreModelReset(VoidCallback callback)
{
	return impl_->preModelReset_.connect(callback);
}

Connection TestTreeModel::connectPostModelReset(VoidCallback callback)
{
	return impl_->postModelReset_.connect(callback);
}

Connection TestTreeModel::connectPreRowsMoved(MoveCallback callback)
{
	return impl_->preRowsMoved_.connect(callback);
}

Connection TestTreeModel::connectPostRowsMoved(MoveCallback callback)
{
	return impl_->postRowsMoved_.connect(callback);
}

void TestTreeModel::revert()
{
	impl_->preModelReset_();
	impl_->data_.clear();
	impl_->dataSource_ = StringList();
	impl_->generateData(nullptr, 0);
	impl_->postModelReset_();
}

MimeData TestTreeModel::mimeData(std::vector<AbstractItemModel::ItemIndex>& indices)
{
	impl_->mimeData_ = indices;
	MimeData mimeData;
	if (!indices.empty())
	{
		mimeData[TestTreeModelDetails::itemMimeKey] = {};
	}
	return mimeData;
}

void TestTreeModel::iterateMimeTypes(const std::function<void(const char*)>& iterFunc) const
{
	for (auto&& role : TestTreeModelDetails::s_MimeTypesVec)
	{
		iterFunc(role.c_str());
	}
}

std::vector<std::string> TestTreeModel::mimeTypes() const
{
	return TestTreeModelDetails::s_MimeTypesVec;
}

bool TestTreeModel::canDropMimeData(const MimeData& mimeData, 
									DropAction action,
									const AbstractItemModel::ItemIndex& index) const
{
	return mimeData.find(TestTreeModelDetails::itemMimeKey) != mimeData.end()
		&& !impl_->mimeData_.empty()
		&& action == DropAction::MoveAction;
}

bool TestTreeModel::dropMimeData(const MimeData& mimeData, 
								 DropAction action,
								 const AbstractItemModel::ItemIndex& index)
{
	bool success = false;

	if (action == DropAction::MoveAction)
	{
		auto it = mimeData.find(TestTreeModelDetails::itemMimeKey);
		if (it != mimeData.end())
		{
			success = true;
			for (auto& data : impl_->mimeData_)
			{
				success &= moveRows(data.parent_, data.row_, 1, index.parent_, index.row_);
			}
		}
	}

	impl_->mimeData_.clear();
	return success;
}

} // end namespace wgt
