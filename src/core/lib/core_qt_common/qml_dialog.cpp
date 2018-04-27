#include "qml_dialog.hpp"

#include "core_common/assert.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_ui_framework/i_window.hpp"
#include "core_qt_common/i_qt_framework.hpp"
#include "core_qt_common/interfaces/i_qt_helpers.hpp"
#include "core_qt_common/helpers/qml_component_loader_helper.hpp"
#include "core_data_model/dialog/dialog_model.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_qt_common/qt_component_finder.hpp"
#include "core_string_utils/file_path.hpp"
#include "core_object/managed_object.hpp"
#include "core_logging/logging.hpp"

#include <QtCore/qnamespace.h>
#include <QQmlEngine>
#include <QQmlContext>
#include <QQuickWidget>
#include <QQmlComponent>
#include <QMainWindow>
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

struct QmlDialog::Implementation : Depends<IQtFramework>
{
	Implementation(QmlDialog& self, QQmlEngine& engine);

	~Implementation();

	void initialise();
	void load(const QUrl& url, QQmlComponent* component);
	void loadCommon(const QUrl& url, QQmlComponent* component);
	void reload(const QUrl& url, QQmlComponent* component);
	void waitForStateChangeFrom(QmlDialogState from);
	void onClose();
	void onShow();
	void setWatched(bool isWatched, QObject* root = nullptr);

	QmlDialog& self_;
	QQmlEngine& engine_;
	Result lastResult_;
	ObjectHandleT<DialogModel> model_;
	ManagedObjectPtr ownedModel_ = nullptr;
	std::unique_ptr<QQmlContext> qmlContext_;
	std::unique_ptr<QQuickWidget> frame_;
	Signal<ClosedSignature> closedSignal_;
	std::string title_;
	QmlDialogState state_;
	QMetaObject::Connection errorConnection_;
	std::set<QString> watchedComponents_;
	std::mutex loadMutex_;
	QtComponentFinder components_;
	QUrl url_;
	bool watched_;
};

QmlDialog::Implementation::Implementation(QmlDialog& self, QQmlEngine& engine)
    : self_(self), engine_(engine), model_(nullptr), qmlContext_(new QQmlContext(engine.rootContext())),
      state_(QmlDialogState::INVALID), watched_(false)
{
	initialise();
}

QmlDialog::Implementation::~Implementation()
{
	if (state_ != QmlDialogState::INVALID)
	{
		frame_->removeEventFilter(&self_);
	}
	setWatched(false);

	frame_->disconnect(errorConnection_);
	frame_.reset();
	qmlContext_.reset();
	engine_.collectGarbage();
}

void QmlDialog::Implementation::initialise()
{
	TF_ASSERT(state_ != QmlDialogState::OPEN);
	state_ = QmlDialogState::INVALID;
	lastResult_ = IDialog::INVALID_RESULT;
	title_ = "";
}

void QmlDialog::Implementation::setWatched(bool isWatched, QObject* root)
{
	if (isWatched != watched_)
	{
		auto watcher = get<IQtFramework>()->qmlWatcher();
		if (isWatched)
		{
			QObject::connect(watcher, SIGNAL(fileChanged(const QString&)), &self_, SLOT(reload(const QString&)));
		}
		else
		{
			QObject::disconnect(watcher, SIGNAL(fileChanged(const QString&)), &self_, SLOT(reload(const QString&)));
		}

		watched_ = isWatched;
	}

	if (isWatched)
	{
		const auto& types = components_.getTypes();
		watchedComponents_.insert(types.begin(), types.end());
	}
}

void QmlDialog::Implementation::loadCommon(const QUrl& url, QQmlComponent* component)
{
	std::unique_ptr<QObject> content(component->create(qmlContext_.get()));

	bool shouldBeWatched = false;
	if (url.scheme() == "file")
	{
		QVariant autoReload = content->property("autoReload");
		shouldBeWatched = !autoReload.isValid() || (autoReload.isValid() && autoReload.toBool());
	}
	setWatched(shouldBeWatched, content.get());

	QObject* rootObject = frame_->rootObject();
	if (rootObject)
	{
		rootObject->deleteLater();
	}

	QVariant titleProperty = content->property("title");
	if (titleProperty.type() == QVariant::Type::String)
	{
		title_ = titleProperty.toString().toUtf8().data();
	}

	if (content->property("modality").isValid())
	{
		NGT_WARNING_MSG("Dialog %s has an internal window mode", title_.c_str());
	}

	frame_->setWindowTitle(QString(title_.c_str()));
	frame_->setContent(url, component, content.release());
	frame_->setResizeMode(QQuickWidget::SizeRootObjectToView);
}

void QmlDialog::Implementation::load(const QUrl& url, QQmlComponent* component)
{
	initialise();

	loadCommon(url, component);

	frame_->installEventFilter(&self_);
	url_ = url;
	state_ = QmlDialogState::READY;
}

void QmlDialog::Implementation::reload(const QUrl& url, QQmlComponent* component)
{
	loadCommon(url, component);
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
	TF_ASSERT(model);

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
	TF_ASSERT(model);
	model->onShow();
}

QmlDialog::QmlDialog(QQmlEngine& engine) : QObject(nullptr), impl_(new Implementation(*this, engine))
{
}

QmlDialog::~QmlDialog()
{
}

