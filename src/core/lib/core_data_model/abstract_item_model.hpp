#ifndef ABSTRACT_ITEM_MODEL_HPP
#define ABSTRACT_ITEM_MODEL_HPP

#include "abstract_item.hpp"

namespace wgt
{
/**
 *	Base class for all types of data models.
 */
class AbstractItemModel : public AbstractItem
{
public:
	struct ItemIndex
	{
		ItemIndex( int row = -1, int column = -1, const AbstractItem * parent = nullptr )
			: row_( row )
			, column_( column )
			, parent_( parent )
		{}

		bool isValid() const
		{
			return row_ >= 0 && column_ >= 0;
		}

		bool operator==( const ItemIndex & other ) const
		{
			if (!isValid() && !other.isValid())
			{
				return true;
			}

			return row_ == other.row_ && column_ == other.column_ && parent_ == other.parent_;
		}

		bool operator!=( const ItemIndex & other ) const
		{
			return !this->operator==( other );
		}

		int row_;
		int column_;
		const AbstractItem * parent_;
	};

	/**
	 *	Data changed at *row*, with the given role and new value.
	 *	@note newValue is always the new value, for both pre- and post- callbacks.
	 */
	typedef void DataSignature( const ItemIndex & index, size_t role, const Variant & newValue );
	/**
	 *	Insert/remove into *parentIndex* from *startPos* to *startPos + count*.
	 *	@param parentIndex item inside which to do the insertion/removal.
	 *	@param startPos first row or column of insertion/removal under the parent.
	 *	@param count number of rows or columns after startPos.
	 */
	typedef void RangeSignature( const ItemIndex & parentIndex, int startPos, int count );
	typedef std::function< DataSignature > DataCallback;
	typedef std::function< RangeSignature > RangeCallback;

	virtual ~AbstractItemModel() {}

	virtual AbstractItem * item( const ItemIndex & index ) const = 0;
	/**
	 *	retrieve model index of specific item
	 *	@param item supposed to belong to the model.
	 *	@param o_Index return a valid ItemIndex if the item belongs to model, otherwise return a invalid ItemIndex.
	 */
	virtual void index( const AbstractItem * item, ItemIndex & o_Index ) const = 0;

	virtual int rowCount( const AbstractItem * item ) const = 0;
	virtual int columnCount( const AbstractItem * item ) const = 0;
	virtual bool hasChildren( const AbstractItem * item ) const { return rowCount( item ) > 0; }

	virtual bool insertRows( int row, int count, const AbstractItem * parent ) { return false; }
	virtual bool insertColumns( int column, int count, const AbstractItem * parent ) { return false; }
	virtual bool removeRows( int row, int count, const AbstractItem * parent ) { return false; }
	virtual bool removeColumns( int column, int count, const AbstractItem * parent ) { return false; }

	virtual std::vector< std::string > roles() const { return std::vector< std::string >(); }

	virtual Connection connectPreItemDataChanged( DataCallback callback ) { return Connection(); }
	virtual Connection connectPostItemDataChanged( DataCallback callback ) { return Connection(); }
	virtual Connection connectPreRowsInserted( RangeCallback callback ) { return Connection(); }
	virtual Connection connectPostRowsInserted( RangeCallback callback ) { return Connection(); }
	virtual Connection connectPreRowsRemoved( RangeCallback callback ) { return Connection(); }
	virtual Connection connectPostRowsRemoved( RangeCallback callback ) { return Connection(); }
	virtual Connection connectPreColumnsInserted( RangeCallback callback ) { return Connection(); }
	virtual Connection connectPostColumnsInserted( RangeCallback callback ) { return Connection(); }
	virtual Connection connectPreColumnsRemoved( RangeCallback callback ) { return Connection(); }
	virtual Connection connectPostColumnsRemoved( RangeCallback callback ) { return Connection(); }
};


/**
 *	Provide models with items arranged one after the other; a list.
 *	Items in rows are all connected, cannot have different items in different columns.
 */
class AbstractListModel : public AbstractItemModel
{
public:
	/**
	 *	Data changed at *row*, with the given column, role and new value.
	 *	@note newValue is always the new value, for both pre- and post- callbacks.
	 */
	typedef void DataSignature( int row, int column, size_t role, const Variant & newValue );
	/**
	 *	Insert/remove from *startRow* to *startRow + count*.
	 *	@param startRow first row of insertion/removal.
	 *	@param count number of rows after startRow.
	 */
	typedef void RangeSignature( int startRow, int count );
	typedef std::function< DataSignature > DataCallback;
	typedef std::function< RangeSignature > RangeCallback;

