#include "qt_connection_holder.hpp"

#include "core_common/assert.hpp"

namespace wgt
{
QtConnectionHolder::QtConnectionHolder()
{
}

QtConnectionHolder::QtConnectionHolder(QMetaObject::Connection& connection)
{
	*this += connection;
}

QtConnectionHolder::QtConnectionHolder(QtConnectionHolder&& qtConnectionHolder)
    : connections_(std::move(qtConnectionHolder.connections_))
{
}

QtConnectionHolder::~QtConnectionHolder()
{
	reset();
}

void QtConnectionHolder::reset()
{
	for (auto connection : connections_)
	{
		QObject::disconnect(connection);
	}
	connections_.resize(0);
}

QtConnectionHolder& QtConnectionHolder::operator+=(const QMetaObject::Connection& connection)
{
	TF_ASSERT(connection);
	connections_.push_back(connection);
	return *this;
}
} // end namespace wgt
