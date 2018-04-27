#ifndef I_VIEW_HPP
#define I_VIEW_HPP

#include "core_common/signal.hpp"

#include <cstdint>
#include <vector>
#include "core_ui_framework/i_cursor.hpp"

namespace wgt
{
struct LayoutHint;
class IMenu;

class IView
{
public:
	using BooleanCallbackSignature = void(bool);
	using BooleanCallback = std::function<BooleanCallbackSignature>;
	using CharStringCallbackSignature = void(const char*);
	using CharStringCallback = std::function<CharStringCallbackSignature>;

	virtual ~IView() = default;
	virtual const char* id() const = 0;
	virtual const char* title() const = 0;
	virtual void title(const char* title) = 0;
	virtual const char* windowId() const = 0;
	virtual const LayoutHint& hint() const = 0;
	virtual void update() = 0;
	virtual void reload() = 0;

	virtual bool focused() = 0;
	virtual void setFocus(bool focus) = 0;
	virtual void focusChanged() = 0;
	virtual Connection connectFocusChanged(const BooleanCallback& callback) = 0;
	virtual Connection connectTitleChanged(const CharStringCallback& callback) = 0;

	virtual CursorId getCursor() const = 0;
	virtual void setCursor(CursorId cursorId) = 0;
	virtual void unsetCursor() = 0;

	virtual uintptr_t getNativeWindowHandle() { return 0; }
	virtual std::vector<IMenu*> menus() const { return {}; }
};
} // end namespace wgt
#endif // I_VIEW_HPP