	virtual ~AbstractListModel() {}

	virtual AbstractItem * item( int row ) const = 0;
	virtual int index( const AbstractItem * item ) const = 0;

	virtual int rowCount() const = 0;
	virtual int columnCount() const = 0;

	virtual bool insertRows( int row, int count ) { return false; }
	virtual bool insertColumns( int column, int count ) { return false; }
	virtual bool removeRows( int row, int count ) { return false; }
	virtual bool removeColumns( int column, int count ) { return false; }

	virtual Connection connectPreItemDataChanged( DataCallback callback ) { return Connection(); }
	virtual Connection connectPostItemDataChanged( DataCallback callback ) { return Connection(); }
	virtual Connection connectPreRowsInserted( RangeCallback callback ) { return Connection(); }
	virtual Connection connectPostRowsInserted( RangeCallback callback ) { return Connection(); }
	virtual Connection connectPreRowsRemoved( RangeCallback callback ) { return Connection(); }
	virtual Connection connectPostRowsRemoved( RangeCallback callback ) { return Connection(); }

private:
	// Override functions using base class ItemIndex and hide them
	// Users can use public overloads with current class' ItemIndex type

	AbstractItem * item( const AbstractItemModel::ItemIndex & index ) const override
	{
		if (!index.isValid())
		{
			return nullptr;
		}

		if (index.parent_ != nullptr)
		{
			return nullptr;
		}

		return item( index.row_ );
	}

	void index( const AbstractItem * item, AbstractItemModel::ItemIndex & o_Index ) const override
	{
		int row = index( item );

		o_Index.row_ = row;
		o_Index.column_ = 0;
		o_Index.parent_ = nullptr;
	}

	int rowCount( const AbstractItem * item ) const override
	{
		if (item != nullptr)
		{
			return 0;
		}
		
		return rowCount();
	}

	int columnCount( const AbstractItem * item ) const override
	{
		return columnCount();
	}

	bool insertRows( int row, int count, const AbstractItem * parent ) override
	{
		if (parent != nullptr)
		{
			return false;
		}

		return insertRows( row, count );
	}

	bool insertColumns( int column, int count, const AbstractItem * parent ) override
	{
		return insertColumns( column, count );
	}

	bool removeRows( int row, int count, const AbstractItem * parent ) override
	{
		if (parent != nullptr)
		{
			return false;
		}

		return removeRows( row, count );
	}

	bool removeColumns( int column, int count, const AbstractItem * parent ) override
	{
		return removeColumns( column, count );
	}

	Connection connectPreItemDataChanged( AbstractItemModel::DataCallback callback ) override
	{ 
		return connectPreItemDataChanged( ( DataCallback )[=]( int row, int column, size_t role, const Variant & value )
		{
			callback( AbstractItemModel::ItemIndex( row, column, nullptr ), role, value );
		}); 
	}

	Connection connectPostItemDataChanged( AbstractItemModel::DataCallback callback ) override
	{ 
		return connectPostItemDataChanged( ( DataCallback )[=]( int row, int column, size_t role, const Variant & value )
		{
			callback( AbstractItemModel::ItemIndex( row, column, nullptr ), role, value );
		}); 
	}

	Connection connectPreRowsInserted( AbstractItemModel::RangeCallback callback ) override
	{ 
		return connectPreRowsInserted( ( RangeCallback )[=]( int pos, int count )
		{
			callback( AbstractItemModel::ItemIndex(), pos, count );
		}); 
	}

	Connection connectPostRowsInserted( AbstractItemModel::RangeCallback callback ) override
	{ 
		return connectPostRowsInserted( ( RangeCallback )[=]( int pos, int count )
		{
			callback( AbstractItemModel::ItemIndex(), pos, count );
		}); 
	}

	Connection connectPreRowsRemoved( AbstractItemModel::RangeCallback callback ) override
	{ 
		return connectPreRowsRemoved( ( RangeCallback )[=]( int pos, int count )
		{
			callback( AbstractItemModel::ItemIndex(), pos, count );
		}); 
	}

