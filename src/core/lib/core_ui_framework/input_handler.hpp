#pragma once

namespace wgt
{
enum class MouseButton
{
	ButtonUndefined,
	ButtonLeft,
	ButtonRight,
	ButtonMiddle
};

class InputHandler
{
public:
	virtual ~InputHandler() = default;
	virtual bool onMouseButtonClick(MouseButton button, int x, int y) { return false; }
	virtual bool onMouseButtonDown(MouseButton button, int x, int y) { return false; }
	virtual bool onMouseButtonUp(MouseButton button, int x, int y) { return false; }
	virtual bool onMouseMove(int x, int y) { return false; }
	virtual void onMouseFocusChanged(bool leavingFocus) {}
	virtual bool onMouseWheel(int delta) { return false; }
	virtual bool onNativeEvent(uintptr_t /*message*/, long* /*result*/) const { return false; }
	virtual bool onKeyDown(unsigned int vkCode) { return false; }
	virtual bool onKeyUp(unsigned int vkCode) { return false; }
	virtual void onFocusChanged(bool focus) {}
	virtual bool hasNativeKeyboardFocus() const { return false; }
	virtual bool wantExclusiveFocus() const { return false; }
};

}
