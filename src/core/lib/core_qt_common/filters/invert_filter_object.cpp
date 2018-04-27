#include "invert_filter_object.hpp"
#include "core_logging/logging.hpp"

namespace wgt
{
//------------------------------------------------------------------------------
InvertFilterObject::InvertFilterObject()
	: QtFilterObject(false)
{
}

//------------------------------------------------------------------------------
InvertFilterObject::~InvertFilterObject()
{
}

//------------------------------------------------------------------------------
bool InvertFilterObject::filterAcceptsRowValid() const
{
	return true;
}

//------------------------------------------------------------------------------
bool InvertFilterObject::hasFilter() const
{
	return pSubFilter_ != nullptr;
}

//------------------------------------------------------------------------------
bool InvertFilterObject::filterAcceptsRow(const Variant& variant) const
{
	return !pSubFilter_->filterAcceptsRow(variant);
}

//------------------------------------------------------------------------------
QObject* InvertFilterObject::getSubFilter() const
{
	return pSubFilter_;
}

//------------------------------------------------------------------------------
void InvertFilterObject::setSubFilter(QObject* subFilter)
{
	pSubFilter_ = dynamic_cast<QtFilterObject*>(subFilter);

	if (subFilter && !pSubFilter_)
	{
		NGT_ERROR_MSG("InvertFilterObject: filter must be native");
	}

	if (pSubFilter_ && !pSubFilter_->filterAcceptsRowValid())
	{
		NGT_ERROR_MSG("InvertFilterObject: filter must have valid filterAcceptsRow");
		pSubFilter_ = nullptr;
	}
}
}

