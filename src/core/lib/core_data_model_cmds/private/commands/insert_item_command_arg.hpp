#pragma once

#include "core_reflection/reflected_object.hpp"
#include "core_variant/variant.hpp"

namespace wgt
{
class CollectionModel;
class AbstractListModel;

/**
 *	Command data for inserting items on CollectionModel.
 */
class InsertItemCommandArgument
{
	DECLARE_REFLECTED

public:
	/**
	 *	Construct with the builder pattern, using set functions below.
	 */
	InsertItemCommandArgument();

	void setModel(CollectionModel& model);
	void setKey(const Variant& key);
	void setValue(const Variant& value);

	AbstractListModel* pModel_;
	Variant key_;
	Variant value_;
};

} // end namespace wgt
