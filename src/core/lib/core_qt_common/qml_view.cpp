#include "qml_view.hpp"

#include "core_logging/logging.hpp"
#include "i_qt_framework.hpp"
#include "core_ui_framework/i_preferences.hpp"
#include "core_qt_common/qt_connection_holder.hpp"
#include "core_qt_common/helpers/qml_component_loader_helper.hpp"
#include "core_qt_common/helpers/qt_helpers.hpp"
#include "core_qt_common/qt_component_finder.hpp"
#include "core_qt_script/qt_script_object.hpp"
#include "core_string_utils/string_utils.hpp"
#include "core_string_utils/file_path.hpp"

#include <cassert>
#include <functional>

#include <QQmlComponent>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickWidget>
#include <QQuickItem>
#include <QVariantMap>
#include <QFileSystemWatcher>
#include <QApplication>
#include <QDir>

namespace wgt
{
//==============================================================================
struct QmlView::Impl
{
	Impl(QmlView& qmlView, const char* id,
	     IQtFramework& qtFramework, QQmlEngine& qmlEngine)
	    : qtFramework_(qtFramework)
	    , id_(id)
	    , qmlView_(qmlView)
	    , quickView_(new QQuickWidget(&qmlEngine, nullptr))
	    , qmlContext_(new QQmlContext(qmlEngine.rootContext()))
	    , qmlEngine_(qmlEngine)
	    , released_(false)
	    , watched_(false)
	{
		QQmlEngine::setContextForObject(quickView_, qmlContext_.get());
	}

	~Impl()
	{
		std::unique_lock< std::mutex > holder(loadMutex_);
		setWatched(false);
		if (!released_)
		{
			delete quickView_;
		}
		qmlEngine_.collectGarbage();
	}


	void handleLoaded( QQmlComponent * qmlComponent )
	{
		auto content = std::unique_ptr< QObject >(
			qmlComponent->create(qmlContext_.get()));

		auto hintsProperty = content->property("layoutHints");
		hint_.clear();

		if (hintsProperty.isValid())
		{
			auto hintsMap = hintsProperty.value< QVariantMap >();
			for (auto it = hintsMap.cbegin(); it != hintsMap.cend(); ++it)
			{
				hint_ += LayoutHint(
					it.key().toUtf8(), it.value().toFloat());
			}
		}

		QVariant windowProperty = content->property("windowId");
		windowId_ = "";
		if (windowProperty.isValid())
		{
			windowId_ = windowProperty.toString().toUtf8().data();
		}

		QVariant titleProperty = content->property("title");
		title_ = "";
		if (titleProperty.isValid())
		{
			title_ = titleProperty.toString().toUtf8().data();
		}

		bool shouldBeWatched = false;
		if (url_.scheme() == "file")
		{
			QVariant autoReload = content->property("autoReload");
			shouldBeWatched = !autoReload.isValid() || (autoReload.isValid() && autoReload.toBool());
		}
		setWatched(shouldBeWatched, content.get());

		QObject * rootObject = quickView_->rootObject();
		if (rootObject)
		{
			rootObject->deleteLater();
		}

		quickView_->setContent(url_, qmlComponent, content.release());
		quickView_->setResizeMode(QQuickWidget::SizeRootObjectToView);
		for (auto& l : listeners_ )
		{
			l->onLoaded( &qmlView_ );
		}
	}

	void setWatched(bool isWatched, QObject* root = nullptr)
	{
		if (isWatched != watched_)
		{
			auto watcher = qtFramework_.qmlWatcher();
			if (isWatched)
			{
				QObject::connect(watcher, SIGNAL(fileChanged(const QString&)), &qmlView_, SLOT(reload(const QString&)));
			}
			else
			{
				QObject::disconnect(watcher, SIGNAL(fileChanged(const QString&)), &qmlView_, SLOT(reload(const QString&)));
			}

			watched_ = isWatched;
		}

		if (isWatched)
		{
			const auto& types = components_.getTypes();
			watchedComponents_.insert(types.begin(), types.end());
		}
	}

