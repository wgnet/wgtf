#pragma once

#include "core_dependency_system/depends.hpp"
#include "core_viewport/viewport.hpp"
#include <string.h>
#include <memory.h>
#include <QMenu>
#include <QWidget>
#include <QMouseEvent>

namespace wgt
{
class IEnvManager;

// TODO: Make this generic. This is similar to how 
// titan editor obtains input information for their viewport.
class ViewportEventListener : public QObject
{
	Q_OBJECT
public:
	ViewportEventListener(QWidget* widget);

private:
	bool eventFilter(QObject* obj, QEvent* event);
	bool grabMouse();
	bool releaseMouse();
	void onMousePress(QMouseEvent* evt);
	void onMouseRelease(QMouseEvent* evt);

	QWidget* m_viewportWidget = nullptr;
	QMenu* m_viewportMenu = nullptr;
	int m_mouseGrabCount = false;
	unsigned long m_mouseGrabTimeStamp = 0;
};

class TestViewport : Depends<IEnvManager>, public Viewport
{
public:
	TestViewport(const std::string& name);
	~TestViewport();
	virtual void onViewportLoaded(IView& view) override;

private:
	std::unique_ptr<ViewportEventListener> listener_;
};

} // end namespace wgt
