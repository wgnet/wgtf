#include "pch.hpp"

#include "test_abstract_item_model.hpp"

namespace wgt
{
	bool TestAbstractItemModelFixtureBase::testInsertRows( TestResult& result_, const char *& m_name )
	{
		auto model = getModel();
		if (model == nullptr)
		{
			return false;
		}

		auto rowCount = model->rowCount( nullptr );

		// Test inserting one item at the beginning of the root
		if (model->insertRows( 0, 1, nullptr ))
		{
			auto newRowCount = model->rowCount( nullptr);
			CHECK_EQUAL( rowCount + 1, newRowCount );
			rowCount = newRowCount;
		}
		else
		{
			// insertRows is not implemented for this type of model
			// Skip this test
			return true;
		}

		// Test inserting one item at the end of the root
		if (rowCount > 0)
		{
			const auto lastIndex = AbstractItemModel::ItemIndex( rowCount - 1, 0, nullptr );
			const auto lastItem = model->item( lastIndex );
			CHECK( lastItem != nullptr );
			const auto endIndex = AbstractItemModel::ItemIndex( rowCount, 0, nullptr );
			const auto endItem = model->item( endIndex );
			CHECK( endItem == nullptr ); // end
			if (model->insertRows( rowCount, 1, nullptr ))
			{
				const auto newRowCount = model->rowCount( nullptr);
				CHECK_EQUAL( rowCount + 1, newRowCount );
				rowCount = newRowCount;
				const auto newItem1 = model->item( lastIndex );
				CHECK( newItem1 == lastItem );
				const auto newLastIndex = AbstractItemModel::ItemIndex( rowCount - 1, 0, nullptr );
				const auto newLastItem = model->item( newLastIndex );
				CHECK( newLastItem != nullptr );
				CHECK( newLastItem != lastItem );
				const auto newEndIndex = AbstractItemModel::ItemIndex( rowCount, 0, nullptr );
				const auto newEndItem = model->item( newEndIndex );
				CHECK( endItem == nullptr ); // end
			}
		}

		// Test inserting one item in the middle of the root
		if (rowCount > 1)
		{
			const auto index1 = AbstractItemModel::ItemIndex( 0, 0, nullptr );
			const auto item1 = model->item( index1 );
			const auto lastIndex = AbstractItemModel::ItemIndex( rowCount - 1, 0, nullptr );
			const auto lastItem = model->item( lastIndex );
			const auto endIndex = AbstractItemModel::ItemIndex( rowCount, 0, nullptr );
			const auto endItem = model->item( endIndex );
			CHECK( endItem == nullptr ); // end
			if (model->insertRows( 1, 1, nullptr ))
			{
				const auto newRowCount = model->rowCount( nullptr);
				CHECK_EQUAL( rowCount + 1, newRowCount );
				rowCount = newRowCount;
				const auto newItem1 = model->item( index1 );
				CHECK( newItem1 == item1 );
				const auto newItem2 = model->item( lastIndex );
				CHECK( newItem2 != lastItem );
				const auto newLastIndex = AbstractItemModel::ItemIndex( rowCount - 1, 0, nullptr );
				const auto newLastItem = model->item( newLastIndex );
				CHECK( newLastItem != nullptr );
				CHECK( newLastItem == lastItem );
				const auto newEndIndex = AbstractItemModel::ItemIndex( rowCount, 0, nullptr );
				const auto newEndItem = model->item( newEndIndex );
				CHECK( endItem == nullptr ); // end

			}
		}

		// Test inserting multiple items at the beginning of the root
		if (model->insertRows( 0, 10, nullptr ))
		{
			auto newRowCount = model->rowCount( nullptr);
			CHECK_EQUAL( rowCount + 10, newRowCount );
			rowCount = newRowCount;
		}

		// Insert one item as a child of another item
		// Note: no models in core currently run this test
		if (rowCount > 0)
		{
			auto childIndex = AbstractItemModel::ItemIndex( 0, 0, nullptr );
			auto childItem = model->item( childIndex );
			auto childCount = model->rowCount( childItem );
			if (model->insertRows( 0, 1, childItem ))
			{
				auto newChildItem = model->item( childIndex );
				CHECK( newChildItem == childItem );
				auto newChildCount = model->rowCount( newChildItem );
				CHECK_EQUAL( childCount + 1, newChildCount );
			}
		}

		// Test insertion callbacks
		auto preInsertedCalled = false;
		auto preInserted = 
			[ & ]( const AbstractItemModel::ItemIndex & parentIndex,
			int startPos,
			int count )
		{
			preInsertedCalled = true;
			CHECK( parentIndex == AbstractItemModel::ItemIndex() );
			CHECK_EQUAL( 0, startPos );
			CHECK_EQUAL( 1, count );
		};

		auto postInsertedCalled = false;
		auto postInserted = 
			[ & ]( const AbstractItemModel::ItemIndex & parentIndex,
			int startPos,
			int count )
		{
			postInsertedCalled = true;
			CHECK( parentIndex == AbstractItemModel::ItemIndex() );
			CHECK_EQUAL( 0, startPos );
			CHECK_EQUAL( 1, count );
		};

		auto preInsertedConnection = model->connectPreRowsInserted( preInserted );
		auto postInsertedConnection = model->connectPostRowsInserted( postInserted );
		if (model->insertRows( 0, 1, nullptr ))
		{
		    rowCount = model->rowCount(nullptr);
		    if (preInsertedConnection.connected())
		    {
				CHECK_EQUAL( true, preInsertedCalled );
			}
			if (postInsertedConnection.connected())
			{
				CHECK_EQUAL( true, postInsertedCalled );
			}
		}

	    // Test that pointers don't change after shifting all the items
	    {
		    std::vector<AbstractItem*> originalItems;
		    for (int row = 0; row < model->rowCount(nullptr); ++row)
		    {
			    const auto index = AbstractItemModel::ItemIndex(row, 0, nullptr);
			    originalItems.emplace_back(model->item(index));
		    }
		    const int insertCount = 10;
		    if (model->insertRows(0, insertCount, nullptr))
		    {
			    auto newRowCount = model->rowCount(nullptr);
			    CHECK_EQUAL(rowCount + insertCount, newRowCount);
			    rowCount = newRowCount;

			    for (int row = 0; row < static_cast<int>(originalItems.size()); ++row)
			    {
				    const auto index = AbstractItemModel::ItemIndex(row + insertCount, 0, nullptr);
				    const auto newItem = model->item(index);
				    CHECK(originalItems[row] == newItem);
			    }
		    }
	    }

	    // Check that removeRows is implemented
	    // insertRows/removeRows must be implemented in pairs to allow undo/redo
	    CHECK( model->removeRows( 0, 1, nullptr ) );

		return true;
	}

