#ifndef QT_CONNECTION_HOLDER_HPP
#define QT_CONNECTION_HOLDER_HPP

#include <QObject>

namespace wgt
{
class QtConnectionHolder
{
public:
	QtConnectionHolder();
	QtConnectionHolder(QMetaObject::Connection& connection);
	~QtConnectionHolder();

	void reset();

	QtConnectionHolder& operator+=(const QMetaObject::Connection& connection);

private:
	std::vector<QMetaObject::Connection> connections_;
};
} // end namespace wgt
#endif // QT_CONNECTION_HOLDER_HPP
