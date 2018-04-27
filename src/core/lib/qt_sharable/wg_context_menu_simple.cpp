#include "wg_context_menu_simple.hpp"

#include "core_common/assert.hpp"
#include "core_logging/logging.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_ui_framework/i_action.hpp"
#include "core_ui_framework/i_menu.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/i_window.hpp"
#include "core_qt_common/qt_context_menu.hpp"
#include "core_dependency_system/depends.hpp"
#include <QMenu>
#include <QString>
#include <QVariant>
#include <QQuickWidget>
#include <QApplication>

namespace wgt
{
struct WGContextMenuSimple::Implementation
{
	Implementation(WGContextMenuSimple& self) : self_(self), view_(nullptr)
	{
	}

	~Implementation()
	{
		destroyMenu();
	}

	void onComponentComplete(WGContextMenuSimple* contextMenu)
	{
		auto context = QQmlEngine::contextForObject(contextMenu);
		TF_ASSERT(context != nullptr);

		auto viewProperty = context->contextProperty("viewWidget");
		if (viewProperty.isValid())
		{
			view_ = qvariant_cast<QQuickWidget*>(viewProperty);
		}

		auto windowIdProperty = context->contextProperty("windowId");
		if (windowIdProperty.isValid())
		{
			windowId_ = windowIdProperty.toString().toUtf8().data();
		}

		destroyMenu();
		createMenu();
		prepareMenu();
	}

	void createMenu()
	{
		TF_ASSERT(qMenu_ == nullptr);

		qMenu_.reset(new QMenu());
		qMenu_->setProperty("path", QString(path_.c_str()));
		connections_ += QObject::connect(qMenu_.get(), &QMenu::aboutToShow, [&]() { emit self_.aboutToShow(); });
		connections_ += QObject::connect(qMenu_.get(), &QMenu::aboutToHide, [&]() { emit self_.aboutToHide(); });
		connections_ +=
		QObject::connect(qApp, &QApplication::paletteChanged, [&]() { qtContextMenu_->onPaletteChanged(); });

		TF_ASSERT(qtContextMenu_ == nullptr);
		qtContextMenu_.reset(new QtContextMenu(*qMenu_, view_, windowId_.c_str()));
	}

	void destroyMenu()
	{
		if (qtContextMenu_ == nullptr)
		{
			return;
		}

		connections_.reset();
		qMenu_.reset();
		qtContextMenu_.reset();
	}

	void prepareMenu()
	{
		if (qtContextMenu_ == nullptr)
		{
			return;
		}

		// Attach the current context object to the actions so that they can be retrieved by bound functions
		auto actions = qtContextMenu_->getActions();
		for (auto& action : actions)
		{
			action.first->setData(contextObject_);
		}
		qtContextMenu_->update();
	}

	bool showMenu()
	{
		if (qMenu_ == nullptr)
		{
			return false;
		}

		// Force set this attribute as Qt won't set it unless the visible attribute is set
		// which Qt won't set until after it tries to set this attribute....
		qMenu_->setAttribute(Qt::WA_OutsideWSRange, false);

		qMenu_->popup(QCursor::pos());
		return true;
	}

	QString getPath() const
	{
		return path_.c_str();
	}

	void setPath(const QString& path)
	{
		path_ = path.toUtf8().data();
		emit self_.pathChanged();

		destroyMenu();
		createMenu();
		prepareMenu();
	}

	QVariant getContextObject() const
	{
		return contextObject_;
	}

	void setContextObject(const QVariant& object)
	{
		contextObject_ = object;
		emit self_.contextObjectChanged();

		prepareMenu();
	}

private:
	WGContextMenuSimple& self_;
	QQuickWidget* view_;
	std::string windowId_;

	std::unique_ptr<QtContextMenu> qtContextMenu_;
	std::unique_ptr<QMenu> qMenu_;
	QtConnectionHolder connections_;
	std::string path_;
	QVariant contextObject_;
};

WGContextMenuSimple::WGContextMenuSimple(QQuickItem* parent) : QQuickItem(parent)
{
	impl_.reset(new Implementation(*this));
}

WGContextMenuSimple::~WGContextMenuSimple()
{
}

void WGContextMenuSimple::componentComplete()
{
	// Derived classes should call the base class method before adding their
	// own actions to perform at componentComplete.
	QQuickItem::componentComplete();
	impl_->onComponentComplete(this);
}

void WGContextMenuSimple::popup()
{
	if (impl_->showMenu())
	{
		emit opened();
	}
}

QString WGContextMenuSimple::getPath() const
{
	return impl_->getPath();
}

void WGContextMenuSimple::setPath(const QString& path)
{
	impl_->setPath(path);
}

QVariant WGContextMenuSimple::getContextObject() const
{
	return impl_->getContextObject();
}

void WGContextMenuSimple::setContextObject(const QVariant& object)
{
	impl_->setContextObject(object);
}
} // end namespace wgt
