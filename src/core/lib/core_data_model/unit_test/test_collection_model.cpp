#include "pch.hpp"

#include "test_abstract_item_model.hpp"
#include "core_data_model/collection_model.hpp"

#include <unordered_map>
#include <vector>

namespace wgt
{
template< typename T >
struct TestCollectionModelFixture : public TestAbstractItemModelFixture< CollectionModel >
{
	TestCollectionModelFixture()
	{
		Collection collection( collection_ );
		model_.setSource( collection );
	}

	bool testInsertItemLinear(TestResult& result_, const char*& m_name);
	bool testInsertItemMapping(TestResult& result_, const char*& m_name);
	bool testRemoveItemLinear(TestResult& result_, const char*& m_name);
	bool testRemoveItemMapping(TestResult& result_, const char*& m_name);

	T collection_;
};

struct TestLinearCollectionModelFixture : public TestCollectionModelFixture< std::vector< int > > {};
struct TestMappingCollectionModelFixture : public TestCollectionModelFixture<std::unordered_map<int, int>>
{
};

#define X( TEST ) ABSTRACT_ITEM_MODEL_TEST_F( TestLinearCollectionModelFixture, TEST )
ABSTRACT_ITEM_MODEL_TESTS
X(InsertItemLinear)
X(RemoveItemLinear)
#undef X

#define X(TEST) ABSTRACT_ITEM_MODEL_TEST_F(TestMappingCollectionModelFixture, TEST)
X(InsertItemMapping)
X(RemoveItemMapping)
#undef X

template <typename T>
bool TestCollectionModelFixture<T>::testInsertItemLinear(TestResult& result_,
                                                         const char*& m_name)
{
	auto rowCount = model_.rowCount();

	// Test inserting one item at the beginning of the root
	{
		CHECK(model_.insertItem(0, 0));
		const auto newRowCount = model_.rowCount();
		CHECK_EQUAL(rowCount + 1, newRowCount);
		rowCount = newRowCount;
	}

	// Test inserting one item past the end of the root
	{
		const auto lastKey = 0;
		const auto lastItem = model_.item(lastKey);
		CHECK(lastItem != nullptr);
		const auto endKey = lastKey + 1;
		const auto endItem = model_.item(endKey);
		CHECK(endItem == nullptr); // end

		CHECK(!model_.insertItem(2, 2));
		const auto newRowCount = model_.rowCount();
		CHECK_EQUAL(rowCount, newRowCount);
		const auto newLastKey = rowCount - 1;
		const auto newLastItem = model_.item(newLastKey);
		CHECK(newLastItem != nullptr);
		CHECK(newLastItem == lastItem);
		const auto newEndKey = rowCount;
		const auto newEndItem = model_.item(newEndKey);
		CHECK(endItem == nullptr); // end
	}

	// Test inserting one item at the end of the root
	{
		const auto lastKey = 0;
		const auto lastItem = model_.item(lastKey);
		CHECK(lastItem != nullptr);
		const auto endKey = lastKey + 1;
		const auto endItem = model_.item(endKey);
		CHECK(endItem == nullptr); // end

		CHECK(model_.insertItem(1, 2));
		const auto newRowCount = model_.rowCount();
		CHECK_EQUAL(rowCount + 1, newRowCount);
		rowCount = newRowCount;
		const auto newItem1 = model_.item(lastKey);
		CHECK(newItem1 == lastItem);
		const auto newLastKey = rowCount - 1;
		const auto newLastItem = model_.item(newLastKey);
		CHECK(newLastItem != nullptr);
		CHECK(newLastItem != lastItem);
		const auto newEndKey = rowCount;
		const auto newEndItem = model_.item(newEndKey);
		CHECK(endItem == nullptr); // end
	}

	// Test inserting one item in the middle of the root
	{
		const auto firstKey = 0;
		const auto item1 = model_.item(firstKey);
		const auto lastKey = 1;
		const auto lastItem = model_.item(lastKey);
		CHECK(lastItem != nullptr);
		const auto endKey = lastKey + 1;
		const auto endItem = model_.item(endKey);
		CHECK(endItem == nullptr); // end
		CHECK(model_.insertItem(1, 1));

		const auto newRowCount = model_.rowCount();
		CHECK_EQUAL(rowCount + 1, newRowCount);
		rowCount = newRowCount;
		const auto newItem1 = model_.item(firstKey);
		CHECK(newItem1 == item1);
		const auto newItem2 = model_.item(lastKey);
		CHECK(newItem2 != lastItem);
		const auto newLastKey = 2;
		const auto newLastItem = model_.item(newLastKey);
		CHECK(newLastItem != nullptr);
		CHECK(newLastItem == lastItem);
		const auto newEndKey = rowCount;
		const auto newEndItem = model_.item(newEndKey);
		CHECK(endItem == nullptr); // end
	}

	// Test insertion callbacks
	{
		auto preInsertedCalled = false;
		auto preInserted =
		[&result_, &m_name, &preInsertedCalled](int startRow, int count)
		{
			preInsertedCalled = true;
			CHECK_EQUAL(3, startRow);
			CHECK_EQUAL(1, count);
		};

		auto postInsertedCalled = false;
		auto postInserted =
		[&result_, &m_name, &postInsertedCalled](int startRow, int count)
		{
			postInsertedCalled = true;
			CHECK_EQUAL(3, startRow);
			CHECK_EQUAL(1, count);
		};

		auto preInsertedConnection = model_.connectPreRowsInserted(preInserted);
		auto postInsertedConnection = model_.connectPostRowsInserted(postInserted);
		CHECK(model_.insertItem(3, 3));
		CHECK(preInsertedConnection.connected());
		CHECK_EQUAL(true, preInsertedCalled);
		CHECK(postInsertedConnection.connected());
		CHECK_EQUAL(true, postInsertedCalled);

		// Check that removeItem is implemented
		// insertItem/removeItem must be implemented in pairs to allow undo/redo
		CHECK(model_.removeItem(0));
	}

	return true;
}

template <typename T>
bool TestCollectionModelFixture<T>::testInsertItemMapping(TestResult& result_,
                                                          const char*& m_name)
{
	auto rowCount = model_.rowCount();

	// Test inserting one item at the beginning of the root
	{
		CHECK(model_.insertItem(0, 0));
		const auto newRowCount = model_.rowCount();
		CHECK_EQUAL(rowCount + 1, newRowCount);
		rowCount = newRowCount;
	}

	// Test inserting one item at the end of the root
	{
		const auto lastKey = 0;
		const auto lastItem = model_.find(lastKey);
		CHECK(lastItem != nullptr);
		const auto endKey = lastKey + 1;
		const auto endItem = model_.find(endKey);
		CHECK(endItem == nullptr); // end

		CHECK(model_.insertItem(2, 2));
		const auto newRowCount = model_.rowCount();
		CHECK_EQUAL(rowCount + 1, newRowCount);
		rowCount = newRowCount;
		const auto newItem1 = model_.find(lastKey);
		CHECK(newItem1 == lastItem);
		const auto newLastKey = 2;
		const auto newLastItem = model_.find(newLastKey);
		CHECK(newLastItem != lastItem);
		const auto newEndKey = rowCount;
		const auto newEndItem = model_.find(newEndKey);
		CHECK(endItem == nullptr); // end
	}

	// Test inserting one item in the middle of the root
	{
		const auto firstKey = 0;
		const auto firstItem = model_.find(firstKey);
		const auto lastKey = 2;
		const auto lastItem = model_.find(lastKey);
		CHECK(lastItem != nullptr);
		const auto endKey = lastKey + 1;
		const auto endItem = model_.find(endKey);
		CHECK(endItem == nullptr); // end
		CHECK(model_.insertItem(1, 1));

		const auto newRowCount = model_.rowCount();
		CHECK_EQUAL(rowCount + 1, newRowCount);
		rowCount = newRowCount;
		const auto newFirstItem = model_.find(firstKey);
		CHECK(newFirstItem == firstItem);
		const auto newMiddleKey = 1;
		const auto newMiddleItem = model_.find(newMiddleKey);
		CHECK(newMiddleItem != nullptr);
		CHECK(newMiddleItem != firstItem);
		CHECK(newMiddleItem != lastItem);
		const auto newLastKey = 2;
		const auto newLastItem = model_.find(newLastKey);
		CHECK(newLastItem == lastItem);
		const auto newEndKey = newLastKey + 1;
		const auto newEndItem = model_.find(newEndKey);
		CHECK(endItem == nullptr); // end
	}

	// Test insertion callbacks
	{
		auto preInsertedCalled = false;
		auto preInserted =
		[&result_, &m_name, &preInsertedCalled](int startRow, int count)
		{
			preInsertedCalled = true;
			CHECK_EQUAL(1, count);
		};

		auto postInsertedCalled = false;
		auto postInserted =
		[&result_, &m_name, &postInsertedCalled](int startRow, int count)
		{
			postInsertedCalled = true;
			CHECK_EQUAL(1, count);
		};

		auto preInsertedConnection = model_.connectPreRowsInserted(preInserted);
		auto postInsertedConnection = model_.connectPostRowsInserted(postInserted);
		CHECK(model_.insertItem(3, 3));
		CHECK(preInsertedConnection.connected());
		CHECK_EQUAL(true, preInsertedCalled);
		CHECK(postInsertedConnection.connected());
		CHECK_EQUAL(true, postInsertedCalled);

		// Check that removeItem is implemented
		// insertItem/removeItem must be implemented in pairs to allow undo/redo
		CHECK(model_.removeItem(0));
	}

	return true;
}

template <typename T>
bool TestCollectionModelFixture<T>::testRemoveItemLinear(TestResult& result_,
                                                         const char*& m_name)
{
	// Insert some items to play with
	const int defaultRowCount = 25;
	const int column = 0;
	for (int row = 0; row < defaultRowCount; ++row)
	{
		CHECK(model_.insertItem(row, row));
	}
	auto rowCount = model_.rowCount();
	CHECK_EQUAL(defaultRowCount, rowCount);

	{
		const auto firstKey = 0;
		const auto firstItem = model_.item(firstKey);
		CHECK(firstItem != nullptr);
		const auto firstItemData = firstItem->getData(firstKey,
		                                              column,
		                                              ValueRole::roleId_);
		CHECK_EQUAL(Variant(firstKey), firstItemData);
		auto lastKey = rowCount - 1;
		const auto lastItem = model_.item(lastKey);
		CHECK(lastItem != nullptr);
		const auto lastItemData = lastItem->getData(lastKey,
		                                            column,
		                                            ValueRole::roleId_);
		CHECK_EQUAL(Variant(lastKey), lastItemData);
		const auto endKey = rowCount;
		const auto endItem = model_.item(endKey);
		CHECK(endItem == nullptr);
	}

	// Test removing one item from the beginning of the root
	// This call to removeItem() will invalidate pointers previously
	// obtained by item() (making them unsafe to use)
	{
		CHECK(model_.removeItem(0));
		const auto newRowCount = model_.rowCount();
		CHECK_EQUAL(rowCount - 1, newRowCount);
		rowCount = newRowCount;

		const auto firstKey = 0;
		const auto newFirstItem = model_.item(firstKey);
		CHECK(newFirstItem != nullptr);
		const auto newFirstItemData = newFirstItem != nullptr ?
		newFirstItem->getData(firstKey,
		                      column,
		                      ValueRole::roleId_) :
		Variant();
		CHECK_EQUAL(Variant(1), newFirstItemData);

		const auto lastKey = rowCount - 1;
		const auto newLastItem = model_.item(lastKey);
		CHECK(newLastItem != nullptr);
		const auto newLastItemData = newLastItem != nullptr ?
		newLastItem->getData(lastKey,
		                     column,
		                     ValueRole::roleId_) :
		Variant();
		CHECK_EQUAL(Variant(24), newLastItemData);

		const auto endKey = rowCount;
		const auto endItem = model_.item(endKey);
		CHECK(endItem == nullptr);
	}

	// Test removing one item from the end of the root
	{
		CHECK(model_.removeItem(rowCount - 1));
		const auto newRowCount = model_.rowCount();
		CHECK_EQUAL(rowCount - 1, newRowCount);
		rowCount = newRowCount;

		const auto firstKey = 0;
		const auto newFirstItem = model_.item(firstKey);
		CHECK(newFirstItem != nullptr);
		const auto newFirstItemData = newFirstItem != nullptr ?
		newFirstItem->getData(firstKey,
		                      column,
		                      ValueRole::roleId_) :
		Variant();
		CHECK_EQUAL(Variant(1), newFirstItemData);

		const auto lastKey = rowCount - 1;
		const auto newLastItem = model_.item(lastKey);
		CHECK(newLastItem != nullptr);
		const auto newLastItemData = newLastItem != nullptr ?
		newLastItem->getData(lastKey,
		                     column,
		                     ValueRole::roleId_) :
		Variant();
		CHECK_EQUAL(Variant(23), newLastItemData);

		const auto endKey = rowCount;
		const auto endItem = model_.item(endKey);
		CHECK(endItem == nullptr);
	}

	// Test removing one item from the middle of the root
	{
		CHECK(model_.removeItem(1));
		const auto newRowCount = model_.rowCount();
		CHECK_EQUAL(rowCount - 1, newRowCount);
		rowCount = newRowCount;

		const auto firstKey = 0;
		const auto newFirstItem = model_.item(firstKey);
		CHECK(newFirstItem != nullptr);
		const auto newFirstItemData = newFirstItem != nullptr ?
		newFirstItem->getData(firstKey,
		                      column,
		                      ValueRole::roleId_) :
		Variant();
		CHECK_EQUAL(Variant(1), newFirstItemData);

		const auto lastKey = rowCount - 1;
		const auto newLastItem = model_.item(lastKey);
		CHECK(newLastItem != nullptr);
		const auto newLastItemData = newLastItem != nullptr ?
		newLastItem->getData(lastKey,
		                     column,
		                     ValueRole::roleId_) :
		Variant();
		CHECK_EQUAL(Variant(23), newLastItemData);

		const auto endKey = rowCount;
		const auto endItem = model_.item(endKey);
		CHECK(endItem == nullptr);
	}

	// Test removal callbacks
	{
		auto preRemovedCalled = false;
		auto preRemoved =
		[&result_, &m_name, &preRemovedCalled](int startRow, int count)
		{
			preRemovedCalled = true;
			CHECK_EQUAL(0, startRow);
			CHECK_EQUAL(1, count);
		};

		auto postRemovedCalled = false;
		auto postRemoved =
		[&result_, &m_name, &postRemovedCalled](int startRow, int count)
		{
			postRemovedCalled = true;
			CHECK_EQUAL(0, startRow);
			CHECK_EQUAL(1, count);
		};

		auto preRemovedConnection = model_.connectPreRowsRemoved(preRemoved);
		auto postRemovedConnection = model_.connectPostRowsRemoved(postRemoved);
		CHECK(model_.removeItem(0));
		CHECK(preRemovedConnection.connected());
		CHECK_EQUAL(true, preRemovedCalled);
		CHECK(postRemovedConnection.connected());
		CHECK_EQUAL(true, postRemovedCalled);
	}

	return true;
}

template <typename T>
bool TestCollectionModelFixture<T>::testRemoveItemMapping(TestResult& result_,
                                                          const char*& m_name)
{
	// Insert some items to play with
	const int defaultRowCount = 25;
	const int column = 0;
	for (int row = 0; row < defaultRowCount; ++row)
	{
		CHECK(model_.insertItem(row, row));
	}
	auto rowCount = model_.rowCount();
	CHECK_EQUAL(defaultRowCount, rowCount);

	{
		const auto firstKey = 0;
		const auto newFirstItem = model_.find(firstKey);
		CHECK(newFirstItem != nullptr);
		const auto newFirstItemData = newFirstItem != nullptr ?
		newFirstItem->getData(firstKey,
		                      column,
		                      ValueRole::roleId_) :
		Variant();
		CHECK_EQUAL(Variant(firstKey), newFirstItemData);

		const auto lastKey = rowCount - 1;
		const auto newLastItem = model_.find(lastKey);
		CHECK(newLastItem != nullptr);
		const auto newLastItemData = newLastItem != nullptr ?
		newLastItem->getData(lastKey,
		                     column,
		                     ValueRole::roleId_) :
		Variant();
		CHECK_EQUAL(Variant(lastKey), newLastItemData);
		const auto endKey = 25;
		const auto endItem = model_.find(endKey);
		CHECK(endItem == nullptr);
	}

	// Test removing one item from the beginning of the root
	// This call to removeItem() will invalidate pointers previously
	// obtained by find() (making them unsafe to use)
	{
		CHECK(model_.removeItem(0));
		const auto newRowCount = model_.rowCount();
		CHECK_EQUAL(rowCount - 1, newRowCount);
		rowCount = newRowCount;

		const auto oldFirstKey = 0;
		const auto oldFirstItem = model_.find(oldFirstKey);
		CHECK(oldFirstItem == nullptr);

		const auto newFirstKey = 1;
		const auto newFirstItem = model_.find(newFirstKey);
		CHECK(newFirstItem != nullptr);
		const auto newFirstItemData = newFirstItem != nullptr ?
		newFirstItem->getData(newFirstKey,
		                      column,
		                      ValueRole::roleId_) :
		Variant();
		CHECK_EQUAL(Variant(newFirstKey), newFirstItemData);

		const auto newLastKey = 24;
		const auto newLastItem = model_.find(newLastKey);
		CHECK(newLastItem != nullptr);
		const auto newLastItemData = newLastItem != nullptr ?
		newLastItem->getData(newLastKey,
		                     column,
		                     ValueRole::roleId_) :
		Variant();
		CHECK_EQUAL(Variant(newLastKey), newLastItemData);

		const auto endKey = 25;
		const auto endItem = model_.find(endKey);
		CHECK(endItem == nullptr);
	}

	// Test removing one item from the end of the root
	{
		CHECK(model_.removeItem(24));
		const auto newRowCount = model_.rowCount();
		CHECK_EQUAL(rowCount - 1, newRowCount);
		rowCount = newRowCount;

		const auto oldFirstKey = 1;
		const auto oldFirstItem = model_.find(oldFirstKey);
		CHECK(oldFirstItem != nullptr);
		const auto oldFirstItemData = oldFirstItem != nullptr ?
		oldFirstItem->getData(oldFirstKey,
		                      column,
		                      ValueRole::roleId_) :
		Variant();
		CHECK_EQUAL(Variant(oldFirstKey), oldFirstItemData);

		const auto oldLastKey = 24;
		const auto oldLastItem = model_.find(oldLastKey);
		CHECK(oldLastItem == nullptr);

		const auto newLastKey = 23;
		const auto newLastItem = model_.find(newLastKey);
		CHECK(newLastItem != nullptr);
		const auto newLastItemData = newLastItem != nullptr ?
		newLastItem->getData(newLastKey,
		                     column,
		                     ValueRole::roleId_) :
		Variant();
		CHECK_EQUAL(Variant(newLastKey), newLastItemData);

		const auto endKey = 24;
		const auto endItem = model_.find(endKey);
		CHECK(endItem == nullptr);
	}

	// Test removing one item from the middle of the root
	{
		CHECK(model_.removeItem(2));
		const auto newRowCount = model_.rowCount();
		CHECK_EQUAL(rowCount - 1, newRowCount);
		rowCount = newRowCount;

		const auto oldFirstKey = 1;
		const auto oldFirstItem = model_.find(oldFirstKey);
		CHECK(oldFirstItem != nullptr);
		const auto oldFirstItemData = oldFirstItem != nullptr ?
		oldFirstItem->getData(oldFirstKey,
		                      column,
		                      ValueRole::roleId_) :
		Variant();
		CHECK_EQUAL(Variant(oldFirstKey), oldFirstItemData);

		const auto oldMiddleKey = 2;
		const auto oldMiddleItem = model_.find(oldMiddleKey);
		CHECK(oldMiddleItem == nullptr);

		const auto newMiddleKey = 1;
		const auto newMiddleItem = model_.find(newMiddleKey);
		CHECK(newMiddleItem != nullptr);
		const auto newMiddleItemData = newMiddleItem != nullptr ?
		newMiddleItem->getData(newMiddleKey,
		                       column,
		                       ValueRole::roleId_) :
		Variant();
		CHECK_EQUAL(Variant(newMiddleKey), newMiddleItemData);

		const auto lastKey = 23;
		const auto newLastItem = model_.find(lastKey);
		CHECK(newLastItem != nullptr);
		const auto newLastItemData = newLastItem != nullptr ?
		newLastItem->getData(lastKey,
		                     column,
		                     ValueRole::roleId_) :
		Variant();
		CHECK_EQUAL(Variant(23), newLastItemData);

		const auto endKey = 24;
		const auto endItem = model_.find(endKey);
		CHECK(endItem == nullptr);
	}

	// Test removal callbacks
	{
		auto preRemovedCalled = false;
		auto preRemoved =
		[&result_, &m_name, &preRemovedCalled](int startRow, int count)
		{
			preRemovedCalled = true;
			CHECK_EQUAL(1, count);
		};

		auto postRemovedCalled = false;
		auto postRemoved =
		[&result_, &m_name, &postRemovedCalled](int startRow, int count)
		{
			postRemovedCalled = true;
			CHECK_EQUAL(1, count);
		};

		auto preRemovedConnection = model_.connectPreRowsRemoved(preRemoved);
		auto postRemovedConnection = model_.connectPostRowsRemoved(postRemoved);
		CHECK(model_.removeItem(5));
		CHECK(preRemovedConnection.connected());
		CHECK_EQUAL(true, preRemovedCalled);
		CHECK(postRemovedConnection.connected());
		CHECK_EQUAL(true, postRemovedCalled);
	}

	return true;
}

} // end namespace wgt
