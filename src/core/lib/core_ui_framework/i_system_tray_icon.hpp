#ifndef I_SYSTEM_TRAY_ICON_HPP
#define I_SYSTEM_TRAY_ICON_HPP

#include <functional>
#include "core_common/signal.hpp"


namespace wgt
{
class IMenu;

class ISystemTrayIcon
{
public:

	enum ActivationReason
	{
		Unknown,
		Context,
		DoubleClick,
		Trigger,
		MiddleClick
	};

	enum MessageIcon
	{
		NoIcon,
		Information,
		Warning,
		Critical
	};

	virtual ~ISystemTrayIcon() { }

	virtual void show() = 0;
	virtual void hide() = 0;

	virtual void setIcon(const char* path) = 0;
	virtual void setToolTip(const char *tip) = 0;
	
	virtual void showMessage(
		const char* title, 
		const char* message, 
		MessageIcon icon = MessageIcon::Information, 
		int displayTime = 10000) = 0;

	virtual IMenu* getContextMenu() = 0;

	typedef void ActivatedSignature(ActivationReason);
	typedef std::function<ActivatedSignature> ActivatedCallback;
	virtual Connection connectActivated(ActivatedCallback callback) = 0;

	typedef void MessageClickedSignature(void);
	typedef std::function<MessageClickedSignature> MessageClickedCallback;
	virtual Connection connectMessageClicked(MessageClickedCallback callback) = 0;
};
} // end namespace wgt
#endif
