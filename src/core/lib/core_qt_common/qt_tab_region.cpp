#include "qt_tab_region.hpp"
#include "i_qt_framework.hpp"
#include "qml_view.hpp"

#include <QTabWidget>
#include <QVariant>

namespace wgt
{
QtTabRegion::QtTabRegion( IComponentContext & context, QTabWidget & qTabWidget )
	: Depends( context )
	, qTabWidget_( qTabWidget )
	, current_(nullptr)
{
	qTabWidget_.setVisible( false );

	auto layoutTagsProperty = qTabWidget_.property( "layoutTags" );
	if (layoutTagsProperty.isValid())
	{
		auto tags = layoutTagsProperty.toStringList();
		for (auto it = tags.cbegin(); it != tags.cend(); ++it)
		{
			tags_.tags_.push_back( std::string( it->toUtf8() ) );
		}
	}

	m_connection = QObject::connect( &qTabWidget_, &QTabWidget::currentChanged, 
		[&](int index)
	{
		if (current_ != nullptr)
		{
			auto it = std::find_if( tabs_.begin(), tabs_.end(), [this](const Tabs::value_type& x) { return x.first == current_; } );
			assert(it != tabs_.end());
			it->second->focusOutEvent();
		}
		if (index >= 0)
		{
			QWidget* curr = qTabWidget_.widget( index );
			auto it = std::find_if( tabs_.begin(), tabs_.end(), [=](const Tabs::value_type& x) { return x.first == curr; } );
			assert(it != tabs_.end());
			auto view = it->second;
			QmlView* qmlView = dynamic_cast<QmlView*>( view );
			if(qmlView != nullptr)
			{
				qmlView->setNeedsToLoad( true );
			}
			view->focusInEvent();
			current_ = curr;
		}
		else
		{
			current_ = nullptr;
		}
	});
}

QtTabRegion::~QtTabRegion()
{
	QObject::disconnect(m_connection);
}

const LayoutTags & QtTabRegion::tags() const
{
	return tags_;
}

void QtTabRegion::addView( IView & view )
{
	// IView will not control qWidget's life-cycle after this call.
	auto qtFramework = get<IQtFramework>();
	assert( qtFramework != nullptr );
	auto qWidget = qtFramework->toQWidget( view );
	if (qWidget == nullptr)
	{
		return;
	}
	int index = qTabWidget_.indexOf( qWidget );
	if (index != -1)
	{
		// already added into the tabWidget
		return;
	}

	tabs_.emplace_back( Tabs::value_type(qWidget, &view) );
	const char* title = view.title();
	int id = qTabWidget_.addTab( qWidget, view.title() );
	if (title == nullptr|| *title == 0)
	{
		qTabWidget_.setTabText(id, QString( (std::string("noname_") + std::to_string(id)).c_str() ));
	}
	qTabWidget_.setVisible(true);
}

void QtTabRegion::removeView( IView & view )
{
	// IView will not control qWidget's life-cycle after this call.
	auto qtFramework = get<IQtFramework>();
	assert( qtFramework != nullptr );
	auto qWidget = qtFramework->toQWidget( view );
	if (qWidget == nullptr)
	{
		return;
	}
	int index = qTabWidget_.indexOf( qWidget );
	auto it = std::find_if( tabs_.begin(), tabs_.end(), [=](Tabs::value_type x) { return x.first == qWidget; } );
	assert(it != tabs_.end());
	if (current_ == qWidget)
	{
		it->second->focusOutEvent();
		current_ = nullptr;
	}
	tabs_.erase(it);

	qTabWidget_.removeTab( index );
	// call this function to let IView control the qWidget's life-cycle again.
	qtFramework->retainQWidget( view );
}
} // end namespace wgt
