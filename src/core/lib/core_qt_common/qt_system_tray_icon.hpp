#ifndef QT_SYSTEM_TRAY_ICON_HPP
#define QT_SYSTEM_TRAY_ICON_HPP

#include "core_ui_framework/i_system_tray_icon.hpp"
#include "core_qt_common/qt_connection_holder.hpp"
#include "core_qt_common/qt_context_menu.hpp"

#include <memory>
#include <QSystemTrayIcon>
#include <QMenu>


namespace wgt
{
class IMenu;

class QtSystemTrayIcon : public ISystemTrayIcon
{
public:
	QtSystemTrayIcon(const char *iconPath);

	virtual void show() override;
	virtual void hide() override;

	virtual void setIcon(const char *path) override;
	virtual void setToolTip(const char *tip) override;
	
	virtual void showMessage(
		const char* title, 
		const char* message, 
		MessageIcon icon = MessageIcon::Information, 
		int displayTime = 10000) override;

	virtual IMenu* getContextMenu() override;

	virtual Connection connectActivated(ActivatedCallback callback) override;
	virtual Connection connectMessageClicked(MessageClickedCallback callback) override;

private:
	void onActivated(QSystemTrayIcon::ActivationReason reason);
	void onMessageClicked();

	QSystemTrayIcon qSystemTrayIcon_;
	QtConnectionHolder qtConnections_;
	Signal<void(ActivationReason)> activatedSignal_;
	Signal<void()> messageClickedSignal_;
	QMenu qMenu_;
	QtContextMenu contextMenu_;
};

} // end namespace wgt

#endif // QT_SYSTEM_TRAY_ICON_HPP
