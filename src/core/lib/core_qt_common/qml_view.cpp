#include "qml_view.hpp"

#include "core_common/assert.hpp"
#include "core_logging/logging.hpp"
#include "i_qt_framework.hpp"
#include "core_ui_framework/i_preferences.hpp"
#include "core_qt_common/qt_connection_holder.hpp"
#include "core_qt_common/helpers/qml_component_loader_helper.hpp"
#include "core_qt_common/interfaces/i_qt_helpers.hpp"
#include "core_qt_common/qt_component_finder.hpp"
#include "core_qt_common/qt_script_object.hpp"
#include "core_string_utils/string_utils.hpp"
#include "core_string_utils/file_path.hpp"

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
struct QmlView::Implementation : Depends<IQtFramework>
{
	Implementation(QmlView& qmlView, QQmlEngine& qmlEngine)
	    : qmlView_(qmlView),
	      qmlContext_(new QQmlContext(qmlEngine.rootContext())), qmlEngine_(qmlEngine),
	      watched_(false), loadNeeded_(false)
	{
		qmlView.setWidget(new QQuickWidget(&qmlEngine, nullptr));
		QQmlEngine::setContextForObject(qmlView.widget(), qmlContext_.get());
	}

	virtual ~Implementation()
	{
		std::unique_lock<std::mutex> holder(loadMutex_);
		setWatched(false);
		qmlView_.deleteWidget();
		qmlEngine_.collectGarbage();
	}

	void handleLoaded(QQmlComponent* qmlComponent)
	{
		auto content = std::unique_ptr<QObject>(qmlComponent->create(qmlContext_.get()));
		qmlView_.QtViewCommon::initialise(content.get());

		auto property = content->property("layoutHints");
		auto& hint = qmlView_.hint();
		hint.clear();

		if (property.isValid())
		{
			auto hintsMap = property.value<QVariantMap>();

			for (auto it = hintsMap.cbegin(); it != hintsMap.cend(); ++it)
			{
				hint += LayoutHint(it.key().toUtf8(), it.value().toFloat());
			}
		}

		bool shouldBeWatched = false;

		if (url_.scheme() == "file")
		{
			QVariant autoReload = content->property("autoReload");
			shouldBeWatched = !autoReload.isValid() || (autoReload.isValid() && autoReload.toBool());
		}

		setWatched(shouldBeWatched, content.get());
		auto view = static_cast<QQuickWidget*>(qmlView_.widget());

		if (QObject* rootObject = view->rootObject())
		{
			rootObject->deleteLater();
		}

		view->setContent(url_, qmlComponent, content.release());
		view->setResizeMode(QQuickWidget::SizeRootObjectToView);
		view->setFocusPolicy(Qt::ClickFocus);

		qmlView_.connect(view->rootObject(), SIGNAL(activeFocusChanged(bool)), SLOT(onFocusChanged(bool)));
	}

	void setWatched(bool isWatched, QObject* root = nullptr)
	{
		if (isWatched != watched_)
		{
			auto watcher = get<IQtFramework>()->qmlWatcher();
			if (isWatched)
			{
				QObject::connect(watcher, SIGNAL(fileChanged(const QString&)), &qmlView_, SLOT(reload(const QString&)));
			}
			else
			{
				QObject::disconnect(watcher, SIGNAL(fileChanged(const QString&)), &qmlView_,
				                    SLOT(reload(const QString&)));
			}

			watched_ = isWatched;
		}

		if (isWatched)
		{
			const auto& types = components_.getTypes();
			watchedComponents_.insert(types.begin(), types.end());
		}
	}

	bool doLoad(const QUrl& qUrl, std::function<void()> loadedHandler = [] {},
	            std::function<void()> errorHandler = [] {}, bool async = true)
	{
		std::unique_lock<std::mutex> holder(loadMutex_);
		auto qmlEngine = qmlContext_->engine();
		auto qmlComponent = new QQmlComponent(qmlEngine, qmlView_.widget());

		QmlComponentLoaderHelper helper(qmlComponent, qUrl);
		using namespace std::placeholders;
		helper.data_->connections_ += helper.data_->sig_Loaded_.connect(
			std::bind(&Implementation::handleLoaded, this, _1));
		helper.data_->connections_ +=
		helper.data_->sig_Loaded_.connect([loadedHandler](QQmlComponent*) { loadedHandler(); });
		helper.data_->connections_ +=
		helper.data_->sig_Error_.connect([errorHandler](QQmlComponent*) { errorHandler(); });
		helper.load(async);
		return true;
	}

	QmlView& qmlView_;
	CursorId cursorId_ = ArrowCursor;
	QtComponentFinder components_;
	QUrl url_;
	std::unique_ptr<QQmlContext> qmlContext_;
	QQmlEngine& qmlEngine_;
	std::mutex loadMutex_;
	bool watched_;
	bool loadNeeded_;
	std::set<QString> watchedComponents_;
};

