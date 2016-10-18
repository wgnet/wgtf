#include "wg_action.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_ui_framework/i_action.hpp"
#include "helpers/qt_helpers.hpp"

#include <QQmlEngine>
#include <QQmlContext>

namespace wgt
{
struct WGAction::Implementation
{
	Implementation(WGAction* self)
		: uiFramework_( nullptr )
        , uiApplication_( nullptr )
        , func_( nullptr )
		, enabledFunc_( nullptr )
		, checkedFunc_( nullptr )
		, checkable_( false )
		, checked_( false )
		, enabled_( true )
		, visible_( true )
		, active_( false )
		, self_(self)
	{
	}

	~Implementation()
	{
		destroyAction();
	}

	void onComponentComplete( WGAction * action )
	{
		func_ = [action](IAction*){ emit action->triggered(); };
		enabledFunc_ = [action](const IAction*){ return action->getEnabled(); };
		checkedFunc_ = [action](const IAction*){ return action->getChecked(); };

		auto context = QQmlEngine::contextForObject( action );
		assert( context != nullptr );

		auto componentContextProperty = context->contextProperty( "componentContext" );
		assert( componentContextProperty.isValid() );
		auto componentContextVariant = QtHelpers::toVariant( componentContextProperty );
		assert( !componentContextVariant.isVoid() );
		auto componentContext = componentContextVariant.value<IComponentContext*>();
		assert( componentContext != nullptr );

		uiApplication_ = componentContext->queryInterface< IUIApplication >();
		uiFramework_ = componentContext->queryInterface< IUIFramework >();

		createAction();
	}

	bool getActive() const
	{
		return active_;
	}

	void setActive( bool active )
	{
		if (active_ == active)
		{
			return;
		}

		active_ = active;

		if (uiApplication_ == nullptr || action_ == nullptr || visible_ == false)
		{
			return;
		}

		active_ ? uiApplication_->addAction( *action_ ) : uiApplication_->removeAction( *action_ );
	}

	QString getActionId() const
	{
		return actionId_.c_str();
	}

	void setActionId( const QString& actionId )
	{
		destroyAction();
		actionId_ = actionId.toUtf8().data();
		createAction();
	}

	bool getCheckable() const
	{
		return checkable_;
	}

	void setCheckable( bool checkable )
	{
		destroyAction();
		checkable_ = checkable;
		createAction();
	}

	bool getChecked() const
	{
		return checked_;
	}

	void setChecked( bool checked )
	{
		checked_ = checked;
	}

	bool getEnabled() const
	{
		return enabled_;
	}

	void setEnabled( bool enabled )
	{
		enabled_ = enabled;
	}

	bool getVisible() const
	{
		return visible_;
	}

	void setVisible( bool visible )
	{
		if (visible_ == visible)
		{
			return;
		}

		visible_ = visible;

		if (uiApplication_ == nullptr || action_ == nullptr || active_ == false)
		{
			return;
		}

		visible_ ? uiApplication_->addAction( *action_ ) : uiApplication_->removeAction( *action_ );
	}

    QVariant data()
    {
		if (action_ && self_)
		{
			auto qdata = QtHelpers::toQVariant( action_->getData(), this->self_ );
			return qdata;
		}

		return QVariant();
    }

	void setData( const QVariant& qdata )
	{
		if (action_)
		{
			auto data = QtHelpers::toVariant( qdata );
			action_->setData( data );
			emit self_->dataChanged();
		}
    }

	void createAction()
	{
		if (actionId_.empty())
		{
			return;
		}

		if (uiFramework_ == nullptr)
		{
			return;
		}

		action_ = uiFramework_->createAction( actionId_.c_str(), func_, enabledFunc_, checkable_ ? checkedFunc_ : nullptr );
		
		if (uiApplication_ == nullptr || active_ == false || visible_ == false)
		{
			return;
		}

		uiApplication_->addAction( *action_ );
	}

	void destroyAction()
	{
		if (action_ == nullptr)
		{
			return;
		}

		if (uiApplication_ != nullptr && active_ == true && visible_ == true)
		{
			uiApplication_->removeAction( *action_ );
		}

		action_.reset();
	}

    WGAction* self_;
	IUIFramework * uiFramework_;
	IUIApplication * uiApplication_;
	std::function<void(IAction*)> func_;
	std::function<bool(const IAction*)> enabledFunc_;
	std::function<bool(const IAction*)> checkedFunc_;
	bool checkable_;
	bool checked_;
	bool enabled_;
	bool visible_;
	bool active_;
	std::string actionId_;
	std::unique_ptr< IAction > action_;
};

WGAction::WGAction( QQuickItem * parent )
	: QQuickItem( parent )
{
	impl_.reset( new Implementation(this) );
}


WGAction::~WGAction()
{
}


void WGAction::componentComplete()
{
	QQuickItem::componentComplete();
	impl_->onComponentComplete( this );
}


bool WGAction::getActive() const
{
	return impl_->getActive();
}


void WGAction::setActive( bool active )
{
	impl_->setActive( active );
}


QString WGAction::getActionId() const
{
	return impl_->getActionId();
}


void WGAction::setActionId( const QString& actionId )
{
	impl_->setActionId( actionId );
}


bool WGAction::getCheckable() const
{
	return impl_->getCheckable();
}


void WGAction::setCheckable( bool checkable )
{
	impl_->setCheckable( checkable );
}


bool WGAction::getChecked() const
{
	return impl_->getChecked();
}


void WGAction::setChecked( bool checked )
{
	impl_->setChecked( checked );
}


bool WGAction::getEnabled() const
{
	return impl_->getEnabled();
}


void WGAction::setEnabled( bool enabled )
{
	impl_->setEnabled( enabled );
}

bool WGAction::getVisible() const
{
	return impl_->getVisible();
}

QVariant WGAction::data() const
{
	return impl_->data();
}

void WGAction::setData(const QVariant& userData)
{
	impl_->setData(userData);
}

void WGAction::setVisible( bool visible )
{
	impl_->setVisible( visible );
}
} // end namespace wgt
