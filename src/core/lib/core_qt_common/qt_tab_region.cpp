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

	auto currentChangedFn = [this](int index)
	{
		if (current_ != nullptr)
		{
			auto it = std::find_if(tabs_.begin(), tabs_.end(), [this](const Tabs::value_type& x) { return x.first == current_; });
			assert(it != tabs_.end());
			it->second->focusOutEvent();
		}
		if (index >= 0)
		{
			QWidget* curr = qTabWidget_.widget(index);
			auto it = std::find_if(tabs_.begin(), tabs_.end(), [curr](const Tabs::value_type& x) { return x.first == curr; });
			assert(it != tabs_.end());
			auto view = it->second;
			QmlView* qmlView = dynamic_cast<QmlView*>(view);
			if (qmlView != nullptr)
			{
				qmlView->setNeedsToLoad(true);
			}
			view->focusInEvent();
			current_ = curr;
		}
		else
		{
			current_ = nullptr;
		}
	};

	m_connection = QObject::connect(&qTabWidget_, &QTabWidget::currentChanged, currentChangedFn);
}

QtTabRegion::~QtTabRegion()
{
	for (auto& tab : tabs_)
	{
		tab.second->deregisterListener(this);
	}

	QObject::disconnect(m_connection);
}

const LayoutTags & QtTabRegion::tags() const
{
	return tags_;
}

QtTabRegion::Tabs::iterator QtTabRegion::findTabFromView(IView& view)
{
	auto qtFramework = get<IQtFramework>();
	assert(qtFramework != nullptr);
	auto qWidget = qtFramework->toQWidget(view);
	if (qWidget == nullptr)
	{
		return tabs_.end();
	}

	auto findFn = [qWidget](const Tabs::value_type& x) { return x.first == qWidget; };
	return std::find_if(tabs_.begin(), tabs_.end(), findFn);
}

void QtTabRegion::setTabTitle(int index, const char* text)
{
	const QString title((text == nullptr || *text == 0) ? (std::string("noname_") + std::to_string(index)).c_str() : text);
	qTabWidget_.setTabText(index, title);
}

void QtTabRegion::onFocusIn(IView*)
{
}

void QtTabRegion::onFocusOut(IView*)
{
}

void QtTabRegion::onLoaded(IView* view)
{
	auto it = findTabFromView(*view);
	if (it != tabs_.end())
	{
		const int index = qTabWidget_.indexOf(it->first);
		setTabTitle(index, it->second->title());
	}
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

	const int id = qTabWidget_.addTab(qWidget, "");
	setTabTitle(id, view.title());

	view.registerListener(this);
	qTabWidget_.setVisible(true);
}

void QtTabRegion::removeView( IView & view )
{
	// IView will not control qWidget's life-cycle after this call.
	auto qtFramework = get<IQtFramework>();
	assert( qtFramework != nullptr );

	auto it = findTabFromView(view);
	assert(it != tabs_.end());
	const int index = qTabWidget_.indexOf(it->first);

	if (current_ == it->first)
	{
		it->second->focusOutEvent();
		it->second->deregisterListener(this);
		current_ = nullptr;
	}
	tabs_.erase(it);

	qTabWidget_.removeTab( index );
	// call this function to let IView control the qWidget's life-cycle again.
	qtFramework->retainQWidget( view );
}
} // end namespace wgt
