#include "qt_application_adapter.hpp"
#include <QtGui/QGuiApplication>
#include <QApplication>
#include <assert.h>
#include "core_logging/logging.hpp"

namespace wgt
{
QtApplicationAdapter::QtApplicationAdapter()
	: QtApplication( __argc, __argv )
{
	QObject::connect( QGuiApplication::instance(),
		SIGNAL( applicationStateChanged( Qt::ApplicationState ) ),
		this,
		SLOT( applicationStateChanged( Qt::ApplicationState ) ) );

	QObject::connect( QCoreApplication::instance(), SIGNAL( aboutToQuit() ), this, SLOT( applicationStopped() ) );
}

QtApplicationAdapter::~QtApplicationAdapter()
{
}

void QtApplicationAdapter::applicationStateChanged( Qt::ApplicationState state )
{
	if (state == Qt::ApplicationActive)
	{
		for(auto & listener : listeners_)
		{
			listener->applicationStarted();
		}
	}
}

void QtApplicationAdapter::applicationStopped()
{
	for (auto listener : listeners_)
	{
		listener->applicationStopped();
	}
}

int QtApplicationAdapter::startApplication()
{
	assert( application_ != nullptr );
	application_->processEvents();
	update();
	return 0;
}

void QtApplicationAdapter::addListener( IApplicationListener * listener )
{
	listeners_.push_back( listener );
}

void QtApplicationAdapter::removeListener( IApplicationListener * listener )
{
	auto && listenerIt = std::find( listeners_.begin(), listeners_.end(), listener );
	assert( listenerIt != listeners_.end() );
	listeners_.erase( listenerIt );
}
} // end namespace wgt
