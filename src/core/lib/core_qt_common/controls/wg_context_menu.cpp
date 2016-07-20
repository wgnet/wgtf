#include "wg_context_menu.hpp"
#include "core_dependency_system/di_ref.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_logging/logging.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_ui_framework/i_action.hpp"
#include "core_ui_framework/i_menu.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/i_window.hpp"
#include "core_qt_common/helpers/qt_helpers.hpp"
#include "core_qt_common/qt_context_menu.hpp"
#include <QMenu>
#include <QString>
#include <QVariant>
#include <QQuickWidget>
#include <QApplication>

namespace wgt
{
struct WGContextMenu::Implementation
{
	Implementation( WGContextMenu & self )
		: self_( self )
		, pContext_( nullptr )
		, view_( nullptr )
	{
	}

	~Implementation()
	{
		destroyMenu();
	}

	void onComponentComplete( WGContextMenu * contextMenu )
	{
		auto context = QQmlEngine::contextForObject( contextMenu );
		assert( context != nullptr );

		auto componentContextProperty = context->contextProperty( "componentContext" );
		assert( componentContextProperty.isValid() );
		auto componentContextVariant = QtHelpers::toVariant( componentContextProperty );
		assert( !componentContextVariant.isVoid() );
		auto componentContextHandle = componentContextVariant.value< ObjectHandle >();
		assert( componentContextHandle != nullptr );
		auto componentContext = componentContextHandle.getBase< IComponentContext >();
		assert( componentContext != nullptr );
		pContext_ = componentContext;

		auto viewProperty = context->contextProperty( "View" );
		if (viewProperty.isValid())
		{
			view_ = qvariant_cast<QQuickWidget *>( viewProperty );
		}

		auto windowIdProperty = context->contextProperty( "windowId" );
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
		if(pContext_ == nullptr)
		{
			return;
		}
		auto uiApplication = pContext_->queryInterface<IUIApplication>();
		if (uiApplication == nullptr)
		{
			return;
		}

		assert( qMenu_ == nullptr );

		qMenu_.reset( new QMenu() );
		qMenu_->setProperty( "path", QString( path_.c_str() ) );
		connections_ += QObject::connect( qMenu_.get(), &QMenu::aboutToShow, [&]() { emit self_.aboutToShow(); } );
		connections_ += QObject::connect( qMenu_.get(), &QMenu::aboutToHide, [&]() { emit self_.aboutToHide(); } );
		connections_ += QObject::connect( qApp, &QApplication::paletteChanged, [&]() { qtContextMenu_->onPaletteChanged(); });

		assert( qtContextMenu_ == nullptr );
		qtContextMenu_.reset( new QtContextMenu( *qMenu_, view_, windowId_.c_str() ) );
		uiApplication->addMenu( *qtContextMenu_ );
	}

	void destroyMenu()
	{
		if(pContext_ == nullptr)
		{
			return;
		}
		auto uiApplication = pContext_->queryInterface<IUIApplication>();
		if (uiApplication == nullptr)
		{
			return;
		}

		if (qtContextMenu_ == nullptr)
		{
			return;
		}

		uiApplication->removeMenu( *qtContextMenu_ );

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
		for (auto & action : actions)
		{
			action.first->setData( contextObject_ );
		}
		qtContextMenu_->update();
	}

	bool showMenu()
	{
		if (qMenu_ == nullptr)
		{
			return false;
		}

		qMenu_->popup( QCursor::pos() );
		return true;
	}

	QString getPath() const
	{
		return path_.c_str();
	}

	void setPath( const QString& path )
	{
		path_ = path.toUtf8().data();
		emit self_.pathChanged();

		destroyMenu();
		createMenu();
		prepareMenu();
	}

	QVariant getContextObject() const
	{	
		return QtHelpers::toQVariant(contextObject_, const_cast<WGContextMenu*>(&this->self_));
	}

	void setContextObject( const QVariant& object )
	{
		contextObject_ = QtHelpers::toVariant( object );
		emit self_.contextObjectChanged();

		prepareMenu();
	}

private:
	WGContextMenu & self_;
	IComponentContext * pContext_;
	QQuickWidget * view_;
	std::string windowId_;

	std::unique_ptr< QtContextMenu > qtContextMenu_;
	std::unique_ptr< QMenu > qMenu_;
	QtConnectionHolder connections_;
	std::string path_;
	Variant contextObject_;
};

WGContextMenu::WGContextMenu( QQuickItem * parent )
	: QQuickItem( parent )
{
	impl_.reset( new Implementation( *this ) );
}

WGContextMenu::~WGContextMenu()
{
}

void WGContextMenu::componentComplete()
{
	// Derived classes should call the base class method before adding their
	// own actions to perform at componentComplete.
	QQuickItem::componentComplete();
	impl_->onComponentComplete( this );
}

void WGContextMenu::popup()
{
	if (impl_->showMenu())
	{
		emit opened();
	}
}

QString WGContextMenu::getPath() const
{
	return impl_->getPath();
}

void WGContextMenu::setPath( const QString& path )
{
	impl_->setPath( path );
}

QVariant WGContextMenu::getContextObject() const
{
	return impl_->getContextObject();
}

void WGContextMenu::setContextObject( const QVariant& object )
{
	impl_->setContextObject( object );
}
} // end namespace wgt
