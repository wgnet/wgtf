#ifndef I_TREE_MODEL_HPP
#define I_TREE_MODEL_HPP

#include "core_common/signal.hpp"

namespace wgt
{
class IItem;
class Variant;

/**
 *	This is our generic data model of a tree.
 *	Composed of a collection of IItems.
 *	Data can be get/set.
 *	There is no concept of rows or columns like Qt. 
 */
class ITreeModel
{
	typedef Signal< void( int, size_t, const Variant & ) > SignalModelData;
	typedef Signal< void( const IItem *, int, size_t, const Variant & ) > SignalItemData;
	typedef Signal< void( const IItem *, size_t, size_t ) > SignalCount;
	typedef Signal< void( void ) > SignalVoid;

public:
	typedef std::pair< size_t, const IItem * > ItemIndex;

	virtual ~ITreeModel()
	{
		signalDestructing();
	}

	virtual IItem * item( size_t index, const IItem * parent ) const = 0;
	IItem * item( ItemIndex index ) const;
	virtual ItemIndex index( const IItem * item ) const = 0;

	virtual bool empty( const IItem * item ) const;
	virtual size_t size( const IItem * item ) const = 0;
	virtual int columnCount() const = 0;

	// ITreeModel signals
	virtual Variant getData( int column, size_t roleId ) const;
	virtual bool setData( int column, size_t roleId, const Variant & data );

	SignalModelData signalModelDataChanged;
	SignalItemData signalPreItemDataChanged;
	SignalItemData signalPostItemDataChanged;
	SignalCount signalPreItemsInserted;
	SignalCount signalPostItemsInserted;
	SignalCount signalPreItemsRemoved;
	SignalCount signalPostItemsRemoved;
	SignalVoid signalDestructing;

};
} // end namespace wgt
#endif // I_TREE_MODEL_HPP