	bool doLoad(const QUrl & qUrl, std::function< void() > loadedHandler = [] {},
                std::function< void() > errorHandler = [] {}, bool async = true )
	{
		std::unique_lock< std::mutex > holder( loadMutex_ );
		auto qmlEngine = qmlContext_->engine();

		auto qmlComponent = new QQmlComponent(qmlEngine, quickView_);

		QmlComponentLoaderHelper helper(qmlComponent, qUrl );
		using namespace std::placeholders;
		helper.data_->connections_ +=
			helper.data_->sig_Loaded_.connect(std::bind(&Impl::handleLoaded, this, _1));
		helper.data_->connections_ +=
			helper.data_->sig_Loaded_.connect([ loadedHandler ]( QQmlComponent * )
		{
			loadedHandler();
		});
        helper.data_->connections_ += 
            helper.data_->sig_Error_.connect([ errorHandler ]( QQmlComponent * )
        {
            errorHandler();
        });
		helper.load( async );
		return true;
	}

	IQtFramework & qtFramework_;
	std::string id_;
	typedef std::vector<IViewEventListener*> Listeners;
	Listeners listeners_;
	QmlView & qmlView_;
	std::string windowId_;
	std::string title_;
	QQuickWidget * quickView_;
	QtComponentFinder components_;
	QUrl url_;
	std::unique_ptr< QQmlContext > qmlContext_;
	QQmlEngine & qmlEngine_;
	LayoutHint hint_;
	bool released_;
	std::mutex loadMutex_;
	bool watched_;
	std::set<QString> watchedComponents_;
};

//==============================================================================
QmlView::QmlView( const char * id, IQtFramework & qtFramework, QQmlEngine & qmlEngine )
	: impl_( new Impl( *this, id, qtFramework, qmlEngine ) )
	, needLoad_(false)
{
	QObject::connect( impl_->quickView_, SIGNAL(sceneGraphError(QQuickWindow::SceneGraphError, const QString&)),
		this, SLOT(error(QQuickWindow::SceneGraphError, const QString&)));
}


//------------------------------------------------------------------------------
QmlView::~QmlView()
{
    // call sendPostedEvents to give chance to QScriptObject's DeferredDeleted event get handled in time
    QApplication::sendPostedEvents( nullptr, QEvent::DeferredDelete );
}


//------------------------------------------------------------------------------
const char * QmlView::id() const
{
	return impl_->id_.c_str();
}


//------------------------------------------------------------------------------
const char * QmlView::title() const
{
	return impl_->title_.c_str();
}


//------------------------------------------------------------------------------
const char * QmlView::windowId() const
{
	return impl_->windowId_.c_str();
}


//------------------------------------------------------------------------------
const LayoutHint& QmlView::hint() const
{
	return impl_->hint_;
}


//------------------------------------------------------------------------------
QWidget * QmlView::releaseView()
{
	impl_->released_ = true;
	return view();
}


//------------------------------------------------------------------------------
void QmlView::retainView()
{
	impl_->released_ = false;
	impl_->quickView_->setParent( nullptr );
}


//------------------------------------------------------------------------------
QWidget * QmlView::view() const
{
	return impl_->quickView_;
}

//------------------------------------------------------------------------------
const std::set<QString>& QmlView::componentTypes() const
{
	return impl_->components_.getTypes();
}

//------------------------------------------------------------------------------
void QmlView::update()
{

}


//------------------------------------------------------------------------------
void QmlView::setContextObject( QObject * object )
{
	//Cannot use qobject_cast because QtScriptObject doesn't have a static 
	//MetaObject implementation.
	auto qtScriptObject = dynamic_cast<QtScriptObject *>( object );
	if(qtScriptObject)
	{
		qtScriptObject->setParent( impl_->qmlContext_.get() );
	}
	else
	{
		object->setParent(impl_->qmlContext_.get());
	}
	impl_->qmlContext_->setContextObject( object );
}


//------------------------------------------------------------------------------
void QmlView::setContextProperty(
	const QString & name, const QVariant & property )
{
    if (property.canConvert< QObject * >())
    {
        auto object = property.value< QObject * >();
        if(!object->isWidgetType() && !object->isWindowType())
        {
			//Cannot use qobject_cast because QtScriptObject doesn't have a static 
			//MetaObject implementation.
			auto qtScriptObject = dynamic_cast<QtScriptObject *>( object );
			if(qtScriptObject)
			{
				qtScriptObject->setParent( impl_->qmlContext_.get() );
			}
			else
			{
				object->setParent(impl_->qmlContext_.get());
			}
        }
    }
	impl_->qmlContext_->setContextProperty( name, property );
}


//------------------------------------------------------------------------------
void QmlView::error( QQuickWindow::SceneGraphError error, const QString &message )
{
	NGT_ERROR_MSG( "QmlView::error, rendering error: %s\n",
		message.toLatin1().constData() );
}


//------------------------------------------------------------------------------
bool QmlView::load( const QUrl & qUrl, std::function< void() > loadedHandler, 
                   std::function< void() > errorHandler, bool async )
{
	impl_->url_ = qUrl;

	if (qUrl.scheme() == "file")
	{
		// Automatically watch any other files in the same folder as the view
		// as these can be added as components without using a module
		const QDir directory(FilePath::getFolder(qUrl.toLocalFile().toUtf8().constData()).c_str());

		QStringList filter;
		filter.push_back("*.qml");
		filter.push_back("*.js");

		const auto files = directory.entryList(filter);
		for (const auto& file : files)
		{
			const auto name(FilePath::getFileNoExtension(file.toUtf8().constData()));
			impl_->watchedComponents_.insert(QString(name.c_str()));
		}
	}

	auto preferences = impl_->qtFramework_.getPreferences();
	auto preference = preferences->getPreference(impl_->id_.c_str() );
	auto value = impl_->qtFramework_.toQVariant( preference, view() );
	this->setContextProperty( QString( "preference" ), value );
	this->setContextProperty( QString( "viewId" ), impl_->id_.c_str() );
	this->setContextProperty( QString( "View" ), QVariant::fromValue(impl_->quickView_ ) );
	impl_->qmlContext_->setContextProperty(QString("qmlView"), this);
	impl_->qmlContext_->setContextProperty(QString("qmlComponents"), &impl_->components_);

	return impl_->doLoad( qUrl, loadedHandler, errorHandler, async );
}


//------------------------------------------------------------------------------
void QmlView::reload(const QString& url)
{
	const QString name(FilePath::getFileNoExtension(url.toUtf8().constData()).c_str());
	if (impl_->watchedComponents_.find(name) != impl_->watchedComponents_.end())
	{
		impl_->doLoad(impl_->url_);
	}
}

//------------------------------------------------------------------------------
void QmlView::focusInEvent()
{
	for (auto& l : impl_->listeners_)
	{
		l->onFocusIn( this );
	}
	auto rootObject = impl_->quickView_->rootObject();
	if (rootObject)
	{
		rootObject->setFocus(true);
	}
}


//------------------------------------------------------------------------------
void QmlView::focusOutEvent()
{
	for (auto& l : impl_->listeners_)
	{
		l->onFocusOut( this );
	}
	auto rootObject = impl_->quickView_->rootObject();
	if (rootObject)
	{
		rootObject->setFocus(false);
	}
}

//------------------------------------------------------------------------------
void QmlView::setNeedsToLoad(bool load)
{
	if(load == needLoad_)
	{
		return;
	}
	needLoad_ = load;
	emit needsToLoadChanged();
}

//------------------------------------------------------------------------------
bool QmlView::getNeedsToLoad() const
{
	return needLoad_;
}

//------------------------------------------------------------------------------
void QmlView::registerListener(IViewEventListener* listener)
{
	assert( std::find(impl_->listeners_.begin(), impl_->listeners_.end(), listener ) == impl_->listeners_.end() );
	impl_->listeners_.push_back( listener );
}


//------------------------------------------------------------------------------
void QmlView::deregisterListener(IViewEventListener* listener)
{
	auto it = std::find(impl_->listeners_.begin(), impl_->listeners_.end(), listener );
	assert( it != impl_->listeners_.end() );
	impl_->listeners_.erase( it );
}
} // end namespace wgt
