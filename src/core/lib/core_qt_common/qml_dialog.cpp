#include "qml_dialog.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_ui_framework/i_window.hpp"
#include "core_qt_common/i_qt_framework.hpp"
#include "core_qt_common/helpers/qt_helpers.hpp"
#include "core_qt_common/helpers/qml_component_loader_helper.hpp"
#include "core_data_model/dialog/dialog_model.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_qt_common/qt_component_finder.hpp"
#include "core_string_utils/file_path.hpp"

#include <QQmlEngine>
#include <QQmlContext>
#include <QQuickWidget>
#include <QQmlComponent>
#include <QString>
#include <QVariant>
#include <QDir>
#include <QUrl>
#include <QFileSystemWatcher>
#include <QApplication>
#include <QQuickItem>

#include <mutex>

namespace wgt
{
enum class QmlDialogState
{
	INVALID,
	LOADING,
	READY,
	OPEN
};

struct QmlDialog::Implementation
{
	Implementation(QmlDialog& self,
	               IQtFramework& framework,
	               IComponentContext& componentContext,
	               QQmlEngine& engine);

	~Implementation();

	void initialise();
	void load( const QUrl& url, QQmlComponent* component );
	void loadCommon( const QUrl& url, QQmlComponent* component );
	void reload( const QUrl& url, QQmlComponent* component );
	ObjectHandleT<DialogModel> createDefaultModel();
	void waitForStateChangeFrom(QmlDialogState from);
	void onClose();
	void onShow();
	void focusInEvent();
	void focusOutEvent();
	void findChildDialogs( QObject* obj );
	void setWatched( bool isWatched, QObject* root = nullptr );

