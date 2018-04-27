#pragma once

#include <QObject>
#include <QMetaType>
#include <QtGlobal>

#include <memory>

namespace wgt
{
class QtInt64
{
public:
	typedef int64_t value_type;

	QtInt64() = default;
	QtInt64(const QtInt64&) = default;
	QtInt64(QtInt64&&) = default;
	QtInt64(int64_t value);
	QtInt64(const QString& value);
	~QtInt64();

	QtInt64& operator=(const QtInt64&) = default;
	QtInt64& operator=(QtInt64&&) = default;

	int64_t value() const;

	QString stringValue() const;
	void fromString(const QString& value);

	bool operator<(QtInt64 v) const
	{
		return value_ < v.value_;
	}

	bool operator==(QtInt64 v) const
	{
		return value_ == v.value_;
	}

private:
	int64_t value_ = 0;
};

class QtUInt64
{
public:
	typedef uint64_t value_type;

	QtUInt64() = default;
	QtUInt64(const QtUInt64&) = default;
	QtUInt64(QtUInt64&&) = default;
	QtUInt64(int64_t value);
	QtUInt64(const QString& value);
	~QtUInt64();

	QtUInt64& operator=(const QtUInt64&) = default;
	QtUInt64& operator=(QtUInt64&&) = default;

	uint64_t value() const;

	QString stringValue() const;
	void fromString(const QString& value);

	bool operator<(const QtUInt64& v) const
	{
		return value_ < v.value_;
	}

	bool operator==(const QtUInt64& v) const
	{
		return value_ == v.value_;
	}

private:
	uint64_t value_ = 0;
};
}

Q_DECLARE_SMART_POINTER_METATYPE(std::shared_ptr);
Q_DECLARE_METATYPE(std::shared_ptr<wgt::QtInt64>);
Q_DECLARE_METATYPE(std::shared_ptr<wgt::QtUInt64>);
