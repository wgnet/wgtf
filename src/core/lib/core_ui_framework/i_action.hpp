#ifndef I_ACTION_HPP
#define I_ACTION_HPP
#include "core_common/signal.hpp"
#include <vector>

namespace wgt
{
class Variant;

class IAction
{
	typedef Signal<void(const char*)> SignalTextChanged;
	typedef Signal<void(const char*)> SignalShortcutChanged;
	typedef Signal<void(int)> SignalIconChanged;
	typedef Signal<void(bool)> SignalVisibilityChanged;

public:
	virtual ~IAction() = default;

	virtual void text(const char* value) = 0;
	virtual const char* id() const = 0;
	virtual const char* text() const = 0;
	virtual const char* icon() const = 0;
	virtual const char* windowId() const = 0;
	virtual const std::vector<std::string>& paths() const = 0;
	virtual const char* shortcut() const = 0;
	virtual const char* group() const = 0;
	virtual int order() const = 0;
	virtual bool isSeparator() const = 0;
	virtual bool enabled() const = 0;
	virtual bool checked() const = 0;
	virtual bool isCheckable() const = 0;
	virtual bool visible() const = 0;
	virtual void visible(bool visible) = 0;
	virtual void execute() = 0;

	virtual void setData(const Variant&) = 0;
	virtual Variant& getData() = 0;
	virtual const Variant& getData() const = 0;

	SignalShortcutChanged signalShortcutChanged;
	SignalTextChanged signalTextChanged;
	SignalVisibilityChanged signalVisibilityChanged;
	SignalIconChanged signalIconChanged;

private:
	friend class ActionManager;
	virtual void setShortcut(const char* shortcut) = 0;
};
} // end namespace wgt
#endif // I_ACTION_HPP
