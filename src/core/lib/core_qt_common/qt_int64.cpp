#include "qt_int64.hpp"

#include <QString>

namespace wgt
{
QtInt64::QtInt64(int64_t value) : value_(value)
{
}
QtInt64::QtInt64(const QString& value)
{
	fromString(value);
}
QtInt64::~QtInt64(){};

int64_t QtInt64::value() const
{
	return value_;
};

QString QtInt64::stringValue() const
{
	return QString::number(value_);
};
void QtInt64::fromString(const QString& value)
{
	static_assert(sizeof(value_) == sizeof(long long), "type error in cast");
	value_ = value.toLongLong();
};

QtUInt64::QtUInt64(int64_t value) : value_(value)
{
}
QtUInt64::QtUInt64(const QString& value)
{
	fromString(value);
}
QtUInt64::~QtUInt64(){};

uint64_t QtUInt64::value() const
{
	return value_;
};

QString QtUInt64::stringValue() const
{
	return QString::number(value_);
};
void QtUInt64::fromString(const QString& value)
{
	static_assert(sizeof(value_) == sizeof(long long), "type error in cast");
	value_ = value.toLongLong();
};
}
