#pragma once
#ifndef _REFLECTED_ENUM_MODEL_NEW_HPP
#define _REFLECTED_ENUM_MODEL_NEW_HPP

#include "core_data_model/abstract_item_model.hpp"
#include <vector>

namespace wgt
{
class PropertyAccessor;
class MetaEnumObj;
template< typename T > class ObjectHandleT;

/**
 *	Expose an enum property as a list model.
 */
class ReflectedEnumModelNew : public AbstractListModel
{
public:
	ReflectedEnumModelNew(const PropertyAccessor& pA, ObjectHandleT<MetaEnumObj> enumObj);
	virtual ~ReflectedEnumModelNew();

	void iterateRoles(const std::function<void(const char*)>&) const override;
	virtual std::vector<std::string> roles() const override;

	virtual AbstractItem* item(int row) const override;
	virtual int index(const AbstractItem* item) const override;

	virtual int rowCount() const override;
	virtual int columnCount() const override;

private:
	std::vector<AbstractItem*> items_;
};
} // end namespace wgt
#endif // _REFLECTED_ENUM_MODEL_NEW_HPP
