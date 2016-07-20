#ifndef QT_MENU_HPP
#define QT_MENU_HPP

#include "qt_connection_holder.hpp"
#include "core_ui_framework/i_menu.hpp"
#include <string>

#include <QAction>
#include <QSharedPointer>
#include <QWeakPointer>

class QMenu;

namespace wgt
{
class Connection;
typedef std::map< IAction *, QSharedPointer< QAction > > Actions;
typedef std::map< IAction *, QWeakPointer< QAction > > SharedActions;
typedef std::map<IAction*, Connection> ActionConnections;
class QtMenu : public IMenu
{
public:
	QtMenu( QObject & menu, const char * windowId );
	const char * path() const override;
	const char * windowId() const override;

	void update() override;
	
	const char * relativePath( const char * path ) const;

	QAction * createQAction( IAction & action );
	void destroyQAction( IAction & action );
	QAction * getQAction( IAction & action );

	const Actions& getActions() const;

protected:
	static void addMenuAction( QMenu & qMenu, QAction & qAction, const char * path );
	static void removeMenuAction( QMenu & qMenu, QAction & qAction );

private:
	QSharedPointer< QAction > createSharedQAction( IAction & action );
	QSharedPointer< QAction > getSharedQAction( IAction & action );

	static SharedActions sharedQActions_;
	QObject & menu_;
	Actions actions_;
	
	std::string path_;
	std::string windowId_;
	ActionConnections connections_;
};
} // end namespace wgt
#endif//QT_MENU_BAR_HPP
