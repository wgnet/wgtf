#include "application_proxy.hpp"
#include <QtCore/QTimer>
#include "core_ui_framework/i_ui_application.hpp"
#include "core_logging/logging.hpp"

#include <QtGui/QDockWidget>
#include <QtGui/QLayout>
#include <QtGui/QMainWindow>
#include "qwinhost.h"
#include "core_ui_framework/i_window.hpp"
#include "interfaces/i_application_adapter.hpp"

#include <maya/MQtUtil.h>

namespace wgt
{
ApplicationProxy::ApplicationProxy( IUIApplication* application, QObject* parent)
	: timer_( new QTimer( this ) )
	, application_( application )
	, started_( false )
	, visible_( false )
{
	QObject::connect( timer_, SIGNAL( timeout() ), this, SLOT( processEvents() ) );
	IApplicationAdapter * adapter = dynamic_cast< IApplicationAdapter * >( application_ );

	if (adapter)
	{
		adapter->addListener( this );
	}
}

ApplicationProxy::~ApplicationProxy()
{
	stop();
}

void ApplicationProxy::applicationStarted()
{

}

void ApplicationProxy::applicationStopped()
{
	stop();
}

void ApplicationProxy::windowShown( IWindowAdapter * window )
{
	auto iter = windows_.find( window );
	
	if (iter != windows_.end())
	{
		iter->second->show();
	}
}

void ApplicationProxy::windowHidden( IWindowAdapter * window )
{
	auto iter = windows_.find( window );

	if (iter != windows_.end())
	{
		iter->second->hide();
	}
}

void ApplicationProxy::windowClosed( IWindowAdapter * window )
{
	auto iter = windows_.find( window );

	if (iter != windows_.end())
	{
		iter->second->hide();
		iter->second->deleteLater();
		windows_.erase( iter );
	}

	if (windows_.empty())
	{
		stop();
	}
}


bool ApplicationProxy::started() const
{
	return started_;
}

bool ApplicationProxy::visible() const
{
	return visible_;
}

void ApplicationProxy::processEvents()
{
	application_->startApplication();
}

void ApplicationProxy::start()
{
	timer_->start( 0 );
	auto mw = qobject_cast< QMainWindow * >( MQtUtil::mainWindow() );

	for (auto & kv : application_->windows())
	{
		auto win = kv.second;
		IWindowAdapter * adapter = dynamic_cast< IWindowAdapter * >( win );
		if (!adapter)
		{
			continue;
		}

		win->hide();
		adapter->makeFramelessWindow();

		auto qWidget = new QWinHost( mw );

		HWND winId = reinterpret_cast< HWND >( adapter->nativeWindowId() );
		qWidget->setWindow( winId );
		qWidget->setWindowTitle( win->title() );
		qWidget->setFeatures( QDockWidget::AllDockWidgetFeatures );
		qWidget->setAllowedAreas( Qt::AllDockWidgetAreas );
		mw->addDockWidget(Qt::RightDockWidgetArea, qWidget );
		win->show();
		windows_.insert( std::make_pair( adapter, qWidget ) );

		adapter->addListener( this );
	}
	started_ = true;
	visible_ = true;
}

void ApplicationProxy::stop()
{
	started_ = false;
	visible_ = false;

	timer_->stop();

	for (auto& kv : windows_)
	{
		kv.second->hide();
		kv.second->deleteLater();
	}

	windows_.clear();
}

void ApplicationProxy::show()
{
	for (auto kv : windows_)
	{
		kv.second->show();
	}
	visible_ = true;
}

void ApplicationProxy::hide()
{
	for (auto kv : windows_)
	{
		kv.second->hide();
	}
	visible_ = false;
}
} // end namespace wgt
