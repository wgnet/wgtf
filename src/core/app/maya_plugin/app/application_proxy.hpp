#ifndef APPLICATION_PROXY_HPP
#define APPLICATION_PROXY_HPP

#include <QtCore/QObject>

#include "core_ui_framework/i_ui_application.hpp"
#include "interfaces/i_application_listener.hpp"
#include "interfaces/i_window_listener.hpp"
#include "interfaces/i_window_adapter.hpp"

class QTimer;
class QWinHost;

namespace wgt
{
class ApplicationProxy : public QObject, public IApplicationListener, public IWindowListener
{
	Q_OBJECT
public:
	ApplicationProxy(IUIApplication* application, QObject* parent = 0);
	virtual ~ApplicationProxy();

	void applicationStarted() override;
	void applicationStopped() override;
	void windowShown(IWindowAdapter* window) override;
	void windowHidden(IWindowAdapter* window) override;
	void windowClosed(IWindowAdapter* window) override;

	bool started() const;
	bool visible() const;

public slots:
	void processEvents();
	void start();
	void stop();
	void show();
	void hide();

private:
	IUIApplication* application_;
	std::map<IWindowAdapter*, QWinHost*> windows_;
	bool started_;
	bool visible_;
};
} // end namespace wgt
#endif // APPLICATION_PROXY_HPP
