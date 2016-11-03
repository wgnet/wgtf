#include "qt_component_finder.hpp"

namespace wgt
{
const std::set<QString>& QtComponentFinder::getTypes() const
{
	return types_;
}

void QtComponentFinder::addType(QString type)
{
	types_.insert(type);
}

QString QtComponentFinder::getType() const
{
	return "";
}

} // end namespace wgt
