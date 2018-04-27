#include "test_viewport.hpp"
#include "core_qt_common/qt_view.hpp"
#include "core_environment_system/i_env_system.hpp"
#include <QEvent>

namespace wgt
{
ViewportEventListener::ViewportEventListener(QWidget* widget) :
	m_viewportWidget(widget)
{
	assert(m_viewportWidget);
	m_viewportMenu = m_viewportWidget->findChild<QMenu*>("viewportContextMenu");
	assert(m_viewportMenu);
}

bool ViewportEventListener::eventFilter(QObject* obj, QEvent* event)
{
	switch (event->type())
	{
	case QEvent::MouseButtonPress:
	case QEvent::MouseButtonDblClick:
		onMousePress(static_cast<QMouseEvent*>(event));
		break;
	case QEvent::MouseButtonRelease:
		onMouseRelease(static_cast<QMouseEvent*>(event));
		break;
	}
	return QObject::eventFilter(obj, event);
}

void ViewportEventListener::onMousePress(QMouseEvent* evt)
{
	if (grabMouse())
	{
		m_mouseGrabTimeStamp = evt->timestamp();
	}
}

void ViewportEventListener::onMouseRelease(QMouseEvent* evt)
{
	if (releaseMouse())
	{
		if (evt->timestamp() - m_mouseGrabTimeStamp <= 250 /*msForClick*/)
		{
			m_viewportMenu->exec(m_viewportWidget->mapToGlobal(
				QPoint(evt->x(), evt->y())));
		}
	}
}

bool ViewportEventListener::grabMouse()
{
	if (m_mouseGrabCount++ == 0)
	{
		m_viewportWidget->grabMouse();
		return true;
	}
	return false;
}

bool ViewportEventListener::releaseMouse()
{
	assert(m_mouseGrabCount > 0);
	if (m_mouseGrabCount > 0 && --m_mouseGrabCount == 0)
	{
		m_viewportWidget->releaseMouse();
		return true;
	}
	return false;
}

TestViewport::TestViewport(const std::string& name) 
    : Viewport(name, 
		":/testing_ui_main/test_property_tree_panel_viewport.ui", 
		ObjectHandle(), 
		IUIFramework::ResourceType::File)
{
	get<IEnvManager>()->createNewEnvironment(name, this);
}

void TestViewport::onViewportLoaded(IView& view)
{
	Viewport::onViewportLoaded(view);

	auto& qtView = dynamic_cast<QtView&>(view);
	auto viewport = qtView.widget();
	assert(viewport != nullptr);

	listener_ = std::make_unique<ViewportEventListener>(viewport);
	viewport->setMouseTracking(true);
	viewport->installEventFilter(listener_.get());
}

TestViewport::~TestViewport()
{
	get<IEnvManager>()->removeEnvironment(getId());
}
}