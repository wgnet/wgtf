#ifndef I_VALUE_CHANGE_NOTIFIER_HPP
#define I_VALUE_CHANGE_NOTIFIER_HPP

#include "core_common/signal.hpp"

namespace wgt
{
class Variant;

/**
 *	Interface for accessing ValueChangeNotifier without a templated type.
 */
class IValueChangeNotifier
{
	typedef Signal<void(void)> SignalVoid;

public:
	virtual ~IValueChangeNotifier()
	{
		signalDestructing();
	}
	virtual Variant variantValue() const = 0;
	virtual bool variantValue(const Variant& data) = 0;

	SignalVoid signalPreDataChanged;
	SignalVoid signalPostDataChanged;
	SignalVoid signalDestructing;
};
} // end namespace wgt
#endif // I_VALUE_CHANGE_NOTIFIER_HPP