	Connection connectPostRowsRemoved( AbstractItemModel::RangeCallback callback ) override
	{ 
		return connectPostRowsRemoved( ( RangeCallback )[=]( int pos, int count )
		{
			callback( AbstractItemModel::ItemIndex(), pos, count );
		}); 
	}
};


/**
 *	Provide models with items arranged in a hierarchy; a tree.
 *	Items in rows are all connected, cannot have different items in different columns.
 */
class AbstractTreeModel : public AbstractItemModel
{
public:
	struct ItemIndex
	{
		ItemIndex( int row = -1, const AbstractItem * parent = nullptr )
			: row_( row )
			, parent_( parent )
		{}

		bool isValid() const
		{
			return row_ >= 0;
		}

		bool operator==( const ItemIndex & other ) const
		{
			if (!isValid() && !other.isValid())
			{
				return true;
			}

			return row_ == other.row_ && parent_ == other.parent_;
		}

		bool operator!=( const ItemIndex & other ) const
		{
			return !this->operator==( other );
		}

		int row_;
		const AbstractItem * parent_;
	};

	/**
	 *	Data changed at *index*, with the given column, role and new value.
	 *	@note value is always the new value, for both pre- and post- callbacks.
	 */
	typedef void DataSignature( const ItemIndex & index, int column, size_t role, const Variant & value );
	/**
	 *	Insert/remove into the item at *parentIndex* from *startRow* to *startRow + count*.
	 *	@param parentIndex item inside which to do the insertion/removal.
	 *	@param startRow first row of insertion/removal under the parent.
	 *	@param count number of rows after startRow.
	 */
	typedef void RangeSignature( const ItemIndex & parentIndex, int startRow, int count );
	typedef std::function< DataSignature > DataCallback;
	typedef std::function< RangeSignature > RangeCallback;

	virtual ~AbstractTreeModel() {}

	virtual AbstractItem * item( const ItemIndex & index ) const = 0;
	virtual ItemIndex index( const AbstractItem * item ) const = 0;

	virtual int rowCount( const AbstractItem * item ) const override = 0;
	virtual int columnCount() const = 0;

	virtual bool insertColumns( int column, int count ) { return false; }
	virtual bool removeColumns( int column, int count ) { return false; }

	virtual Connection connectPreItemDataChanged( DataCallback callback ) { return Connection(); }
	virtual Connection connectPostItemDataChanged( DataCallback callback ) { return Connection(); }
	virtual Connection connectPreRowsInserted( RangeCallback callback ) { return Connection(); }
	virtual Connection connectPostRowsInserted( RangeCallback callback ) { return Connection(); }
	virtual Connection connectPreRowsRemoved( RangeCallback callback ) { return Connection(); }
	virtual Connection connectPostRowsRemoved( RangeCallback callback ) { return Connection(); }

private:
	// Override functions using base class ItemIndex and hide them
	// Users can use public overloads with current class' ItemIndex type

	AbstractItem * item( const AbstractItemModel::ItemIndex & index ) const override
	{
		if (!index.isValid())
		{
			return nullptr;
		}

		return item( ItemIndex( index.row_, index.parent_ ) );
	}

	void index( const AbstractItem * item, AbstractItemModel::ItemIndex & o_Index ) const override
	{
		ItemIndex index = this->index( item );

		o_Index.row_ = index.row_;
		o_Index.column_ = 0;
		o_Index.parent_ = index.parent_;
	}

	int columnCount( const AbstractItem * item ) const override
	{
		return columnCount();
	}

	bool insertColumns( int column, int count, const AbstractItem * parent ) override
	{
		return insertColumns( column, count );
	}

	bool removeColumns( int column, int count, const AbstractItem * parent ) override
	{
		return removeColumns( column, count );
	}

	Connection connectPreItemDataChanged( AbstractItemModel::DataCallback callback ) override
	{ 
		return connectPreItemDataChanged( ( DataCallback )[=]( const ItemIndex & index, int column, size_t role, const Variant & value )
		{
			callback( AbstractItemModel::ItemIndex( index.row_, column, index.parent_ ), role, value );
		}); 
	}
	
	Connection connectPostItemDataChanged( AbstractItemModel::DataCallback callback ) override
	{ 
		return connectPostItemDataChanged( ( DataCallback )[=]( const ItemIndex & index, int column, size_t role, const Variant & value )
		{
			callback( AbstractItemModel::ItemIndex( index.row_, column, index.parent_ ), role, value );
		}); 
	}
	