	bool TestAbstractItemModelFixtureBase::testInsertColumns( TestResult& result_, const char *& m_name )
	{
		auto model = getModel();
		if (model == nullptr)
		{
			return false;
		}

		auto columnCount = model->columnCount( nullptr );

		// Test inserting one item at the beginning of the root
		if (model->insertColumns( 0, 1, nullptr ))
		{
			auto newColumnCount = model->columnCount( nullptr);
			CHECK_EQUAL( columnCount + 1, newColumnCount );
			columnCount = newColumnCount;
		}
		else
		{
			// insertColumns is not implemented for this type of model
			// Skip this test
			// Note: no models in core currently run this test
			return true;
		}

		// Test inserting one item at the end of the root
		if (columnCount > 0)
		{
			const auto lastIndex = AbstractItemModel::ItemIndex( 0, columnCount - 1, nullptr );
			const auto lastItem = model->item( lastIndex );
			CHECK( lastItem != nullptr );
			const auto endIndex = AbstractItemModel::ItemIndex( 0, columnCount, nullptr );
			const auto endItem = model->item( endIndex );
			CHECK( endItem == nullptr ); // end
			if (model->insertColumns( columnCount, 1, nullptr ))
			{
				auto newColumnCount = model->columnCount( nullptr);
				CHECK_EQUAL( columnCount + 1, newColumnCount );
				columnCount = newColumnCount;
				auto newItem1 = model->item( lastIndex );
				CHECK( newItem1 == lastItem );
				const auto newLastIndex = AbstractItemModel::ItemIndex( 0, columnCount - 1, nullptr );
				const auto newLastItem = model->item( newLastIndex );
				CHECK( newLastItem != nullptr );
				CHECK( newLastItem != lastItem );
				const auto newEndIndex = AbstractItemModel::ItemIndex( 0, columnCount, nullptr );
				const auto newEndItem = model->item( newEndIndex );
				CHECK( endItem == nullptr ); // end
			}
		}

		// Test inserting one item in the middle of the root
		if (columnCount > 1)
		{
			auto index1 = AbstractItemModel::ItemIndex( 0, 0, nullptr );
			auto item1 = model->item( index1 );
			const auto lastIndex = AbstractItemModel::ItemIndex( 0, columnCount - 1, nullptr );
			const auto lastItem = model->item( lastIndex );
			CHECK( lastItem != nullptr );
			const auto endIndex = AbstractItemModel::ItemIndex( 0, columnCount, nullptr );
			const auto endItem = model->item( endIndex );
			CHECK( endItem == nullptr ); // end
			if (model->insertColumns( 1, 1, nullptr ))
			{
				auto newColumnCount = model->columnCount( nullptr);
				CHECK_EQUAL( columnCount + 1, newColumnCount );
				columnCount = newColumnCount;
				auto newItem1 = model->item( index1 );
				CHECK( newItem1 == item1 );
				const auto newLastItem = model->item( lastIndex );
				CHECK( newLastItem == lastItem );
				auto index3 = AbstractItemModel::ItemIndex( 0, columnCount, nullptr );
				auto item3 = model->item( index3 );
				CHECK( item3 == nullptr ); // end
			}
		}

		// Test inserting multiple items at the beginning of the root
		if (model->insertColumns( 0, 10, nullptr ))
		{
			auto newColumnCount = model->columnCount( nullptr);
			CHECK_EQUAL( columnCount + 10, newColumnCount );
			columnCount = newColumnCount;
		}

		// Insert one item as a child of another item
		// Note: no models in core currently run this test
		if (columnCount > 0)
		{
			auto childIndex = AbstractItemModel::ItemIndex( 0, 0, nullptr );
			auto childItem = model->item( childIndex );
			auto childCount = model->columnCount( childItem );
			if (model->insertColumns( 0, 1, childItem ))
			{
				auto newChildItem = model->item( childIndex );
				CHECK( newChildItem == childItem );
				auto newChildCount = model->columnCount( newChildItem );
				CHECK_EQUAL( childCount + 1, newChildCount );
			}
		}

		// Test insertion callbacks
		auto preInsertedCalled = false;
		auto preInserted = 
			[ & ]( const AbstractItemModel::ItemIndex & parentIndex,
			int startPos,
			int count )
		{
			preInsertedCalled = true;
			CHECK( parentIndex == AbstractItemModel::ItemIndex() );
			CHECK_EQUAL( 0, startPos );
			CHECK_EQUAL( 1, count );
		};

		auto postInsertedCalled = false;
		auto postInserted = 
			[ & ]( const AbstractItemModel::ItemIndex & parentIndex,
			int startPos,
			int count )
		{
			postInsertedCalled = true;
			CHECK( parentIndex == AbstractItemModel::ItemIndex() );
			CHECK_EQUAL( 0, startPos );
			CHECK_EQUAL( 1, count );
		};

		auto preInsertedConnection = model->connectPreColumnsInserted( preInserted );
		auto postInsertedConnection = model->connectPostColumnsInserted( postInserted );
	    if (model->insertColumns(0, 1, nullptr))
	    {
		    columnCount = model->columnCount(nullptr);
		    if (preInsertedConnection.connected())
		    {
			    CHECK_EQUAL(true, preInsertedCalled);
		    }
		    if (postInsertedConnection.connected())
			{
				CHECK_EQUAL( true, postInsertedCalled );
			}
		}

	    // Test that pointers don't change after shifting all the items
	    {
		    std::vector<AbstractItem*> originalItems;
		    for (int column = 0; column < model->columnCount(nullptr); ++column)
		    {
			    const auto index = AbstractItemModel::ItemIndex(0, column, nullptr);
			    originalItems.emplace_back(model->item(index));
		    }
		    const int insertCount = 10;
		    if (model->insertColumns(0, insertCount, nullptr))
		    {
			    auto newColumnCount = model->columnCount(nullptr);
			    CHECK_EQUAL(columnCount + insertCount, newColumnCount);
			    columnCount = newColumnCount;

			    for (int column = 0; column < static_cast<int>(originalItems.size()); ++column)
			    {
				    const auto index = AbstractItemModel::ItemIndex(0, column + insertCount, nullptr);
				    const auto newItem = model->item(index);
				    CHECK(originalItems[column] == newItem);
			    }
		    }
	    }

	    // Check that removeColumns is implemented
	    // insertColumns/removeColumns must be implemented in pairs to allow undo/redo
	    CHECK( model->removeColumns( 0, 1, nullptr ) );

		return true;
	}

