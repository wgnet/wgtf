#include "qt_spinbox_validator.hpp"
#include "qt_int64.hpp"

#include <QVariant>

#include <functional>

namespace wgt
{
WGSpinBoxValidator::WGSpinBoxValidator(QObject* parent) : QValidator(parent), value_("0"), step_(1), initialized_(false)
{
	validator_.setTop(99);
	validator_.setBottom(0);
	validator_.setNotation(QDoubleValidator::StandardNotation);

	QLocale locale;
	locale.setNumberOptions(QLocale::OmitGroupSeparator);
	setLocale(locale);

	connect(this, SIGNAL(valueChanged()), this, SIGNAL(textChanged()));
	connect(this, SIGNAL(minimumValueChanged()), this, SIGNAL(textChanged()));
	connect(this, SIGNAL(maximumValueChanged()), this, SIGNAL(textChanged()));
	connect(this, SIGNAL(decimalsChanged()), this, SIGNAL(textChanged()));
	connect(this, SIGNAL(prefixChanged()), this, SIGNAL(textChanged()));
	connect(this, SIGNAL(suffixChanged()), this, SIGNAL(textChanged()));
}

WGSpinBoxValidator::~WGSpinBoxValidator()
{
}

QString WGSpinBoxValidator::text() const
{
	return textFromValue(value_);
}

QVariant WGSpinBoxValidator::value() const
{
	return value_;
}

void WGSpinBoxValidator::setValue(QVariant value)
{
	if (initialized_) {
		value = boundValue(value);
	}

	if (!compareValue(value))
	{
		if (value_.type() == QVariant::UserType && value_.userType() == qMetaTypeId<std::shared_ptr<QtInt64>>())
		{
			value_ = QVariant::fromValue(std::shared_ptr<QtInt64>(new QtInt64(value.toLongLong())));
		}
		else if (value_.type() == QVariant::UserType && value_.userType() == qMetaTypeId<std::shared_ptr<QtUInt64>>())
		{
			value_ = QVariant::fromValue(std::shared_ptr<QtUInt64>(new QtUInt64(value.toULongLong())));
		}
		else
		{
			value_ = value;
		}

		if (initialized_)
		{
			emit valueChanged();
		}
	}
}

qreal WGSpinBoxValidator::minimumValue() const
{
	return validator_.bottom();
}

void WGSpinBoxValidator::setMinimumValue(qreal min)
{
	if (min != validator_.bottom())
	{
		validator_.setBottom(min);
		emit minimumValueChanged();
		if (initialized_)
			setValue(value_);
	}
}

qreal WGSpinBoxValidator::maximumValue() const
{
	return validator_.top();
}

void WGSpinBoxValidator::setMaximumValue(qreal max)
{
	if (max != validator_.top())
	{
		validator_.setTop(max);
		emit maximumValueChanged();
		if (initialized_)
			setValue(value_);
	}
}

int WGSpinBoxValidator::decimals() const
{
	return decimals_;
}

void WGSpinBoxValidator::setDecimals(int decimals)
{
	if(decimals != decimals_)
	{
		decimals_ = decimals;
		emit decimalsChanged();
		if (initialized_)
			setValue(value_);
	}
}

qreal WGSpinBoxValidator::stepSize() const
{
	return step_;
}

void WGSpinBoxValidator::setStepSize(qreal step)
{
	if (step_ != step)
	{
		step_ = step;
		emit stepSizeChanged();
	}
}

QString WGSpinBoxValidator::prefix() const
{
	return prefix_;
}

void WGSpinBoxValidator::setPrefix(const QString& prefix)
{
	if (prefix_ != prefix)
	{
		prefix_ = prefix;
		emit prefixChanged();
	}
}

QString WGSpinBoxValidator::suffix() const
{
	return suffix_;
}

void WGSpinBoxValidator::setSuffix(const QString& suffix)
{
	if (suffix_ != suffix)
	{
		suffix_ = suffix;
		emit suffixChanged();
	}
}

void WGSpinBoxValidator::fixup(QString& input) const
{
	input = QString(value_.toString()).remove(locale().groupSeparator());
}

QValidator::State WGSpinBoxValidator::validate(QString& input, int& pos) const
{
	if (!prefix_.isEmpty() && !input.startsWith(prefix_))
	{
		input.prepend(prefix_);
		pos += prefix_.length();
	}

	if (!suffix_.isEmpty() && !input.endsWith(suffix_))
	{
		input.append(suffix_);
	}

	QString value = input.mid(prefix_.length(), input.length() - prefix_.length() - suffix_.length());
	
	int countSeparatorsBeforePos = 0;
	bool containsSeparator = false;
	for (int i = 0; i < value.size(); ++i)
	{
		bool isSeparator = (value[i] == locale().groupSeparator());
		countSeparatorsBeforePos += (i < pos) && isSeparator;
		containsSeparator |= isSeparator;
	}
	if (containsSeparator)
	{
		value = value.replace(locale().groupSeparator(), "");
		pos -= countSeparatorsBeforePos;
	}
	if (decimals() == 0 && value.contains(locale().decimalPoint()))
	{
		value = textFromValue(value);
		pos = prefix_.size() + value.size();
	}

	int valuePos = pos - prefix_.length();
	QValidator::State state = validator_.validate(value, valuePos);
	input = prefix_ + value + suffix_;
	pos = prefix_.length() + valuePos;

	if (state == QValidator::Acceptable || state == QValidator::Intermediate)
	{
		bool ok = false;
		QVariant val = variantFromText(value, ok);
		if (ok)
		{
			if (state == QValidator::Acceptable ||
			    (state == QValidator::Intermediate && val >= 0 && val <= validator_.top()) ||
			    (state == QValidator::Intermediate && val < 0 && val >= validator_.bottom()))
			{
				const_cast<WGSpinBoxValidator*>(this)->setValue(val);
				if (input != textFromValue(val))
					state = QValidator::Intermediate;
			}
			else if (val < validator_.bottom() || val > validator_.top())
			{
				return QValidator::Invalid;
			}
		}
	}
	return state;
}

void WGSpinBoxValidator::componentComplete()
{
	initialized_ = true;
	setValue(value_);
}

void WGSpinBoxValidator::increment()
{
	if (value_.type() == QVariant::UserType && value_.userType() == qMetaTypeId<std::shared_ptr<QtInt64>>())
	{
		int64_t value = value_.toLongLong();
		setValue(std::numeric_limits<int64_t>::max() - static_cast<int64_t>(step_) < value ?
		         std::numeric_limits<int64_t>::max() :
		         value + static_cast<int64_t>(step_));
	}
	else if (value_.type() == QVariant::UserType && value_.userType() == qMetaTypeId<std::shared_ptr<QtUInt64>>())
	{
		uint64_t value = value_.toULongLong();
		setValue(std::numeric_limits<uint64_t>::max() - static_cast<uint64_t>(step_) < value ?
		         std::numeric_limits<uint64_t>::max() :
		         value + static_cast<uint64_t>(step_));
	}
	else
	{
		setValue(value_.toDouble() + step_);
	}
}

void WGSpinBoxValidator::decrement()
{
	if (value_.type() == QVariant::UserType && value_.userType() == qMetaTypeId<std::shared_ptr<QtInt64>>())
	{
		int64_t value = value_.toLongLong();
		setValue(std::numeric_limits<int64_t>::min() + static_cast<int64_t>(step_) > value ?
		         std::numeric_limits<int64_t>::min() :
		         value - static_cast<int64_t>(step_));
	}
	else if (value_.type() == QVariant::UserType && value_.userType() == qMetaTypeId<std::shared_ptr<QtUInt64>>())
	{
		uint64_t value = value_.toULongLong();
		setValue(std::numeric_limits<uint64_t>::min() + static_cast<uint64_t>(step_) > value ?
		         std::numeric_limits<uint64_t>::min() :
		         value - static_cast<uint64_t>(step_));
	}
	else
	{
		setValue(value_.toDouble() - step_);
	}
}

QVariant WGSpinBoxValidator::variantFromText(const QString& input, bool& ok) const
{
	if (value_.type() == QVariant::UserType && value_.userType() == qMetaTypeId<std::shared_ptr<QtInt64>>())
	{
		return QVariant::fromValue(std::shared_ptr<QtInt64>(new QtInt64(locale().toLongLong(input, &ok))));
	}
	else if (value_.type() == QVariant::UserType && value_.userType() == qMetaTypeId<std::shared_ptr<QtUInt64>>())
	{
		return QVariant::fromValue(std::shared_ptr<QtUInt64>(new QtUInt64(locale().toULongLong(input, &ok))));
	}
	else
	{
		return QVariant::fromValue(locale().toDouble(input, &ok));
	}
}

QString WGSpinBoxValidator::textFromValue(const QVariant& value) const
{
	if (value_.type() == QVariant::UserType && value_.userType() == qMetaTypeId<std::shared_ptr<QtInt64>>())
	{
		return prefix_ + locale().toString(value.toLongLong()) + suffix_;
	}
	else if (value_.type() == QVariant::UserType && value_.userType() == qMetaTypeId<std::shared_ptr<QtUInt64>>())
	{
		return prefix_ + locale().toString(value.toULongLong()) + suffix_;
	}
	else
	{
		return prefix_ + locale().toString(value.toDouble(), 'f', decimals()) + suffix_;
	}
}

QVariant WGSpinBoxValidator::boundValue(const QVariant& value) const
{
	if (!initialized_)
	{
		return value;
	}

	if (value_.type() == QVariant::UserType && value_.userType() == qMetaTypeId<std::shared_ptr<QtInt64>>())
	{
		int64_t minVal = std::less<double>()(std::numeric_limits<int64_t>::min(), minimumValue()) ?
		static_cast<int64_t>(minimumValue()) :
		std::numeric_limits<int64_t>::min();
		int64_t maxVal = std::greater<double>()(std::numeric_limits<int64_t>::max(), maximumValue()) ?
		static_cast<int64_t>(maximumValue()) :
		std::numeric_limits<int64_t>::max();
		return qBound(minVal, value.toLongLong(), maxVal);
	}
	else if (value_.type() == QVariant::UserType && value_.userType() == qMetaTypeId<std::shared_ptr<QtUInt64>>())
	{
		uint64_t minVal = std::less<double>()(std::numeric_limits<uint64_t>::min(), minimumValue()) ?
		static_cast<uint64_t>(minimumValue()) :
		std::numeric_limits<uint64_t>::min();
		uint64_t maxVal = std::greater<double>()(std::numeric_limits<uint64_t>::max(), maximumValue()) ?
		static_cast<uint64_t>(maximumValue()) :
		std::numeric_limits<uint64_t>::max();
		return qBound(minVal, value.toULongLong(), maxVal);
	}
	else
	{
		double bounded = qBound(minimumValue(), value.toDouble(), maximumValue());
		return QString::number(bounded, 'f', decimals()).toDouble();
	}
}

bool WGSpinBoxValidator::compareValue(const QVariant& value) const
{
	if (!initialized_)
	{
		return false;
	}

	if (value_.type() == QVariant::UserType && value_.userType() == qMetaTypeId<std::shared_ptr<QtInt64>>())
	{
		return value.toLongLong() == value_.toLongLong();
	}
	else if (value_.type() == QVariant::UserType && value_.userType() == qMetaTypeId<std::shared_ptr<QtUInt64>>())
	{
		return value.toULongLong() == value_.toULongLong();
	}
	else
	{
		return value.toDouble() == value_.toDouble();
	}
}

}
