#pragma once
#ifndef _REFLECTED_ENUM_MODEL_NEW_HPP
#define _REFLECTED_ENUM_MODEL_NEW_HPP

#include "core_data_model/abstract_item_model.hpp"
#include <vector>

namespace wgt
{
class PropertyAccessor;
class MetaEnumObj;


/**
 *	Expose an enum property as a list model.
 */
class ReflectedEnumModelNew : public AbstractListModel
{
public:
	ReflectedEnumModelNew( const PropertyAccessor & pA, const MetaEnumObj * enumObj );
	virtual ~ReflectedEnumModelNew();

	virtual AbstractItem * item( int row ) const override;
	virtual int index( const AbstractItem * item ) const override;

	virtual int rowCount() const override;
	virtual int columnCount() const override;

private:
	std::vector< AbstractItem * > items_;
};
} // end namespace wgt
#endif // _REFLECTED_ENUM_MODEL_NEW_HPP
