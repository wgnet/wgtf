#ifndef I_MENU_HPP
#define I_MENU_HPP

namespace wgt
{
class IAction;

class IMenu
{
public:
	virtual ~IMenu()
	{
	}

	virtual const char* path() const = 0;
	virtual const char* windowId() const = 0;

	virtual void update() = 0;

	virtual void addPath(const char* path) = 0;
	virtual void addAction(IAction& action, const char* path) = 0;
	virtual void removeAction(IAction& action) = 0;
};
} // end namespace wgt
#endif // I_MENU_HPP
