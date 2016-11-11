#ifndef VALUE_CHANGE_NOTIFIER_HPP
#define VALUE_CHANGE_NOTIFIER_HPP

#include "i_value_change_notifier.hpp"
#include "core_variant/variant.hpp"

namespace wgt
{
/**
 *	Wraps a type T, and sends notification events when the value is changed.
 */
template <class T>
class ValueChangeNotifier : public IValueChangeNotifier
{
public:
	typedef T value_type;

	ValueChangeNotifier(const T& data);
	Variant variantValue() const override;
	bool variantValue(const Variant& data) override;

	const T& value() const;
	void value(const T& data);

private:
	ValueChangeNotifier(const ValueChangeNotifier& other);
	ValueChangeNotifier& operator=(const ValueChangeNotifier& other);

	T value_;
};

template <class T>
ValueChangeNotifier<T>::ValueChangeNotifier(const T& data) : value_(data)
{
}

template <class T>
Variant ValueChangeNotifier<T>::variantValue() const
{
	return Variant(value_);
}

template <class T>
bool ValueChangeNotifier<T>::variantValue(const Variant& data)
{
	T typedValue;
	const bool ok = data.tryCast(typedValue);
	if (!ok)
	{
		return false;
	}

	this->value(typedValue);
	return true;
}

template <class T>
const T& ValueChangeNotifier<T>::value() const
{
	return value_;
}

template <class T>
void ValueChangeNotifier<T>::value(const T& data)
{
	if (value_ == data)
	{
		return;
	}
	this->signalPreDataChanged();
	value_ = data;
	this->signalPostDataChanged();
}
} // end namespace wgt
#endif // VALUE_CHANGE_NOTIFIER_HPP
