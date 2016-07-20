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
			CHECK( newRowCount == rowCount + 1 );
			rowCount = newRowCount;
		}

		// Test inserting one item at the end of the root
		if (rowCount > 0)
		{
			auto index1 = AbstractItemModel::ItemIndex( rowCount, 0, nullptr );
			auto item1 = model->item( index1 );
			if (model->insertRows( rowCount, 1, nullptr ))
			{
				auto newRowCount = model->rowCount( nullptr);
				CHECK( newRowCount == rowCount + 1 );
				rowCount = newRowCount;
				auto newItem1 = model->item( index1 );
				CHECK( newItem1 == item1 );
				auto index2 = AbstractItemModel::ItemIndex( rowCount, 0, nullptr );
				auto item2 = model->item( index2 );
				CHECK( item1 != item2 );
			}
		}

		// Test inserting one item in the middle of the root
		if (rowCount > 1)
		{
			auto index1 = AbstractItemModel::ItemIndex( 0, 0, nullptr );
			auto item1 = model->item( index1 );
			auto index2 = AbstractItemModel::ItemIndex( rowCount, 0, nullptr );
			auto item2 = model->item( index2 );
			if (model->insertRows( 1, 1, nullptr ))
			{
				auto newRowCount = model->rowCount( nullptr);
				CHECK( newRowCount == rowCount + 1 );
				rowCount = newRowCount;
				auto newItem1 = model->item( index1 );
				CHECK( newItem1 == item1 );
				auto newItem2 = model->item( index2 );
				CHECK( newItem2 != item2 );
				auto index3 = AbstractItemModel::ItemIndex( rowCount, 0, nullptr );
				auto item3 = model->item( index3 );
				CHECK( item3 == item2 );
			}
		}

		// Test inserting multiple items at the beginning of the root
		if (model->insertRows( 0, 10, nullptr ))
		{
			auto newRowCount = model->rowCount( nullptr);
			CHECK( newRowCount == rowCount + 10 );
			rowCount = newRowCount;
		}

		// Insert one item as a child of another item
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
				CHECK( newChildCount == childCount + 1 );
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
			CHECK( startPos == 0 );
			CHECK( count == 1 );
		};

		auto postInsertedCalled = false;
		auto postInserted = 
			[ & ]( const AbstractItemModel::ItemIndex & parentIndex,
			int startPos,
			int count )
		{
			postInsertedCalled = true;
			CHECK( parentIndex == AbstractItemModel::ItemIndex() );
			CHECK( startPos == 0 );
			CHECK( count == 1 );
		};

		auto preInsertedConnection = model->connectPreRowsInserted( preInserted );
		auto postInsertedConnection = model->connectPostRowsInserted( postInserted );
		if (model->insertRows( 0, 1, nullptr ))
		{
			if (preInsertedConnection.connected())
			{
				CHECK( preInsertedCalled == true );
			}
			if (postInsertedConnection.connected())
			{
				CHECK( postInsertedCalled == true );
			}
		}

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
			CHECK( newColumnCount == columnCount + 1 );
			columnCount = newColumnCount;
		}

		// Test inserting one item at the end of the root
		if (columnCount > 0)
		{
			auto index1 = AbstractItemModel::ItemIndex( 0, columnCount, nullptr );
			auto item1 = model->item( index1 );
			if (model->insertColumns( columnCount, 1, nullptr ))
			{
				auto newColumnCount = model->columnCount( nullptr);
				CHECK( newColumnCount == columnCount + 1 );
				columnCount = newColumnCount;
				auto newItem1 = model->item( index1 );
				CHECK( newItem1 == item1 );
				auto index2 = AbstractItemModel::ItemIndex( 0, columnCount, nullptr );
				auto item2 = model->item( index2 );
				CHECK( item1 != item2 );
			}
		}

		// Test inserting one item in the middle of the root
		if (columnCount > 1)
		{
			auto index1 = AbstractItemModel::ItemIndex( 0, 0, nullptr );
			auto item1 = model->item( index1 );
			auto index2 = AbstractItemModel::ItemIndex( 0, columnCount, nullptr );
			auto item2 = model->item( index2 );
			if (model->insertColumns( 1, 1, nullptr ))
			{
				auto newColumnCount = model->columnCount( nullptr);
				CHECK( newColumnCount == columnCount + 1 );
				columnCount = newColumnCount;
				auto newItem1 = model->item( index1 );
				CHECK( newItem1 == item1 );
				auto newItem2 = model->item( index2 );
				CHECK( newItem2 != item2 );
				auto index3 = AbstractItemModel::ItemIndex( 0, columnCount, nullptr );
				auto item3 = model->item( index3 );
				CHECK( item3 == item2 );
			}
		}

		// Test inserting multiple items at the beginning of the root
		if (model->insertColumns( 0, 10, nullptr ))
		{
			auto newColumnCount = model->columnCount( nullptr);
			CHECK( newColumnCount == columnCount + 10 );
			columnCount = newColumnCount;
		}

		// Insert one item as a child of another item
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
				CHECK( newChildCount == childCount + 1 );
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
			CHECK( startPos == 0 );
			CHECK( count == 1 );
		};

		auto postInsertedCalled = false;
		auto postInserted = 
			[ & ]( const AbstractItemModel::ItemIndex & parentIndex,
			int startPos,
			int count )
		{
			postInsertedCalled = true;
			CHECK( parentIndex == AbstractItemModel::ItemIndex() );
			CHECK( startPos == 0 );
			CHECK( count == 1 );
		};

		auto preInsertedConnection = model->connectPreColumnsInserted( preInserted );
		auto postInsertedConnection = model->connectPostColumnsInserted( postInserted );
		if (model->insertColumns( 0, 1, nullptr ))
		{
			if (preInsertedConnection.connected())
			{
				CHECK( preInsertedCalled == true );
			}
			if (postInsertedConnection.connected())
			{
				CHECK( postInsertedCalled == true );
			}
		}

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
		model->insertRows( 0, 25, nullptr );
		auto rowCount = model->rowCount( nullptr );

		auto firstIndex = AbstractItemModel::ItemIndex( 0, 0, nullptr );
		auto firstItem = model->item( firstIndex );
		auto lastIndex = AbstractItemModel::ItemIndex( rowCount, 0, nullptr );
		auto lastItem = model->item( lastIndex );

		// Test removing one item from the beginning of the root
		if (rowCount > 1 && model->removeRows( 0, 1, nullptr ))
		{
			auto newRowCount = model->rowCount( nullptr);
			CHECK( newRowCount == rowCount - 1 );
			rowCount = newRowCount;

			auto newFirstItem = model->item( firstIndex );
			CHECK( newFirstItem != firstItem );
			firstItem = newFirstItem;

			lastIndex = AbstractItemModel::ItemIndex( rowCount, 0, nullptr );
			auto newLastItem = model->item( lastIndex );
			CHECK( newLastItem == lastItem );
		}

		// Test removing one item from the end of the root
		if (rowCount > 1 && model->removeRows( rowCount - 1, 1, nullptr ))
		{
			auto newRowCount = model->rowCount( nullptr);
			CHECK( newRowCount == rowCount - 1 );
			rowCount = newRowCount;

			auto newFirstItem = model->item( firstIndex );
			CHECK( newFirstItem == firstItem );

			lastIndex = AbstractItemModel::ItemIndex( rowCount, 0, nullptr );
			auto newLastItem = model->item( lastIndex );
			CHECK( newLastItem != lastItem );
			lastItem = newLastItem;
		}

		// Test removing one item from the middle of the root
		if (rowCount > 2 && model->removeRows( 1, 1, nullptr ))
		{
			auto newRowCount = model->rowCount( nullptr);
			CHECK( newRowCount == rowCount - 1 );
			rowCount = newRowCount;

			auto newFirstItem = model->item( firstIndex );
			CHECK( newFirstItem == firstItem );

			lastIndex = AbstractItemModel::ItemIndex( rowCount, 0, nullptr );
			auto newLastItem = model->item( lastIndex );
			CHECK( newLastItem == lastItem );
		}

		// Test removing multiple items from the beginning of the root
		if (rowCount > 10 && model->removeRows( 0, 10, nullptr ))
		{
			auto newRowCount = model->rowCount( nullptr);
			CHECK( newRowCount == rowCount - 10 );
			rowCount = newRowCount;

			auto newFirstItem = model->item( firstIndex );
			CHECK( newFirstItem != firstItem );
			firstItem = newFirstItem;

			lastIndex = AbstractItemModel::ItemIndex( rowCount, 0, nullptr );
			auto newLastItem = model->item( lastIndex );
			CHECK( newLastItem == lastItem );
		}

		// Remove one child item of another item
		if (rowCount > 0)
		{
			auto childIndex = AbstractItemModel::ItemIndex( 0, 0, nullptr );
			auto childItem = model->item( childIndex );
			model->insertRows( 0, 10, childItem );
			auto childCount = model->rowCount( childItem );
			if (model->removeRows( 0, 1, childItem ))
			{
				auto newChildItem = model->item( childIndex );
				CHECK( newChildItem == childItem );
				auto newChildCount = model->columnCount( newChildItem );
				CHECK( newChildCount == childCount - 1 );
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
			CHECK( startPos == 0 );
			CHECK( count == 1 );
		};

		auto postRemovedCalled = false;
		auto postRemoved = 
			[ & ]( const AbstractItemModel::ItemIndex & parentIndex,
			int startPos,
			int count )
		{
			postRemovedCalled = true;
			CHECK( parentIndex == AbstractItemModel::ItemIndex() );
			CHECK( startPos == 0 );
			CHECK( count == 1 );
		};

		auto preRemovedConnection = model->connectPreRowsRemoved( preRemoved );
		auto postRemovedConnection = model->connectPostRowsRemoved( postRemoved );
		if (model->insertColumns( 0, 1, nullptr ))
		{
			if (preRemovedConnection.connected())
			{
				CHECK( preRemovedCalled == true );
			}
			if (postRemovedConnection.connected())
			{
				CHECK( postRemovedCalled == true );
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
		model->insertColumns( 0, 25, nullptr );
		auto columnCount = model->columnCount( nullptr );

		auto firstIndex = AbstractItemModel::ItemIndex( 0, 0, nullptr );
		auto firstItem = model->item( firstIndex );
		auto lastIndex = AbstractItemModel::ItemIndex( columnCount, 0, nullptr );
		auto lastItem = model->item( lastIndex );

		// Test removing one item from the beginning of the root
		if (columnCount > 1 && model->removeColumns( 0, 1, nullptr ))
		{
			auto newColumnCount = model->columnCount( nullptr);
			CHECK( newColumnCount == columnCount - 1 );
			columnCount = newColumnCount;

			auto newFirstItem = model->item( firstIndex );
			CHECK( newFirstItem != firstItem );
			firstItem = newFirstItem;

			lastIndex = AbstractItemModel::ItemIndex( columnCount, 0, nullptr );
			auto newLastItem = model->item( lastIndex );
			CHECK( newLastItem == lastItem );
		}

		// Test removing one item from the end of the root
		if (columnCount > 1 && model->removeColumns( columnCount - 1, 1, nullptr ))
		{
			auto newColumnCount = model->columnCount( nullptr);
			CHECK( newColumnCount == columnCount - 1 );
			columnCount = newColumnCount;

			auto newFirstItem = model->item( firstIndex );
			CHECK( newFirstItem == firstItem );

			lastIndex = AbstractItemModel::ItemIndex( columnCount, 0, nullptr );
			auto newLastItem = model->item( lastIndex );
			CHECK( newLastItem != lastItem );
			lastItem = newLastItem;
		}

		// Test removing one item from the middle of the root
		if (columnCount > 2 && model->removeColumns( 1, 1, nullptr ))
		{
			auto newColumnCount = model->columnCount( nullptr);
			CHECK( newColumnCount == columnCount - 1 );
			columnCount = newColumnCount;

			auto newFirstItem = model->item( firstIndex );
			CHECK( newFirstItem == firstItem );

			lastIndex = AbstractItemModel::ItemIndex( columnCount, 0, nullptr );
			auto newLastItem = model->item( lastIndex );
			CHECK( newLastItem == lastItem );
		}

		// Test removing multiple items from the beginning of the root
		if (columnCount > 10 && model->removeColumns( 0, 10, nullptr ))
		{
			auto newColumnCount = model->columnCount( nullptr);
			CHECK( newColumnCount == columnCount - 10 );
			columnCount = newColumnCount;

			auto newFirstItem = model->item( firstIndex );
			CHECK( newFirstItem != firstItem );
			firstItem = newFirstItem;

			lastIndex = AbstractItemModel::ItemIndex( columnCount, 0, nullptr );
			auto newLastItem = model->item( lastIndex );
			CHECK( newLastItem == lastItem );
		}

		// Remove one child item of another item
		if (columnCount > 0)
		{
			auto childIndex = AbstractItemModel::ItemIndex( 0, 0, nullptr );
			auto childItem = model->item( childIndex );
			model->insertColumns( 0, 10, childItem );
			auto childCount = model->columnCount( childItem );
			if (model->removeColumns( 0, 1, childItem ))
			{
				auto newChildItem = model->item( childIndex );
				CHECK( newChildItem == childItem );
				auto newChildCount = model->columnCount( newChildItem );
				CHECK( newChildCount == childCount - 1 );
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
			CHECK( startPos == 0 );
			CHECK( count == 1 );
		};

		auto postRemovedCalled = false;
		auto postRemoved = 
			[ & ]( const AbstractItemModel::ItemIndex & parentIndex,
			int startPos,
			int count )
		{
			postRemovedCalled = true;
			CHECK( parentIndex == AbstractItemModel::ItemIndex() );
			CHECK( startPos == 0 );
			CHECK( count == 1 );
		};

		auto preRemovedConnection = model->connectPreColumnsRemoved( preRemoved );
		auto postRemovedConnection = model->connectPostColumnsRemoved( postRemoved );
		if (model->insertColumns( 0, 1, nullptr ))
		{
			if (preRemovedConnection.connected())
			{
				CHECK( preRemovedCalled == true );
			}
			if (postRemovedConnection.connected())
			{
				CHECK( postRemovedCalled == true );
			}
		}

		return true;
	}
}