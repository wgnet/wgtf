#include "qt_dock_region.hpp"
#include "qt_window.hpp"
#include "i_qt_framework.hpp"

#include "core_ui_framework/i_view.hpp"
#include "core_ui_framework/i_ui_application.hpp"

#include <QDockWidget>
#include <QLayout>
#include <QMainWindow>
#include <QVariant>
#include <QEvent>
#include <QCoreApplication>

namespace wgt
{
QtDockRegion::QtDockRegion( IQtFramework & qtFramework, QtWindow & qtWindow, QDockWidget & qDockWidget )
	: qtFramework_( qtFramework )
	, qtWindow_( qtWindow )
	, qDockWidget_( qDockWidget )
	, hidden_( false )
{
	auto qMainWindow = qtWindow_.window();
	assert( qMainWindow != nullptr );

	// Walk our parent hierarchy and make sure we are tabified with the topmost dock widget.
	// Dock widgets as children of anything but the main window are not supported.
	// We support this in the Qt designer so that we can override the properties of a tab or collection of tabs within a dock region
	QWidget * qWidget = &qDockWidget_;
	while (qWidget != nullptr)
	{
		qWidget = qWidget->parentWidget();
		if (qWidget == nullptr)
		{
			break;
		}
		auto qDockWidget = qobject_cast< QDockWidget * >( qWidget );
		if (qDockWidget != nullptr)
		{
			qMainWindow->tabifyDockWidget( qDockWidget, &qDockWidget_ );
		}
	}
	qDockWidget_.setVisible( false );

	auto layoutTagsProperty = qDockWidget_.property( "layoutTags" );
	if (layoutTagsProperty.isValid())
	{
		auto tags = layoutTagsProperty.toStringList();
		for (auto it = tags.cbegin(); it != tags.cend(); ++it)
		{
			tags_.tags_.push_back( std::string( it->toUtf8() ) );
		}
	}

	auto hiddenProperty = qDockWidget_.property( "hidden" );
	if (hiddenProperty.isValid())
	{
		hidden_ = hiddenProperty.toBool();
	}

	QObject::connect( &qtWindow_, &QtWindow::windowReady, 
		[&](){
			if (needToRestorePreference_.empty())
			{
				return;
			}
			auto qMainWindow = qtWindow_.window();
			for( auto qtDockWidget : needToRestorePreference_)
			{
				bool isOk = qMainWindow->restoreDockWidget( qtDockWidget );
				if (!isOk)
				{
					setDefaultPreferenceForDockWidget( qtDockWidget );
				}
			}
			needToRestorePreference_.clear();
		});
}

const LayoutTags & QtDockRegion::tags() const
{
	return tags_;
}

// This ugly class handles all the cases for switching between docked and floating DockWigets
// Unfortunately Qt does not provide generous solution for it
class NGTDockWidget : public QDockWidget
{
public:
	NGTDockWidget( IView* view ) : QDockWidget( view->title() ), view_(view) {}

