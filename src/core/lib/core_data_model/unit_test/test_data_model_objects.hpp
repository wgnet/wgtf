#ifndef TEST_DATA_MODEL_OBJECTS_HPP
#define TEST_DATA_MODEL_OBJECTS_HPP
#include "pch.hpp"

#include "test_data_model_fixture.hpp"
#include "test_string_data.hpp"
#include "core_data_model/filtered_list_model.hpp"
#include "core_data_model/filtered_tree_model.hpp"
#include "core_data_model/variant_list.hpp"
#include "core_data_model/filtering/string_filter.hpp"
#include <memory>
#include <vector>

namespace wgt
{
class TestStringFilter : public StringFilter
{
public:
	TestStringFilter() : filterDescendantsOfMatchingItems_(false)
	{
	}

	virtual ~TestStringFilter()
	{
	}

	virtual bool filterDescendantsOfMatchingItems() override
	{
		return filterDescendantsOfMatchingItems_;
	}

	void filterDescendantsOfMatchingItems(bool value)
	{
		filterDescendantsOfMatchingItems_ = value;
		signalFilterChanged();
	}

private:
	bool filterDescendantsOfMatchingItems_;
};

//------------------------------------------------------------------------------
// Test Fixture
//------------------------------------------------------------------------------

class TestFixture : public TestDataModelFixture
{
public:
	TestFixture()
	{
	}
	~TestFixture()
	{
	}

	// Initialization
	void initialise(TestStringData::State state);

	// List Model Helper Functions
	void echoListData();
	bool findItemInFilteredList(const char* term, bool exactMatch = false);
	bool verifyListItemPosition(unsigned int index, const char* value);
	void insertIntoListAtIndex(unsigned int index, const char* value);
	void removeFromListAtIndex(unsigned int index);
	void getListItemValueAtIndex(unsigned int index, std::string& value);
	void updateListItemAtIndex(unsigned int index, const char* value);

	// Tree Model Helper Functions
	bool verifyTreeItemMatch(IItem* item, const char* value, bool exactMatch = false);

public:
	// Data for the test(s)
	TestStringData testStringData_;

	// List Models
	FilteredListModel filteredTestList_;

	// Tree Models
	FilteredTreeModel filteredTestTree_;

	// Filters
	std::vector<std::string> filterTerms_;
	TestStringFilter filter_;
};

//------------------------------------------------------------------------------
// Test Tree Model and IItem
//------------------------------------------------------------------------------

class UnitTestTreeItem : public IItem
{
public:
	UnitTestTreeItem(const char* name, const IItem* parent);
	UnitTestTreeItem(const UnitTestTreeItem& rhs);
	virtual ~UnitTestTreeItem();

	UnitTestTreeItem& operator=(const UnitTestTreeItem& rhs);

	const IItem* getParent() const;
	virtual const char* getDisplayText(int column) const;
	virtual ThumbnailData getThumbnail(int column) const;
	virtual void setName(const char* name);
	virtual Variant getData(int column, ItemRole::Id roleId) const;
	virtual bool setData(int column, ItemRole::Id roleId, const Variant& data);

private:
	struct Implementation;
	std::unique_ptr<Implementation> impl_;
};

enum class InsertAt
{
	FRONT,
	BACK
};

class UnitTestTreeModel : public ITreeModel
{
public:
	UnitTestTreeModel();
	UnitTestTreeModel(const UnitTestTreeModel& rhs);
	virtual ~UnitTestTreeModel();

	UnitTestTreeModel& operator=(const UnitTestTreeModel& rhs);

	void initialise(TestStringData* dataSource);

	virtual IItem* item(size_t index, const IItem* parent) const override;
	virtual ItemIndex index(const IItem* item) const override;
	virtual bool empty(const IItem* parent) const override;
	virtual size_t size(const IItem* parent) const override;
	virtual int columnCount() const override;

	virtual UnitTestTreeItem* insert(const UnitTestTreeItem* parent, std::string& data, InsertAt where);
	virtual void erase(size_t index, const UnitTestTreeItem* parent);
	virtual void update(size_t index, const UnitTestTreeItem* parent, std::string& data);

private:
	struct Implementation;
	std::unique_ptr<Implementation> impl_;
};
} // end namespace wgt
#endif // TEST_DATA_MODEL_OBJECTS_HPP