	Connection connectPreRowsInserted( AbstractItemModel::RangeCallback callback ) override
	{ 
		return connectPreRowsInserted( ( RangeCallback )[=]( const ItemIndex & index, int pos, int count )
		{
			callback( AbstractItemModel::ItemIndex( index.row_, 0, index.parent_ ), pos, count );
		}); 
	}
	
	Connection connectPostRowsInserted( AbstractItemModel::RangeCallback callback ) override
	{ 
		return connectPostRowsInserted( ( RangeCallback )[=]( const ItemIndex & index, int pos, int count )
		{
			callback( AbstractItemModel::ItemIndex( index.row_, 0, index.parent_ ), pos, count );
		}); 
	}
	
	Connection connectPreRowsRemoved( AbstractItemModel::RangeCallback callback ) override
	{ 
		return connectPreRowsRemoved( ( RangeCallback )[=]( const ItemIndex & index, int pos, int count )
		{
			callback( AbstractItemModel::ItemIndex( index.row_, 0, index.parent_ ), pos, count );
		}); 
	}
	
	Connection connectPostRowsRemoved( AbstractItemModel::RangeCallback callback ) override
	{ 
		return connectPostRowsRemoved( ( RangeCallback )[=]( const ItemIndex & index, int pos, int count )
		{
			callback( AbstractItemModel::ItemIndex( index.row_, 0, index.parent_ ), pos, count );
		}); 
	}
};


/**
 *	Provide models with items arranged in a grid; a table.
 *	Items in each (row, column) can be unrelated.
 */
class AbstractTableModel : public AbstractItemModel
{
public:
	struct ItemIndex
	{
		ItemIndex( int row = -1, int column = -1 )
			: row_( row )
			, column_( column )
		{}

		bool isValid() const
		{
			return row_ >= 0 && column_ >= 0;
		}

		bool operator==( const ItemIndex & other ) const
		{
			if (!isValid() && !other.isValid())
			{
				return true;
			}

			return row_ == other.row_ && column_ == other.column_;
		}

		bool operator!=( const ItemIndex & other ) const
		{
			return !this->operator==( other );
		}

		int row_;
		int column_;
	};

	/**
	 *	Data changed at *index*, with the given role and new value.
	 *	@note newValue is always the new value, for both pre- and post- callbacks.
	 */
	typedef void DataSignature( const ItemIndex & index, size_t role, const Variant & newValue );
	/**
	 *	Insert/remove from *startPos* to *startPos + count*.
	 *	@param startPos first row or column of insertion/removal.
	 *	@param count number of rows or columns after startPos.
	 */
	typedef void RangeSignature( int startPos, int count );
	typedef std::function< DataSignature > DataCallback;
	typedef std::function< RangeSignature > RangeCallback;

	virtual ~AbstractTableModel() {}

	virtual AbstractItem * item( const ItemIndex & index ) const = 0;
	virtual ItemIndex index( const AbstractItem * item ) const = 0;

	virtual int rowCount() const = 0;
	virtual int columnCount() const = 0;

	virtual bool insertRows( int row, int count ) { return false; }
	virtual bool insertColumns( int column, int count ) { return false; }
	virtual bool removeRows( int row, int count ) { return false; }
	virtual bool removeColumns( int column, int count ) { return false; }

	virtual Connection connectPreItemDataChanged( DataCallback callback ) { return Connection(); }
	virtual Connection connectPostItemDataChanged( DataCallback callback ) { return Connection(); }
	virtual Connection connectPreRowsInserted( RangeCallback callback ) { return Connection(); }
	virtual Connection connectPostRowsInserted( RangeCallback callback ) { return Connection(); }
	virtual Connection connectPreRowsRemoved( RangeCallback callback ) { return Connection(); }
	virtual Connection connectPostRowsRemoved( RangeCallback callback ) { return Connection(); }
	virtual Connection connectPreColumnsInserted( RangeCallback callback ) { return Connection(); }
	virtual Connection connectPostColumnsInserted( RangeCallback callback ) { return Connection(); }
	virtual Connection connectPreColumnsRemoved( RangeCallback callback ) { return Connection(); }
	virtual Connection connectPostColumnsRemoved( RangeCallback callback ) { return Connection(); }

private:
	// Override functions using base class ItemIndex and hide them
	// Users can use public overloads with current class' ItemIndex type

	AbstractItem * item( const AbstractItemModel::ItemIndex & index ) const override
	{
		if (!index.isValid())
		{
			return nullptr;
		}

		if (index.parent_ != nullptr)
		{
			return nullptr;
		}

		return item( ItemIndex( index.row_, index.column_ ) );
	}