	QmlDialog& self_;
	IQtFramework& framework_;
	IComponentContext& componentContext_;
	QQmlEngine& engine_;
	Result lastResult_;
	ObjectHandleT<DialogModel> model_;
	std::unique_ptr<QQmlContext> qmlContext_;
	std::unique_ptr<QQuickWidget> frame_;
	Signal<ClosedSignature> closedSignal_;
	std::string title_;
	QmlDialogState state_;
	QMetaObject::Connection errorConnection_;
	std::vector<QQuickItem*> childDialogs_;
	std::set<QString> watchedComponents_;
	std::mutex loadMutex_;
	QtComponentFinder components_;
	QUrl url_;
	bool watched_;
};

QmlDialog::Implementation::Implementation(QmlDialog& self,
                                          IQtFramework& framework,
                                          IComponentContext& componentContext,
                                          QQmlEngine& engine)
    : self_(self)
    , framework_(framework)
    , componentContext_(componentContext)
    , engine_(engine)
    , model_(nullptr)
    , qmlContext_(new QQmlContext(engine.rootContext()))
    , state_(QmlDialogState::INVALID)
	, watched_ (false)
{
	initialise();
}

QmlDialog::Implementation::~Implementation()
{
	if (state_ != QmlDialogState::INVALID)
	{
		frame_->removeEventFilter(&self_);
	}
	setWatched( false );

	frame_->disconnect(errorConnection_);
	frame_.reset();
	qmlContext_.reset();
	engine_.collectGarbage();
}

void QmlDialog::Implementation::initialise()
{
	assert(state_ != QmlDialogState::OPEN);
	state_ = QmlDialogState::INVALID;
	lastResult_ = IDialog::INVALID_RESULT;
	title_ = "";
}

void QmlDialog::Implementation::setWatched( bool isWatched, QObject* root )
{
	if (isWatched != watched_)
	{
		auto watcher = framework_.qmlWatcher();
		if (isWatched)
		{
			QObject::connect( watcher, SIGNAL( fileChanged( const QString& ) ), &self_, SLOT( reload( const QString& ) ) );
		}
		else
		{
			QObject::disconnect( watcher, SIGNAL( fileChanged( const QString& ) ), &self_, SLOT( reload( const QString& ) ) );
		}

		watched_ = isWatched;
	}

	if (isWatched)
	{
		const auto& types = components_.getTypes();
		watchedComponents_.insert( types.begin(), types.end() );
	}
}

void QmlDialog::Implementation::loadCommon( const QUrl& url, QQmlComponent* component )
{
	std::unique_ptr<QObject> content( component->create( qmlContext_.get() ) );
	QVariant titleProperty = content->property( "title" );

	bool shouldBeWatched = false;
	if (url.scheme() == "file")
	{
		QVariant autoReload = content->property( "autoReload" );
		shouldBeWatched = !autoReload.isValid() || (autoReload.isValid() && autoReload.toBool());
	}
	setWatched( shouldBeWatched, content.get() );
	
	childDialogs_.clear();
	findChildDialogs( content.get() );

	QObject * rootObject = frame_->rootObject();
	if (rootObject)
	{
		rootObject->deleteLater();
	}

	frame_->setContent( url, component, content.release() );
	frame_->setResizeMode( QQuickWidget::SizeRootObjectToView );

	if (titleProperty.type() == QVariant::Type::String)
	{
		title_ = titleProperty.toString().toUtf8().data();
		frame_->setWindowTitle( titleProperty.toString() );
	}
}

void QmlDialog::Implementation::load( const QUrl& url, QQmlComponent* component )
{
	initialise();

	loadCommon( url, component );

	frame_->installEventFilter(&self_);
	url_ = url;
	state_ = QmlDialogState::READY;
}

void QmlDialog::Implementation::reload( const QUrl& url, QQmlComponent* component )
{
	loadCommon( url, component );
}

void QmlDialog::Implementation::findChildDialogs(QObject* obj)
{
	for (QObject* child : obj->children())
	{
		if (child->inherits("QQuickAbstractDialog"))
		{
			auto* frame = dynamic_cast<QQuickItem*>(child->parent());
			if (frame)
			{
				childDialogs_.push_back(frame);
				break;
			}
		}
		findChildDialogs(child);
	}
}

ObjectHandleT<DialogModel> QmlDialog::Implementation::createDefaultModel()
{
	auto definitionManager = componentContext_.queryInterface<IDefinitionManager>();
	assert(definitionManager);

	return definitionManager->create<DialogModel>();
}

void QmlDialog::Implementation::waitForStateChangeFrom(QmlDialogState from)
{
	while (state_ == from)
	{
		QApplication::processEvents();
		std::this_thread::yield();
	}
}

void QmlDialog::Implementation::onClose()
{
	auto model = model_.get();
	assert(model);

	if (lastResult_ == IDialog::INVALID_RESULT)
	{
		lastResult_ = model->result();
	}

	model->onClose(lastResult_);
	state_ = QmlDialogState::READY;
	closedSignal_(self_);
}

void QmlDialog::Implementation::onShow()
{
	auto model = model_.get();
	assert(model);
	model->onShow();
}

void QmlDialog::Implementation::focusInEvent()
{
	auto model = model_.get();
	assert(model);
	model->onFocusIn();
}

void QmlDialog::Implementation::focusOutEvent()
{
	const auto findFocused = [](const QQuickItem* obj) { return obj->hasFocus(); };
	const auto itr = std::find_if(childDialogs_.begin(), childDialogs_.end(), findFocused);

	auto model = model_.get();
	assert(model);
	model->onFocusOut(itr != childDialogs_.end());
}

QmlDialog::QmlDialog(IComponentContext& context,
                     QQmlEngine& engine,
                     IQtFramework& framework)
    : QObject(nullptr)
    , impl_(new Implementation(*this, framework, context, engine))
{
	impl_->errorConnection_ = QObject::connect(impl_->frame_.get(),
	                                           SIGNAL(sceneGraphError(QQuickWindow::SceneGraphError, const QString&)),
	                                           this, SLOT(error(QQuickWindow::SceneGraphError, const QString&)));
}

QmlDialog::~QmlDialog()
{
	// call sendPostedEvents to give chance to QScriptObject's DeferredDeleted event get handled in time
	QApplication::sendPostedEvents(nullptr, QEvent::DeferredDelete);
}

const char* QmlDialog::title() const
{
	return impl_->title_.c_str();
}

IDialog::Result QmlDialog::result() const
{
	return impl_->lastResult_;
}

void QmlDialog::setModel(ObjectHandleT<DialogModel> model)
{
	assert(impl_->state_ == QmlDialogState::INVALID || impl_->state_ == QmlDialogState::READY);

	if (model.get() == nullptr)
	{
		model = impl_->createDefaultModel();
	}

	DialogModel* dialogModel = model.get();

	QObject* qtModel = qvariant_cast<QObject*>(impl_->framework_.toQVariant(model, impl_->qmlContext_.get()));
	impl_->qmlContext_->setContextObject(qtModel);

	impl_->qmlContext_->setContextProperty(QString("dialog"), this);
	impl_->qmlContext_->setContextProperty( QString( "qmlComponents" ), &impl_->components_ );
	impl_->model_ = model;
}

void QmlDialog::load(const char* resource)
{
	assert(impl_->state_ != QmlDialogState::OPEN);
	impl_->waitForStateChangeFrom(QmlDialogState::LOADING);
	impl_->state_ = QmlDialogState::LOADING;

	auto url = QtHelpers::resolveQmlPath(impl_->engine_, resource);

	if (url.scheme() == "file")
	{
		// Automatically watch any other files in the same folder as the view
		// as these can be added as components without using a module
		const QDir directory( FilePath::getFolder( url.toLocalFile().toUtf8().constData() ).c_str() );

		QStringList filter;
		filter.push_back( "*.qml" );
		filter.push_back( "*.js" );

		const auto files = directory.entryList( filter );
		for (const auto& file : files)
		{
			const auto name( FilePath::getFileNoExtension( file.toUtf8().constData() ) );
			impl_->watchedComponents_.insert( QString( name.c_str() ) );
		}
	}

	impl_->frame_.reset( new QQuickWidget( &impl_->engine_, nullptr ) );
	auto qmlComponent = new QQmlComponent( &impl_->engine_, impl_->frame_.get() );
	QmlComponentLoaderHelper helper(qmlComponent, url);
	helper.data_->connections_ += helper.data_->sig_Loaded_.connect(
	std::bind(&Implementation::load, impl_.get(), url, std::placeholders::_1));

	helper.load(true);
}

void QmlDialog::show(Mode mode)
{
	assert(impl_->state_ != QmlDialogState::OPEN);
	impl_->waitForStateChangeFrom(QmlDialogState::LOADING);
	impl_->state_ = QmlDialogState::OPEN;
	impl_->lastResult_ = INVALID_RESULT;

	if (!impl_->model_.get())
	{
		setModel(impl_->createDefaultModel());
	}

	impl_->frame_->setWindowModality(mode == Mode::MODAL ? Qt::ApplicationModal : Qt::NonModal);
	impl_->frame_->setWindowTitle(title());
	impl_->frame_->show();

	if (mode == IDialog::Mode::MODAL)
	{
		impl_->waitForStateChangeFrom(QmlDialogState::OPEN);
	}
}

void QmlDialog::close(IDialog::Result result)
{
	if (impl_->state_ != QmlDialogState::OPEN)
	{
		return;
	}

	impl_->lastResult_ = result;
	impl_->frame_->close();
}

Connection QmlDialog::connectClosedCallback(ClosedCallback callback)
{
	return impl_->closedSignal_.connect(callback);
}

void QmlDialog::setParent(QObject* parent)
{
	QObject::setParent(parent);
}

void QmlDialog::error(QQuickWindow::SceneGraphError error, const QString& message)
{
	NGT_ERROR_MSG("QmlView::error, rendering error: %s\n",
	              message.toLatin1().constData());
}

ObjectHandle QmlDialog::model() const
{
	return impl_->model_;
}

IComponentContext& QmlDialog::componentContext() const
{
	return impl_->componentContext_;
}

QQmlEngine& QmlDialog::engine() const
{
	return impl_->engine_;
}

bool QmlDialog::eventFilter(QObject* object, QEvent* event)
{
	if (object == impl_->frame_.get())
	{
		switch (event->type())
		{
		case QEvent::Close:
			impl_->onClose();
			return true;
		case QEvent::Show:
			impl_->onShow();
			break;
		case QEvent::FocusIn:
			impl_->focusInEvent();
			break;
		case QEvent::FocusOut:
			impl_->focusOutEvent();
			break;
		}
	}

	return QObject::eventFilter(object, event);
}

//------------------------------------------------------------------------------
void QmlDialog::reload( const QString& url )
{
	const QString name( FilePath::getFileNoExtension( url.toUtf8().constData() ).c_str() );
	if (impl_->watchedComponents_.find( name ) != impl_->watchedComponents_.end())
	{
		std::unique_lock< std::mutex > holder( impl_->loadMutex_ );

		auto qmlComponent = new QQmlComponent( impl_->qmlContext_->engine(), impl_->frame_.get() );

		QmlComponentLoaderHelper helper( qmlComponent, impl_->url_ );

		helper.data_->connections_ += helper.data_->sig_Loaded_.connect(
			std::bind( &Implementation::reload, impl_.get(), impl_->url_, std::placeholders::_1 ) );

		helper.load( true );
	}
}

} // end namespace wgt
