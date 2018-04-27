#include "ref_object_ids.hpp"

namespace wgt
{
const RefObjectId& RefObjectIds::current() const
{
	TF_ASSERT(ids_.size() > current_);
	return ids_.at(current_);
}

const RefObjectId& RefObjectIds::next()
{
	if (++current_ >= ids_.size())
	{
		ids_.emplace_back(RefObjectId::generate());
	}
	return current();
}

void RefObjectIds::begin()
{
	current_ = -1;
}

} // end namespace wgt