//==============================================================================
QmlView::QmlView(const char* id, QQmlEngine& qmlEngine)
	: QtViewCommon(id), impl_(new Implementation(*this, qmlEngine))
{
	QObject::connect(widget(), SIGNAL(sceneGraphError(QQuickWindow::SceneGraphError, const QString&)), this,
	                 SLOT(error(QQuickWindow::SceneGraphError, const QString&)));
}

//------------------------------------------------------------------------------
QmlView::~QmlView()
{
	// call sendPostedEvents to give chance to QScriptObject's DeferredDeleted event get handled in time
	QApplication::sendPostedEvents(nullptr, QEvent::DeferredDelete);
}

const std::set<QString>& QmlView::componentTypes() const
{
	return impl_->components_.getTypes();
}

//------------------------------------------------------------------------------
void QmlView::update()
{
}

//------------------------------------------------------------------------------
void QmlView::reload()
{
	impl_->doLoad(impl_->url_);
}

void QmlView::setFocus(bool focus)
{
	QtViewCommon::setFocus(focus);
	auto rootObject = static_cast<QQuickWidget*>(widget())->rootObject();

	if (!rootObject || focus == rootObject->hasActiveFocus())
	{
		return;
	}

	focus ? rootObject->forceActiveFocus() : rootObject->setFocus(false);
}

//------------------------------------------------------------------------------
void QmlView::setContextObject(QObject* object)
{
	// Cannot use qobject_cast because QtScriptObject doesn't have a static
	// MetaObject implementation.
	auto qtScriptObject = dynamic_cast<QtScriptObject*>(object);
	if (qtScriptObject)
	{
		qtScriptObject->setParent(impl_->qmlContext_.get());
	}
	else
	{
		object->setParent(impl_->qmlContext_.get());
	}
	impl_->qmlContext_->setContextObject(object);
}

//------------------------------------------------------------------------------
void QmlView::setContextProperty(const QString& name, const QVariant& property)
{
	if (property.canConvert<QObject*>())
	{
		auto object = property.value<QObject*>();
		if (!object->isWidgetType() && !object->isWindowType())
		{
			// Cannot use qobject_cast because QtScriptObject doesn't have a static
			// MetaObject implementation.
			auto qtScriptObject = dynamic_cast<QtScriptObject*>(object);
			if (qtScriptObject)
			{
				qtScriptObject->setParent(impl_->qmlContext_.get());
			}
			else
			{
				object->setParent(impl_->qmlContext_.get());
			}
		}
	}
	impl_->qmlContext_->setContextProperty(name, property);
}

//------------------------------------------------------------------------------
void QmlView::error(QQuickWindow::SceneGraphError error, const QString& message)
{
	NGT_ERROR_MSG("QmlView::error, rendering error: %s\n", message.toLatin1().constData());
}

//------------------------------------------------------------------------------
bool QmlView::load(const QUrl& qUrl, std::function<void()> loadedHandler, std::function<void()> errorHandler,
                   bool async)
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

	auto qtFramework = impl_->get<IQtFramework>();
	auto preferences = qtFramework->getPreferences();
	auto preference = preferences->getPreference(id());
	auto value = qtFramework->toQVariant(preference, widget());
	this->setContextProperty(QString("viewPreference"), value);
	this->setContextProperty(QString("viewId"), id());
	this->setContextProperty(QString("viewWidget"), QVariant::fromValue(widget()));
	impl_->qmlContext_->setContextProperty(QString("qmlView"), this);
	impl_->qmlContext_->setContextProperty(QString("qmlComponents"), &impl_->components_);

	return impl_->doLoad(qUrl, loadedHandler, errorHandler, async);
}

//------------------------------------------------------------------------------
void QmlView::reload(const QString& url)
{
	const QString name(FilePath::getFileNoExtension(url.toUtf8().constData()).c_str());
	if (impl_->watchedComponents_.find(name) != impl_->watchedComponents_.end())
	{
		reload();
	}
}

void QmlView::setNeedsToLoad(bool load)
{
	if (load == impl_->loadNeeded_)
	{
		return;
	}

	impl_->loadNeeded_ = load;
	emit needsToLoadChanged();
}

bool QmlView::getNeedsToLoad() const
{
	return impl_->loadNeeded_;
}

void QmlView::onFocusChanged(bool focused)
{
	setFocus(focused);
}

CursorId QmlView::getCursor() const
{
	if (impl_->cursorId_ != ArrowCursor)
	{
		return impl_->cursorId_;
	}

	auto shape = widget()->cursor().shape();
	TF_ASSERT(shape <= LastCursor.id());
	return CursorId::make(shape, nullptr);
}

void QmlView::setCursor(CursorId cursorId)
{
	impl_->cursorId_ = cursorId;
	auto widget = this->widget();

	if(cursorId.id() > LastCursor.id())
	{
		auto qCursor = reinterpret_cast<QCursor*>(cursorId.nativeCursor());
		widget->setCursor(*qCursor);
	}
	else
	{
		auto shape = static_cast<Qt::CursorShape>(cursorId.id());
		widget->setCursor(shape);
	}
}

void QmlView::unsetCursor()
{
	impl_->cursorId_ = ArrowCursor;
	widget()->unsetCursor();
}
} // end namespace wgt
