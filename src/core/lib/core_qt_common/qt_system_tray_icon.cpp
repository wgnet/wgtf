#include "qt_system_tray_icon.hpp"

#include <QIcon>
#include <QString>


namespace wgt
{
static_assert(QSystemTrayIcon::ActivationReason::Unknown == ISystemTrayIcon::ActivationReason::Unknown, "Enumerations don't match");
static_assert(QSystemTrayIcon::ActivationReason::Context == ISystemTrayIcon::ActivationReason::Context, "Enumerations don't match");
static_assert(QSystemTrayIcon::ActivationReason::DoubleClick == ISystemTrayIcon::ActivationReason::DoubleClick, "Enumerations don't match");
static_assert(QSystemTrayIcon::ActivationReason::Trigger == ISystemTrayIcon::ActivationReason::Trigger, "Enumerations don't match");
static_assert(QSystemTrayIcon::ActivationReason::MiddleClick == ISystemTrayIcon::ActivationReason::MiddleClick, "Enumerations don't match");

static_assert(QSystemTrayIcon::MessageIcon::NoIcon == ISystemTrayIcon::MessageIcon::NoIcon, "Enumerations don't match");
static_assert(QSystemTrayIcon::MessageIcon::Information == ISystemTrayIcon::MessageIcon::Information, "Enumerations don't match");
static_assert(QSystemTrayIcon::MessageIcon::Warning == ISystemTrayIcon::MessageIcon::Warning, "Enumerations don't match");
static_assert(QSystemTrayIcon::MessageIcon::Critical == ISystemTrayIcon::MessageIcon::Critical, "Enumerations don't match");

QtSystemTrayIcon::QtSystemTrayIcon(const char *path)
	: qMenu_(), contextMenu_(qMenu_, nullptr, ""), qSystemTrayIcon_(QIcon(path))
{
	qSystemTrayIcon_.setContextMenu(&qMenu_);

	qtConnections_ += QObject::connect(
		&qSystemTrayIcon_,
		&QSystemTrayIcon::activated, 
		[this](QSystemTrayIcon::ActivationReason r) { onActivated(r); });

	qtConnections_ += QObject::connect(
		&qSystemTrayIcon_, &QSystemTrayIcon::messageClicked,
		[this]() { onMessageClicked(); });
}

void QtSystemTrayIcon::show()
{
	qSystemTrayIcon_.show();
}

void QtSystemTrayIcon::hide()
{
	qSystemTrayIcon_.hide();
}

void QtSystemTrayIcon::setIcon(const char *path)
{
	qSystemTrayIcon_.setIcon(QIcon(path));
}

void QtSystemTrayIcon::setToolTip(const char *tip)
{
	qSystemTrayIcon_.setToolTip(tip);
}

void QtSystemTrayIcon::showMessage(
	const char* title,
	const char* message,
	MessageIcon icon,
	int displayTime)
{
	qSystemTrayIcon_.showMessage(title, message, (QSystemTrayIcon::MessageIcon)icon, displayTime);
}

IMenu* QtSystemTrayIcon::getContextMenu()
{
	return &contextMenu_;
}

Connection QtSystemTrayIcon::connectActivated(ActivatedCallback callback)
{
	return activatedSignal_.connect(callback);
}

Connection QtSystemTrayIcon::connectMessageClicked(MessageClickedCallback callback)
{
	return messageClickedSignal_.connect(callback);
}

void QtSystemTrayIcon::onActivated(QSystemTrayIcon::ActivationReason reason)
{
	activatedSignal_((ActivationReason)reason);
}

void QtSystemTrayIcon::onMessageClicked()
{
	messageClickedSignal_();
}

} // end namespace wgt