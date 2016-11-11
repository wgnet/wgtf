#ifndef I_VIEW_HPP
#define I_VIEW_HPP

namespace wgt
{
struct LayoutHint;
class IView;

class IViewEventListener
{
public:
	virtual void onFocusIn(IView* view) = 0;
	virtual void onFocusOut(IView* view) = 0;
	virtual void onLoaded(IView* view) = 0;
};

class IView
{
public:
	virtual ~IView()
	{
	}
	virtual const char* id() const = 0;
	virtual const char* title() const = 0;
	virtual const char* windowId() const = 0;
	virtual const LayoutHint& hint() const = 0;
	virtual void update() = 0;

	virtual void focusInEvent() = 0;
	virtual void focusOutEvent() = 0;

	virtual void registerListener(IViewEventListener* listener) = 0;
	virtual void deregisterListener(IViewEventListener* listener) = 0;
};
} // end namespace wgt
#endif // I_VIEW_HPP
