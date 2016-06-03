#ifndef I_ITEM_FILTER_HPP
#define I_ITEM_FILTER_HPP

#include "core_common/signal.hpp"

namespace wgt
{
class IItem;

/**
 *	IItemFilter
 *  A filter interface that can be implemented to provide advanced
 *  filtering functionality to the IItem-driven QML filtering components and
 *  their corresponding data models.
 */
class IItemFilter
{
	typedef Signal< void( void ) > SignalVoid;

public:
	virtual ~IItemFilter() {}
	
	virtual bool checkFilter( const IItem * item ) = 0;

	virtual void setRole( unsigned int roleId ) = 0;

	virtual bool filterDescendantsOfMatchingItems() { return false; }

	SignalVoid signalFilterChanged;
};
} // end namespace wgt
#endif // I_ITEM_FILTER_HPP