	void visibilityChanged(bool visible)
	{
		visible_ = visible;
	}

protected:
	virtual bool event(QEvent * e) override
	{
		switch (e->type())
		{
		case QEvent::WindowActivate:
			active_ = true;
			break;

		case QEvent::WindowDeactivate:
			active_ = false;
			break;
        case QEvent::FocusIn:
            if(visible_)
            {
                view_->focusInEvent();
            }
            break;

		case QEvent::ActivationChange:
            if(isFloating())
            {
                if (active_)
                    view_->focusInEvent();
                else
                    view_->focusOutEvent();
            }
			break;
		}
		return QDockWidget::event(e);
	}

private:
	IView* view_;
	bool active_;
	bool visible_;
};

void QtDockRegion::restoreDockWidgets()
{
    auto qMainWindow = qtWindow_.window();
    assert( qMainWindow != nullptr );
    for( auto& it : dockWidgetMap_)
    {
        auto qDockWidget = it.second.first.get();
        if (qtWindow_.isReady())
        {
            bool isOk = qMainWindow->restoreDockWidget( qDockWidget );
        }
        else
        {
            needToRestorePreference_.push_back( qDockWidget );
        }
    }
}

void QtDockRegion::setDefaultPreferenceForDockWidget( QDockWidget * qDockWidget )
{
	auto qMainWindow = qtWindow_.window();
	assert( qMainWindow != nullptr );
	qMainWindow->tabifyDockWidget( &qDockWidget_, qDockWidget );
	qDockWidget->setFloating( qDockWidget_.isFloating() );
	qDockWidget->setFeatures( qDockWidget_.features() );
	qDockWidget->setAllowedAreas( qDockWidget_.allowedAreas() );
	qDockWidget->setVisible( !hidden_ ); 
}

void QtDockRegion::addView( IView & view )
{
	auto qMainWindow = qtWindow_.window();
	assert( qMainWindow != nullptr );

	auto findIt = dockWidgetMap_.find( &view );
	if (findIt != dockWidgetMap_.end())
	{
		// already added into the dockWidget
		return;
	}
	// IView will not control qWidget's life-cycle after this call.
	auto qWidget = qtFramework_.toQWidget( view );
	if (qWidget == nullptr)
	{
		return;
	}

	qMainWindow->centralWidget()->layout()->addWidget( qWidget );
	qWidget->setSizePolicy( qDockWidget_.sizePolicy() );
	qWidget->setMinimumSize( qDockWidget_.minimumSize() );
	qWidget->setMaximumSize( qDockWidget_.maximumSize() );
	qWidget->setSizeIncrement( qDockWidget_.sizeIncrement() );
	qWidget->setBaseSize( qDockWidget_.baseSize() );
	qWidget->resize( qWidget->baseSize() );

	auto qDockWidget = new NGTDockWidget( &view );
	qDockWidget->setObjectName( view.id() );
	qDockWidget->setWidget( qWidget );
	
	if (qtWindow_.isReady())
	{
		bool isOk = qMainWindow->restoreDockWidget( qDockWidget );
		if (!isOk)
		{
			setDefaultPreferenceForDockWidget( qDockWidget );
		}
	}
	else
	{
		needToRestorePreference_.push_back( qDockWidget );
	}

	std::string actionId( "View." );
	actionId += view.title();
	auto action = qtFramework_.createAction( actionId.c_str(), [=](IAction *)
	{
		qDockWidget->show();
		qDockWidget->raise();
	});
	auto application = qtWindow_.getApplication();
	assert( application != nullptr );
	application->addAction( *action );

	dockWidgetMap_[ &view ] = std::make_pair( std::unique_ptr< QDockWidget >( qDockWidget ), std::move( action ) );

	QObject::connect( qDockWidget, &QDockWidget::visibilityChanged,
		[=](bool visible) { 
        qDockWidget->visibilityChanged( visible ); 
        if(visible)
        {
            if(qtWindow_.isLoadingPreferences())
            {
                return;
            }
            QCoreApplication::postEvent( qDockWidget, new QEvent(QEvent::FocusIn));
        }
        
    } );
}

void QtDockRegion::removeView( IView & view )
{
	auto qMainWindow = qtWindow_.window();
	if (qMainWindow == nullptr)
	{
		return;
	}

	auto findIt = dockWidgetMap_.find( &view );
	if (findIt == dockWidgetMap_.end())
	{
		return;
	}
	
	//TODO: save dockWidget state
	auto dockWidget = std::move( findIt->second.first );
	auto action = std::move( findIt->second.second );
	dockWidgetMap_.erase( findIt );

	auto application = qtWindow_.getApplication();
	assert( application != nullptr );
	application->removeAction( *action );
	action = nullptr;

	assert( dockWidget != nullptr );
	dockWidget->setWidget( nullptr );
	qMainWindow->removeDockWidget( dockWidget.get() );
	// call this function to let IView control the qWidget's life-cycle again.
	qtFramework_.retainQWidget( view );
	dockWidget = nullptr;
}
} // end namespace wgt