	bool TestAbstractItemModelFixtureBase::testRemoveRows( TestResult& result_, const char *& m_name )
	{
		auto model = getModel();
		if (model == nullptr)
		{
			return false;
		}

		// Insert some items to play with
		const int defaultRowCount = 25;
		const auto canInsertRows = model->insertRows( 0, defaultRowCount, nullptr );
		if (!canInsertRows)
		{
			// Ignore test
			return true;
		}
		auto rowCount = model->rowCount( nullptr );
		CHECK_EQUAL( defaultRowCount, rowCount );

		// Initialise, assumes test data is integers
		// Need a way of comparing item data after one has been deleted 
		{
			for (int row = 0; row != rowCount; ++row)
			{
				const auto index = AbstractItemModel::ItemIndex( row, 0, nullptr );
				const auto setOk = model->item( index )->setData( index.row_,
					index.column_,
					ValueRole::roleId_,
					row );
				CHECK( setOk );
			}
		}

		const auto firstIndex = AbstractItemModel::ItemIndex( 0, 0, nullptr );
		const auto firstItem = model->item( firstIndex );
		CHECK( firstItem != nullptr );
		const auto firstItemData = firstItem->getData( firstIndex.row_,
			firstIndex.column_,
			ValueRole::roleId_ );
		CHECK_EQUAL( Variant( firstIndex.row_ ), firstItemData );
		auto lastIndex = AbstractItemModel::ItemIndex( rowCount - 1, 0, nullptr );
		const auto lastItem = model->item( lastIndex );
		CHECK( lastItem != nullptr );
		const auto lastItemData = lastItem->getData( lastIndex.row_,
			lastIndex.column_,
			ValueRole::roleId_ );
		CHECK_EQUAL( Variant( lastIndex.row_ ), lastItemData );
		const auto endIndex = AbstractItemModel::ItemIndex( rowCount, 0, nullptr );
		const auto endItem = model->item( endIndex );
		CHECK( endItem == nullptr );

		// Test removing one item from the beginning of the root
		// This call to removeRows() will invalidate pointers previously
		// obtained by item() (making them unsafe to use)
		if (rowCount > 1 && model->removeRows( 0, 1, nullptr ))
		{
			const auto newRowCount = model->rowCount( nullptr);
			CHECK_EQUAL( rowCount - 1, newRowCount );
			rowCount = newRowCount;

			const auto newFirstItem = model->item( firstIndex );
			const auto newFirstItemData = model->item( firstIndex )->getData(
				firstIndex.row_,
				firstIndex.column_,
				ValueRole::roleId_ );
			CHECK_EQUAL( Variant( 1 ), newFirstItemData );

			lastIndex = AbstractItemModel::ItemIndex( rowCount - 1, 0, nullptr );
			const auto newLastItem = model->item( lastIndex );
			const auto newLastItemData = model->item( lastIndex )->getData(
				lastIndex.row_,
				lastIndex.column_,
				ValueRole::roleId_ );
			CHECK_EQUAL( Variant( 24 ), newLastItemData );

			const auto endIndex = AbstractItemModel::ItemIndex( rowCount, 0, nullptr );
			const auto endItem = model->item( endIndex );
			CHECK( endItem == nullptr );
		}
		else
		{
			// insertRows/removeRows is not implemented for this type of model
			// Skip this test
			return true;
		}

		// Test removing one item from the end of the root
		if (rowCount > 1 && model->removeRows( rowCount - 1, 1, nullptr ))
		{
			const auto newRowCount = model->rowCount( nullptr);
			CHECK_EQUAL( rowCount - 1, newRowCount );
			rowCount = newRowCount;

			const auto newFirstItem = model->item( firstIndex );
			const auto newFirstItemData = model->item( firstIndex )->getData(
				firstIndex.row_,
				firstIndex.column_,
				ValueRole::roleId_ );
			CHECK_EQUAL( Variant( 1 ), newFirstItemData );

			lastIndex = AbstractItemModel::ItemIndex( rowCount - 1, 0, nullptr );
			const auto newLastItem = model->item( lastIndex );
			const auto newLastItemData = model->item( lastIndex )->getData(
				lastIndex.row_,
				lastIndex.column_,
				ValueRole::roleId_ );
			CHECK_EQUAL( Variant( 23 ), newLastItemData );

			const auto endIndex = AbstractItemModel::ItemIndex( rowCount, 0, nullptr );
			const auto endItem = model->item( endIndex );
			CHECK( endItem == nullptr );
		}

		// Test removing one item from the middle of the root
		if (rowCount > 2 && model->removeRows( 1, 1, nullptr ))
		{
			const auto newRowCount = model->rowCount( nullptr);
			CHECK_EQUAL( rowCount - 1, newRowCount );
			rowCount = newRowCount;

			const auto newFirstItem = model->item( firstIndex );
			const auto newFirstItemData = model->item( firstIndex )->getData(
				firstIndex.row_,
				firstIndex.column_,
				ValueRole::roleId_ );
			CHECK_EQUAL( Variant( 1 ), newFirstItemData );

			lastIndex = AbstractItemModel::ItemIndex( rowCount - 1, 0, nullptr );
			const auto newLastItem = model->item( lastIndex );
			const auto newLastItemData = model->item( lastIndex )->getData(
				lastIndex.row_,
				lastIndex.column_,
				ValueRole::roleId_ );
			CHECK_EQUAL( Variant( 23 ), newLastItemData );

			const auto endIndex = AbstractItemModel::ItemIndex( rowCount, 0, nullptr );
			const auto endItem = model->item( endIndex );
			CHECK( endItem == nullptr );
		}

		// Test removing multiple items from the beginning of the root
		if (rowCount > 10 && model->removeRows( 0, 10, nullptr ))
		{
			const auto newRowCount = model->rowCount( nullptr);
			CHECK_EQUAL( rowCount - 10, newRowCount );
			rowCount = newRowCount;

			const auto newFirstItem = model->item( firstIndex );
			const auto newFirstItemData = model->item( firstIndex )->getData(
				firstIndex.row_,
				firstIndex.column_,
				ValueRole::roleId_ );
			CHECK_EQUAL( Variant( 12 ), newFirstItemData );

			lastIndex = AbstractItemModel::ItemIndex( rowCount - 1, 0, nullptr );
			const auto newLastItem = model->item( lastIndex );
			const auto newLastItemData = model->item( lastIndex )->getData(
				lastIndex.row_,
				lastIndex.column_,
				ValueRole::roleId_ );
			CHECK_EQUAL( Variant( 23 ), newLastItemData );

			const auto endIndex = AbstractItemModel::ItemIndex( rowCount, 0, nullptr );
			const auto endItem = model->item( endIndex );
			CHECK( endItem == nullptr );
		}

		// Remove one child item of another item
		if (rowCount > 0)
		{
			const auto childIndex = AbstractItemModel::ItemIndex( 0, 0, nullptr );
			const auto childItem = model->item( childIndex );
			auto childItemData = childItem->getData( childIndex.row_,
				childIndex.column_,
				ValueRole::roleId_ );
			CHECK_EQUAL( Variant( 12 ), childItemData );

			// Note: no models in core currently run this test
			const int defaultChildCount = 10;
			const auto canAddChildren = model->insertRows( 0, defaultChildCount, childItem );
			if (canAddChildren)
			{
				auto childCount = model->rowCount( childItem );
				CHECK_EQUAL( defaultChildCount, childCount );

				if (model->removeRows( 0, 1, childItem ))
				{
					const auto newChildItem = model->item( childIndex );
					auto newChildItemData = childItem->getData( childIndex.row_,
						childIndex.column_,
						ValueRole::roleId_ );
					CHECK_EQUAL( Variant( 11 ), childItemData );

					const auto newChildCount = model->columnCount( newChildItem );
					CHECK_EQUAL( childCount - 1, newChildCount );
				}
			}
		}

		// Test removal callbacks
		auto preRemovedCalled = false;
		auto preRemoved = 
			[ & ]( const AbstractItemModel::ItemIndex & parentIndex,
			int startPos,
			int count )
		{
			preRemovedCalled = true;
			CHECK( parentIndex == AbstractItemModel::ItemIndex() );
			CHECK_EQUAL( 0, startPos );
			CHECK_EQUAL( 1, count );
		};

		auto postRemovedCalled = false;
		auto postRemoved = 
			[ & ]( const AbstractItemModel::ItemIndex & parentIndex,
			int startPos,
			int count )
		{
			postRemovedCalled = true;
			CHECK( parentIndex == AbstractItemModel::ItemIndex() );
			CHECK_EQUAL( 0, startPos );
			CHECK_EQUAL( 1, count );
		};

		auto preRemovedConnection = model->connectPreRowsRemoved( preRemoved );
		auto postRemovedConnection = model->connectPostRowsRemoved( postRemoved );
	    if (model->removeRows(0, 1, nullptr))
	    {
		    rowCount = model->rowCount(nullptr);

		    if (preRemovedConnection.connected())
		    {
			    CHECK_EQUAL(true, preRemovedCalled);
		    }
		    if (postRemovedConnection.connected())
			{
				CHECK_EQUAL( true, postRemovedCalled );
			}
		}

	    // Test that pointers don't change after shifting all the items
	    {
		    std::vector<AbstractItem*> originalItems;
		    for (int row = 0; row < model->rowCount(nullptr); ++row)
		    {
			    const auto index = AbstractItemModel::ItemIndex(row, 0, nullptr);
			    originalItems.emplace_back(model->item(index));
		    }
		    const int removeCount = 10;
		    CHECK(model->rowCount(nullptr) >= removeCount);
		    if (model->removeRows(0, removeCount, nullptr))
		    {
			    auto newRowCount = model->rowCount(nullptr);
			    CHECK_EQUAL(rowCount - removeCount, newRowCount);
			    rowCount = newRowCount;

			    for (int row = 0; row < model->rowCount(nullptr); ++row)
			    {
				    const auto itemIndex = AbstractItemModel::ItemIndex(row, 0, nullptr);
				    const auto newItem = model->item(itemIndex);
				    const size_t originalIndex = static_cast<size_t>(row + removeCount);
				    CHECK(originalItems.at(originalIndex) == newItem);
			    }
		    }
	    }

	    return true;
    }

