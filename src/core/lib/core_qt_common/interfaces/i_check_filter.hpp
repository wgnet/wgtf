#ifndef I_CHECK_FILTER_HPP
#define I_CHECK_FILTER_HPP

#include <vector>

/// Interface class for the QtListFilter's custom filter

class QString;

namespace wgt
{
class IItem;

class ICheckFilter
{
public:
	virtual bool checkFilter(const IItem* item, const std::vector<QString>& filters) = 0;
};
} // end namespace wgt
#endif // I_CHECK_FILTER_HPP