	void index( const AbstractItem * item, AbstractItemModel::ItemIndex & o_Index ) const override
	{
		ItemIndex index = this->index( item );

		o_Index.row_ = index.row_;
		o_Index.column_ = index.column_;
		o_Index.parent_ = nullptr;
	}

	int rowCount( const AbstractItem * item ) const override
	{
		if (item != nullptr)
		{
			return 0;
		}

		return rowCount();
	}

	int columnCount( const AbstractItem * item ) const override
	{
		return columnCount();
	}

	bool insertRows( int row, int count, const AbstractItem * parent ) override
	{
		if (parent != nullptr)
		{
			return false;
		}

		return insertRows( row, count );
	}

	bool insertColumns( int column, int count, const AbstractItem * parent ) override
	{
		if (parent != nullptr)
		{
			return false;
		}

		return insertColumns( column, count );
	}

	bool removeRows( int row, int count, const AbstractItem * parent ) override
	{
		if (parent != nullptr)
		{
			return false;
		}

		return removeRows( row, count );
	}

	bool removeColumns( int column, int count, const AbstractItem * parent ) override
	{
		if (parent != nullptr)
		{
			return false;
		}

		return removeColumns( column, count );
	}

	Connection connectPreItemDataChanged( AbstractItemModel::DataCallback callback ) override
	{ 
		return connectPreItemDataChanged( ( DataCallback )[=]( const ItemIndex & index, size_t role, const Variant & value )
		{
			callback( AbstractItemModel::ItemIndex( index.row_, index.column_, nullptr ), role, value );
		}); 
	}

	Connection connectPostItemDataChanged( AbstractItemModel::DataCallback callback ) override
	{ 
		return connectPostItemDataChanged( ( DataCallback )[=]( const ItemIndex & index, size_t role, const Variant & value )
		{
			callback( AbstractItemModel::ItemIndex( index.row_, index.column_, nullptr ), role, value );
		}); 
	}

	Connection connectPreRowsInserted( AbstractItemModel::RangeCallback callback ) override
	{ 
		return connectPreRowsInserted( ( RangeCallback )[=]( int pos, int count )
		{
			callback( AbstractItemModel::ItemIndex(), pos, count );
		}); 
	}

	Connection connectPostRowsInserted( AbstractItemModel::RangeCallback callback ) override
	{ 
		return connectPostRowsInserted( ( RangeCallback )[=]( int pos, int count )
		{
			callback( AbstractItemModel::ItemIndex(), pos, count );
		}); 
	}

	Connection connectPreRowsRemoved( AbstractItemModel::RangeCallback callback ) override
	{ 
		return connectPreRowsRemoved( ( RangeCallback )[=]( int pos, int count )
		{
			callback( AbstractItemModel::ItemIndex(), pos, count );
		}); 
	}

	Connection connectPostRowsRemoved( AbstractItemModel::RangeCallback callback ) override
	{ 
		return connectPostRowsRemoved( ( RangeCallback )[=]( int pos, int count )
		{
			callback( AbstractItemModel::ItemIndex(), pos, count );
		}); 
	}

	Connection connectPreColumnsInserted( AbstractItemModel::RangeCallback callback ) override
	{ 
		return connectPreColumnsInserted( ( RangeCallback )[=]( int pos, int count )
		{
			callback( AbstractItemModel::ItemIndex(), pos, count );
		}); 
	}

	Connection connectPostColumnsInserted( AbstractItemModel::RangeCallback callback ) override
	{ 
		return connectPostColumnsInserted( ( RangeCallback )[=]( int pos, int count )
		{
			callback( AbstractItemModel::ItemIndex(), pos, count );
		}); 
	}

	Connection connectPreColumnsRemoved( AbstractItemModel::RangeCallback callback ) override
	{ 
		return connectPreColumnsRemoved( ( RangeCallback )[=]( int pos, int count )
		{
			callback( AbstractItemModel::ItemIndex(), pos, count );
		}); 
	}

	Connection connectPostColumnsRemoved( AbstractItemModel::RangeCallback callback ) override
	{ 
		return connectPostColumnsRemoved( ( RangeCallback )[=]( int pos, int count )
		{
			callback( AbstractItemModel::ItemIndex(), pos, count );
		}); 
	}
};
} // end namespace wgt
#endif//ABSTRACT_ITEM_MODEL_HPP
