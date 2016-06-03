#include "qt_window_adapter.hpp"
#include <QtWidgets/QWidget>
#include <QtWidgets/QMainWindow>
#include <assert.h>

namespace wgt
{
QtWindowAdapter::QtWindowAdapter( IQtFramework & qtFramework, QIODevice & source )
	: QtWindow( qtFramework, source )
{
}

QtWindowAdapter::~QtWindowAdapter()
{
}

void QtWindowAdapter::close()
{
	QtWindow::close();

	for (auto listener : listeners_)
	{
		listener->windowClosed( this );
	}
}

void QtWindowAdapter::show( bool wait /* = false */ )
{
	QtWindow::show( wait );

	for (auto listener : listeners_)
	{
		listener->windowShown( this );
	}
}

void QtWindowAdapter::hide()
{
	QtWindow::hide();

	for (auto listener : listeners_)
	{
		listener->windowHidden( this );
	}
}


void * QtWindowAdapter::nativeWindowId() const
{
	return reinterpret_cast< void * >( window()->winId() );
}

void QtWindowAdapter::makeFramelessWindow()
{
	window()->setWindowFlags( Qt::Widget | Qt::FramelessWindowHint );
}


void QtWindowAdapter::addListener( IWindowListener * listener )
{
	listeners_.push_back( listener );
}

void QtWindowAdapter::removeListener( IWindowListener * listener )
{
	auto && listenerIt = std::find( listeners_.begin(), listeners_.end(), listener );
	assert( listenerIt != listeners_.end() );
	listeners_.erase( listenerIt );
}
} // end namespace wgt