const char* QmlDialog::title() const
{
	if (impl_->model_ != nullptr && impl_->model_->getTitle() != nullptr)
	{
		return impl_->model_->getTitle();
	}
	return impl_->title_.c_str();
}

IDialog::Result QmlDialog::result() const
{
	return impl_->lastResult_;
}

ObjectHandleT<DialogModel> QmlDialog::model() const
{
	return impl_->model_;
}

void QmlDialog::setModel(const std::nullptr_t&)
{
	impl_->qmlContext_->setContextObject(nullptr);
	impl_->qmlContext_->setContextProperty(QString("dialog"), nullptr);
	impl_->qmlContext_->setContextProperty(QString("qmlComponents"), nullptr);

	impl_->ownedModel_ = nullptr;
	impl_->model_ = nullptr;
}

void QmlDialog::setModel(ManagedObjectPtr model)
{
	impl_->ownedModel_ = std::move(model);
	setModel(impl_->ownedModel_->getHandleT<DialogModel>());
}

void QmlDialog::setModel(ObjectHandleT<DialogModel> model)
{
	TF_ASSERT(model.get());
	TF_ASSERT(impl_->state_ == QmlDialogState::INVALID || impl_->state_ == QmlDialogState::READY);

	DialogModel* dialogModel = model.get();

	QObject* qtModel = qvariant_cast<QObject*>(impl_->get<IQtFramework>()->toQVariant(model, impl_->qmlContext_.get()));
	impl_->qmlContext_->setContextObject(qtModel);

	impl_->qmlContext_->setContextProperty(QString("dialog"), this);
	impl_->qmlContext_->setContextProperty(QString("qmlComponents"), &impl_->components_);
	impl_->model_ = model;
}

void QmlDialog::load(const char* resource)
{
	TF_ASSERT(resource != nullptr);
	TF_ASSERT(impl_->state_ != QmlDialogState::OPEN);
	impl_->waitForStateChangeFrom(QmlDialogState::LOADING);
	impl_->state_ = QmlDialogState::LOADING;

	auto url = impl_->get<IQtFramework>()->resolveQmlPath(impl_->engine_, resource);

	if (url.scheme() == "file")
	{
		// Automatically watch any other files in the same folder as the view
		// as these can be added as components without using a module
		const QDir directory(FilePath::getFolder(url.toLocalFile().toUtf8().constData()).c_str());

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

	impl_->frame_.reset(new QQuickWidget(&impl_->engine_, nullptr));

	impl_->errorConnection_ =
	QObject::connect(impl_->frame_.get(), SIGNAL(sceneGraphError(QQuickWindow::SceneGraphError, const QString&)), this,
	                 SLOT(error(QQuickWindow::SceneGraphError, const QString&)));

	auto qmlComponent = new QQmlComponent(&impl_->engine_, impl_->frame_.get());
	QmlComponentLoaderHelper helper(qmlComponent, url);
	helper.data_->connections_ +=
	helper.data_->sig_Loaded_.connect(std::bind(&Implementation::load, impl_.get(), url, std::placeholders::_1));

	helper.load(true);
}

void QmlDialog::raise()
{
	if (impl_->state_ == QmlDialogState::OPEN)
	{
		impl_->frame_->raise();
	}
}

bool QmlDialog::isOpen() const
{
	return impl_->state_ == QmlDialogState::OPEN;
}

void QmlDialog::waitForClose()
{
	impl_->waitForStateChangeFrom(QmlDialogState::OPEN);
}

void QmlDialog::show(Mode mode)
{
	TF_ASSERT(impl_->state_ != QmlDialogState::OPEN);
	impl_->waitForStateChangeFrom(QmlDialogState::LOADING);
	impl_->state_ = QmlDialogState::OPEN;
	impl_->lastResult_ = INVALID_RESULT;
	if (mode == Mode::MODELESS)
	{
		// Set parent for modeless dialog to be on top of application, use parent widget in case active window is context menu.
		QWidget* parent = nullptr;
		for (QWidget* widget : QApplication::topLevelWidgets())
		{
			if (QMainWindow* mainWindow = qobject_cast<QMainWindow*>(widget))
			{
				parent = widget;
			}
		}
		impl_->frame_->setParent(parent);
		impl_->frame_->setWindowFlags(Qt::Tool);
	}
	impl_->frame_->setWindowModality(mode == Mode::MODAL || mode == Mode::MODAL_NONBLOCKING ? Qt::ApplicationModal : Qt::NonModal);
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
	NGT_ERROR_MSG("QmlView::error, rendering error: %s\n", message.toLatin1().constData());
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
		}
	}

	return QObject::eventFilter(object, event);
}

//------------------------------------------------------------------------------
void QmlDialog::reload(const QString& url)
{
	const QString name(FilePath::getFileNoExtension(url.toUtf8().constData()).c_str());
	if (impl_->watchedComponents_.find(name) != impl_->watchedComponents_.end())
	{
		std::unique_lock<std::mutex> holder(impl_->loadMutex_);

		auto qmlComponent = new QQmlComponent(impl_->qmlContext_->engine(), impl_->frame_.get());

		QmlComponentLoaderHelper helper(qmlComponent, impl_->url_);

		helper.data_->connections_ += helper.data_->sig_Loaded_.connect(
		std::bind(&Implementation::reload, impl_.get(), impl_->url_, std::placeholders::_1));

		helper.load(true);
	}
}

} // end namespace wgt
