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
class InsertDefaultCommandArgument
{
	DECLARE_REFLECTED

public:
	/**
	 *	Construct with the builder pattern, using set functions below.
	 */
	InsertDefaultCommandArgument();

	void setModel(CollectionModel& model);
	void setKey(const Variant& key);

	AbstractListModel* pModel_;
	Variant key_;
};

} // end namespace wgt
