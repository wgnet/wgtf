#include "qt_menu.hpp"
#include "core_ui_framework/i_action.hpp"
#include "core_logging/logging.hpp"
#include "core_common/signal.hpp"
#include <QApplication>
#include <QAction>
#include <QObject>
#include <QString>
#include <QMenu>

#include <cassert>

namespace wgt
{
namespace QtMenu_Locals
{
	const char * modeDisabled = "_disabled";
	const char * modeActive = "_active";
	const char * modeSelected = "_selected";
	const char * stateOn = "_checked";

	QIcon generateIcon( IAction & action )
	{
		QIcon icon;

		QString normalPath( action.icon() );
		QString disabledPath( normalPath + modeDisabled );
		QString activePath( normalPath + modeActive );
		QString selectedPath( normalPath + modeSelected );
		QString normalOnPath( normalPath + stateOn );
		QString disabledOnPath( normalOnPath + modeDisabled );
		QString activeOnPath( normalOnPath + modeActive );
		QString selectedOnPath( normalOnPath + modeSelected );

		icon.addPixmap( QPixmap( normalPath ), QIcon::Normal, QIcon::Off );
		icon.addPixmap( QPixmap( disabledPath ), QIcon::Disabled, QIcon::Off );
		icon.addPixmap( QPixmap( activePath ), QIcon::Active, QIcon::Off );
		icon.addPixmap( QPixmap( selectedPath ), QIcon::Selected, QIcon::Off );
		icon.addPixmap( QPixmap( normalOnPath ), QIcon::Normal, QIcon::On );
		icon.addPixmap( QPixmap( disabledOnPath ), QIcon::Disabled, QIcon::On );
		icon.addPixmap( QPixmap( activeOnPath ), QIcon::Active, QIcon::On );
		icon.addPixmap( QPixmap( selectedOnPath ), QIcon::Selected, QIcon::On );

		return icon;
	}
}

SharedActions QtMenu::sharedQActions_;

QtMenu::QtMenu( QObject & menu, const char * windowId )
	: menu_( menu )
	, windowId_( windowId )
{
	auto pathProperty = menu_.property( "path" );
	if (pathProperty.isValid())
	{
		path_ = pathProperty.toString().toUtf8().operator const char *();
	}

	QMenu* qMenu = dynamic_cast<QMenu*>(&menu_);
	if(qMenu)
	{
		QObject::connect(qMenu, &QMenu::aboutToShow, [=](){ update(); });
	}
}

const char * QtMenu::path() const
{
	return path_.c_str();
}

const char * QtMenu::windowId() const
{
	return windowId_.c_str();
}

void QtMenu::update()
{
	for (auto & action : actions_)
	{
		action.second->setEnabled( action.first->enabled() );
		if (action.second->isCheckable())
		{
			action.second->setChecked(action.second->actionGroup() ?
			                          action.second->actionGroup()->checkedAction() == action.second.data() :
			                          action.first->checked());
		}
	}
}

const char * QtMenu::relativePath( const char * path ) const
{
	auto menuPath = this->path();
	if (path == nullptr || menuPath == nullptr)
	{
		return nullptr;
	}

	auto menuPathLen = strlen( menuPath );
	if (menuPathLen == 0)
	{
		return path;
	}

	//Action paths come in the format -
	//	minimum.path.to.match|.optional.path.to.match
	//We need to be able to match the action path -
	//	a.b.c|.d.e.f
	//To the menu path -
	//	a.b.c.d.e
	//To do this we split the action path into its minimum and optional components
	//and compare them to the appropriate subpaths of the menu path

	auto chr = strchr( path, '|' );
	auto minPath = chr != nullptr ? path : nullptr;
	auto minPathLen = chr != nullptr ? static_cast< size_t >( chr - path ) : 0;
	auto optPath = chr != nullptr ? chr + 1 : path;
	auto optPathLen = strlen( optPath );

	if (minPathLen > menuPathLen ||
		strncmp( minPath, menuPath, minPathLen ) != 0)
	{
		return nullptr;
	}

	if (minPathLen + optPathLen < menuPathLen ||
		strncmp( optPath, menuPath + minPathLen, menuPathLen - minPathLen ) != 0)
	{
		return nullptr;
	}

	optPath += menuPathLen - minPathLen;

	if (optPath[0] == '\0')
	{
		return optPath;
	}

	if (optPath[0] == '.')
	{
		return ++optPath;
	}

	return nullptr;
}

QAction * QtMenu::createQAction( IAction & action )
{
	auto qAction = getQAction( action );
	if (qAction != nullptr)
	{
		NGT_WARNING_MSG("Action %s already existing.\n", action.text());
		return nullptr;
	}

	actions_[&action] = createSharedQAction(action);
	qAction = getQAction( action );
	assert( qAction != nullptr );

	return qAction;
}

void QtMenu::destroyQAction( IAction & action )
{
	auto it = actions_.find( &action );
	if (it != actions_.end())
	{
		const std::string groupID(action.group());
		if (!groupID.empty())
		{
			auto groupItr = groups_.find(groupID);
			if (groupItr != groups_.end())
			{
				groupItr->second->removeAction(it->second.data());
				if (groupItr->second->actions().empty())
				{
					groups_.erase(groupItr);
				}
			}
		}
		actions_.erase( it );
	}

	auto findIt = connections_.find( &action );
	if (findIt != connections_.end())
	{
		 connections_.erase( findIt );
	}
}

QAction * QtMenu::getQAction( IAction & action )
{
	auto it = actions_.find( &action );
	if (it != actions_.end())
	{
		return it->second.data();
	}
	return nullptr;
}

const Actions& QtMenu::getActions() const
{
	return actions_;
}

void QtMenu::addMenuAction( QMenu & qMenu, QAction & qAction, const char * path )
{
	QMenu * menu = &qMenu;
	while (path != nullptr)
	{
		auto tok = strchr( path, '.' );
		auto subPath = tok != nullptr ? QString::fromUtf8( path, tok - path ) : path;
		if (!subPath.isEmpty())
		{
			QMenu * subMenu = menu->findChild<QMenu*>( subPath, Qt::FindDirectChildrenOnly );

			if (subMenu == nullptr)
			{
				subMenu = menu->addMenu( subPath );
				subMenu->setObjectName( subPath );
			}
			menu = subMenu;
		}
		path = tok != nullptr ? tok + 1 : nullptr;
	}

	assert( menu != nullptr );
	auto order = qAction.property( "order" ).toInt();

	auto actions = menu->actions();
	auto it = std::find_if( actions.begin(), actions.end(), 
		[&](QAction * action) { return action->property( "order" ).toInt() > order; } );
	menu->insertAction( it == actions.end() ? nullptr : *it, &qAction );
}

void QtMenu::removeMenuAction( QMenu & qMenu, QAction & qAction )
{
	qMenu.removeAction( &qAction );
	auto children = qMenu.findChildren<QMenu*>( QString(), Qt::FindDirectChildrenOnly );
	for (auto & child : children)
	{
		removeMenuAction( *child, qAction );
		if (child->isEmpty())
		{
            child->deleteLater();
		}
	}
}

QSharedPointer< QAction > QtMenu::createSharedQAction( IAction & action )
{
	auto qAction = getSharedQAction( action );
	if( qAction )
	{
		return qAction;
	}

	qAction.reset( new QAction( action.text(), QApplication::instance() ), &QObject::deleteLater );
	sharedQActions_[&action] = qAction;
	qAction->setProperty("order", action.order());
	if (action.isSeparator())
	{
		qAction->setSeparator(true);
	}
	else
	{
		qAction->setIcon(QtMenu_Locals::generateIcon(action));
		qAction->setShortcut(QKeySequence(action.shortcut()));
		qAction->setEnabled(action.enabled());
		if (action.isCheckable())
		{
			qAction->setCheckable(true);
			qAction->setChecked(action.checked());
		}

		QObject::connect(qAction.data(), &QAction::triggered,
		                 [&action]()
		                 {
			                 if (!action.enabled())
			                 {
				                 return;
			                 }
			                 action.execute();
			             });

		connections_[&action] = action.signalShortcutChanged.connect(
		[qAction](const char* shortcut)
		{
			assert(qAction != nullptr);
			qAction->setShortcut(QKeySequence(shortcut));
		});
		const std::string groupID(action.group());
		if (!groupID.empty())
		{
			auto itr = groups_.find(groupID);
			if (itr == groups_.end())
			{
				groups_[groupID].reset(new QActionGroup(&menu_));
			}

			groups_.at(groupID)->addAction(qAction.data());
			assert(qAction->actionGroup());
		}
	}

	return qAction;
}

QSharedPointer< QAction > QtMenu::getSharedQAction( IAction & action )
{
	auto it = sharedQActions_.find( &action );
	if ( it != sharedQActions_.end() )
	{
		return it->second.lock();
	}
	return QSharedPointer< QAction >();
}
} // end namespace wgt
