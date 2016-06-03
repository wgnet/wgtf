#ifndef ABSTRACT_ITEM_HPP
#define ABSTRACT_ITEM_HPP

#include "core_common/signal.hpp"
#include "core_variant/variant.hpp"


namespace wgt
{
/**
 *	Stores data for an entry in a data model or on the data model itself.
 */
class AbstractItem
{
public:
	typedef void DataSignature( int row, int column, size_t role, const Variant & value );
	typedef std::function< DataSignature > DataCallback;

	virtual ~AbstractItem() {}

	virtual Variant getData( int row, int column, size_t roleId ) const { return Variant(); }
	virtual bool setData( int row, int column, size_t roleId, const Variant & data ) { return false; }

	virtual Connection connectPreDataChanged( DataCallback callback ) { return Connection(); }
	virtual Connection connectPostDataChanged( DataCallback callback ) { return Connection(); }
};

class AbstractListItem : public AbstractItem
{
public:
	typedef void DataSignature( int column, size_t role, const Variant & value );
	typedef std::function< DataSignature > DataCallback;

	virtual ~AbstractListItem() {}

	virtual Variant getData( int column, size_t roleId ) const { return Variant(); }
	virtual bool setData( int column, size_t roleId, const Variant & data ) { return false; }

	virtual Connection connectPreDataChanged( DataCallback callback ) { return Connection(); }
	virtual Connection connectPostDataChanged( DataCallback callback ) { return Connection(); }

private:
	Variant getData( int row, int column, size_t roleId ) const override
	{
		return getData( column, roleId );
	}

	bool setData( int row, int column, size_t roleId, const Variant & data ) override
	{
		return setData( column, roleId, data );
	}

	Connection connectPreDataChanged( AbstractItem::DataCallback callback ) override
	{ 
		return connectPreDataChanged( ( DataCallback )[=]( int column, size_t role, const Variant & value )
		{
			callback( 0, column, role, value );
		}); 
	}

	Connection connectPostDataChanged( AbstractItem::DataCallback callback ) override
	{ 
		return connectPostDataChanged( ( DataCallback )[=]( int column, size_t role, const Variant & value )
		{
			callback( 0, column, role, value );
		}); 
	}
};

typedef AbstractListItem AbstractTreeItem;

class AbstractTableItem : public AbstractItem
{
public:
	typedef void DataSignature( size_t role, const Variant & value );
	typedef std::function< DataSignature > DataCallback;

	virtual ~AbstractTableItem() {}

	virtual Variant getData( size_t roleId ) const { return Variant(); }
	virtual bool setData( size_t roleId, const Variant & data ) { return false; }

	virtual Connection connectPreDataChanged( DataCallback callback ) { return Connection(); }
	virtual Connection connectPostDataChanged( DataCallback callback ) { return Connection(); }

private:
	Variant getData( int row, int column, size_t roleId ) const override
	{
		return getData( roleId );
	}

	bool setData( int row, int column, size_t roleId, const Variant & data ) override
	{
		return setData( roleId, data );
	}

	Connection connectPreDataChanged( AbstractItem::DataCallback callback ) override
	{ 
		return connectPreDataChanged( ( DataCallback )[=]( size_t role, const Variant & value )
		{
			callback( 0, 0, role, value );
		}); 
	}

	Connection connectPostDataChanged( AbstractItem::DataCallback callback ) override
	{ 
		return connectPostDataChanged( ( DataCallback )[=]( size_t role, const Variant & value )
		{
			callback( 0, 0, role, value );
		}); 
	}
};
} // end namespace wgt
#endif//ABSTRACT_ITEM_HPP