	bool TestAbstractItemModelFixtureBase::testRemoveColumns( TestResult& result_, const char *& m_name )
	{
		auto model = getModel();
		if (model == nullptr)
		{
			return false;
		}

		// Insert some items to play with
		const int defaultColumnCount = 25;
		const auto canInsertColumns = model->insertColumns( 0, defaultColumnCount, nullptr );
		if (!canInsertColumns)
		{
			// Ignore test
			return true;
		}
		auto columnCount = model->columnCount( nullptr );
		CHECK_EQUAL( defaultColumnCount, columnCount );

		// Initialise, assumes test data is integers
		// Need a way of comparing item data after one has been deleted 
		{
			for (int column = 0; column != columnCount; ++column)
			{
				const auto index = AbstractItemModel::ItemIndex( column, 0, nullptr );
				const auto setOk = model->item( index )->setData( index.row_,
					index.column_,
					ValueRole::roleId_,
					column );
				CHECK( setOk );
			}
		}

		const auto firstIndex = AbstractItemModel::ItemIndex( 0, 0, nullptr );
		const auto firstItem = model->item( firstIndex );
		const auto firstItemData = firstItem->getData( firstIndex.row_,
			firstIndex.column_,
			ValueRole::roleId_ );
		CHECK_EQUAL( Variant( firstIndex.column_ ), firstItemData );
		const auto lastIndex = AbstractItemModel::ItemIndex( columnCount, 0, nullptr );
		const auto lastItem = model->item( lastIndex );
		const auto lastItemData = lastItem->getData( lastIndex.row_,
			lastIndex.column_,
			ValueRole::roleId_ );
		CHECK_EQUAL( Variant( lastIndex.column_ ), lastItemData );

		// Test removing one item from the beginning of the root
		// This call to removeColumns() will invalidate pointers previously
		// obtained by item() (making them unsafe to use)
		if (columnCount > 1 && model->removeColumns( 0, 1, nullptr ))
		{
			auto newColumnCount = model->columnCount( nullptr);
			CHECK_EQUAL( columnCount - 1, newColumnCount );
			columnCount = newColumnCount;

			auto newFirstItem = model->item( firstIndex );
			const auto newFirstItemData = model->item( firstIndex )->getData(
				firstIndex.row_,
				firstIndex.column_,
				ValueRole::roleId_ );
			CHECK_EQUAL( Variant( 1 ), newFirstItemData );

			const auto newLastIndex = AbstractItemModel::ItemIndex( columnCount, 0, nullptr );
			auto newLastItem = model->item( newLastIndex );
			const auto newLastItemData = model->item( newLastIndex )->getData(
				newLastIndex.row_,
				newLastIndex.column_,
				ValueRole::roleId_ );
			CHECK_EQUAL( Variant( 24 ), newLastItemData );
		}
		else
		{
			// insertColumns/removeColumns is not implemented for this type of model
			// Skip this test
			// Note: no models in core currently run this test
			return true;
		}

		// Test removing one item from the end of the root
		if (columnCount > 1 && model->removeColumns( columnCount - 1, 1, nullptr ))
		{
			auto newColumnCount = model->columnCount( nullptr);
			CHECK_EQUAL( columnCount - 1, newColumnCount );
			columnCount = newColumnCount;

			auto newFirstItem = model->item( firstIndex );
			const auto newFirstItemData = model->item( firstIndex )->getData(
				firstIndex.row_,
				firstIndex.column_,
				ValueRole::roleId_ );
			CHECK_EQUAL( Variant( 1 ), newFirstItemData );

			const auto newLastIndex = AbstractItemModel::ItemIndex( columnCount, 0, nullptr );
			auto newLastItem = model->item( newLastIndex );
			CHECK( newLastItem != lastItem );
			const auto newLastItemData = model->item( newLastIndex )->getData(
				newLastIndex.row_,
				newLastIndex.column_,
				ValueRole::roleId_ );
			CHECK_EQUAL( Variant( 23 ), newLastItemData );
		}

		// Test removing one item from the middle of the root
		if (columnCount > 2 && model->removeColumns( 1, 1, nullptr ))
		{
			auto newColumnCount = model->columnCount( nullptr);
			CHECK_EQUAL( columnCount - 1, newColumnCount );
			columnCount = newColumnCount;

			auto newFirstItem = model->item( firstIndex );
			const auto newFirstItemData = model->item( firstIndex )->getData(
				firstIndex.row_,
				firstIndex.column_,
				ValueRole::roleId_ );
			CHECK_EQUAL( Variant( 1 ), newFirstItemData );

			const auto newLastIndex = AbstractItemModel::ItemIndex( columnCount, 0, nullptr );
			auto newLastItem = model->item( newLastIndex );
			const auto newLastItemData = model->item( newLastIndex )->getData(
				newLastIndex.row_,
				newLastIndex.column_,
				ValueRole::roleId_ );
			CHECK_EQUAL( Variant( 23 ), newLastItemData );
		}

		// Test removing multiple items from the beginning of the root
		if (columnCount > 10 && model->removeColumns( 0, 10, nullptr ))
		{
			auto newColumnCount = model->columnCount( nullptr);
			CHECK_EQUAL( columnCount - 10, newColumnCount );
			columnCount = newColumnCount;

			auto newFirstItem = model->item( firstIndex );
			const auto newFirstItemData = model->item( firstIndex )->getData(
				firstIndex.row_,
				firstIndex.column_,
				ValueRole::roleId_ );
			CHECK_EQUAL( Variant( 12 ), newFirstItemData );

			const auto newLastIndex = AbstractItemModel::ItemIndex( columnCount, 0, nullptr );
			auto newLastItem = model->item( newLastIndex );
			const auto newLastItemData = model->item( newLastIndex )->getData(
				newLastIndex.row_,
				newLastIndex.column_,
				ValueRole::roleId_ );
			CHECK_EQUAL( Variant( 23 ), newLastItemData );
		}

		// Remove one child item of another item
		if (columnCount > 0)
		{
			const auto childIndex = AbstractItemModel::ItemIndex( 0, 0, nullptr );
			const auto childItem = model->item( childIndex );
			const auto childItemData = childItem->getData( childIndex.row_,
				childIndex.column_,
				ValueRole::roleId_ );
			CHECK_EQUAL( Variant( 12 ), childItemData );

			// Note: no models in core currently run this test
			const int defaultChildCount = 10;
			const auto canAddChildren = model->insertColumns( 0, defaultChildCount, childItem );
			if (canAddChildren)
			{
				auto childCount = model->columnCount( childItem );
				CHECK_EQUAL( defaultChildCount, childCount );

				if (model->removeColumns( 0, 1, childItem ))
				{
					auto newChildItem = model->item( childIndex );
					auto newChildItemData = childItem->getData( childIndex.row_,
						childIndex.column_,
						ValueRole::roleId_ );
					CHECK_EQUAL( Variant( 11 ), childItemData );

					auto newChildCount = model->columnCount( newChildItem );
					CHECK( newChildCount == childCount - 1 );
				}
			}
		}

		// Test removal callbacks
		auto preRemovedCalled = false;
		auto preRemoved = 
			[ & ]( const AbstractItemModel::ItemIndex & parentIndex,
			int startPos,
			int count )
		{
			preRemovedCalled = true;
			CHECK( parentIndex == AbstractItemModel::ItemIndex() );
			CHECK_EQUAL( 0, startPos );
			CHECK_EQUAL( 1, count );
		};

		auto postRemovedCalled = false;
		auto postRemoved = 
			[ & ]( const AbstractItemModel::ItemIndex & parentIndex,
			int startPos,
			int count )
		{
			postRemovedCalled = true;
			CHECK( parentIndex == AbstractItemModel::ItemIndex() );
			CHECK_EQUAL( 0, startPos );
			CHECK_EQUAL( 1, count );
		};

		auto preRemovedConnection = model->connectPreColumnsRemoved( preRemoved );
		auto postRemovedConnection = model->connectPostColumnsRemoved( postRemoved );
	    if (model->removeColumns(0, 1, nullptr))
	    {
		    columnCount = model->columnCount(nullptr);
		    if (preRemovedConnection.connected())
		    {
			    CHECK_EQUAL(true, preRemovedCalled);
		    }
		    if (postRemovedConnection.connected())
			{
				CHECK_EQUAL( true, postRemovedCalled );
			}
		}

	    // Test that pointers don't change after shifting all the items
	    {
		    std::vector<AbstractItem*> originalItems;
		    for (int column = 0; column < model->columnCount(nullptr); ++column)
		    {
			    const auto index = AbstractItemModel::ItemIndex(0, column, nullptr);
			    originalItems.emplace_back(model->item(index));
		    }
		    const int removeCount = 10;
		    CHECK(model->columnCount(nullptr) >= removeCount);
		    if (model->removeColumns(0, removeCount, nullptr))
		    {
			    auto newColumnCount = model->columnCount(nullptr);
			    CHECK_EQUAL(columnCount - removeCount, newColumnCount);
			    columnCount = newColumnCount;

			    for (int column = 0; column < model->columnCount(nullptr); ++column)
			    {
				    const auto itemIndex = AbstractItemModel::ItemIndex(0, column, nullptr);
				    const auto newItem = model->item(itemIndex);
				    const size_t originalIndex = static_cast<size_t>(column + removeCount);
				    CHECK(originalItems.at(originalIndex) == newItem);
			    }
		    }
	    }

	    return true;
    }
}