#ifndef WG_CONTEXT_MENU_HPP
#define WG_CONTEXT_MENU_HPP

#include "../qt_new_handler.hpp"
#include <QQuickItem>
#include <QtQml>
#include <memory>

class QString;
class QVariant;

namespace wgt
{
class IListModel;
class QtContextMenu;

class WGContextMenu : public QQuickItem
{
	Q_OBJECT

	// Represents the path of the context menu actions you wish to load. Actions are loaded from
	// XML and include path data. The *.ui file will indicate what your QtContextMenu's path
	// will be and all actions with that path will be added to that menu.
	Q_PROPERTY(QString path READ getPath WRITE setPath NOTIFY pathChanged)

	// The objects, which may be required for determining if an action should be enabled, checked, or
	// passed in to the handler function.
	// May be QVariantList of objects (for multiple selection) or a single object (for a single selection).
	Q_PROPERTY(QVariant contextObject READ getContextObject WRITE setContextObject NOTIFY contextObjectChanged)

	DECLARE_QT_MEMORY_HANDLER

protected:
	void componentComplete();

public:
	WGContextMenu(QQuickItem* parent = NULL);
	virtual ~WGContextMenu();

	QString getPath() const;
	void setPath(const QString& path);

	QVariant getContextObject() const;
	void setContextObject(const QVariant& selection);

	// QML invokable function to locate the menu based on the provided parameters (windowId and path) and
	// display it to the end-user.
	Q_INVOKABLE void popup();

	// Determines whether or not the action identified by the actionId is in this menu
	Q_INVOKABLE QObject* find(const QString& actionId) const;

signals:
	void pathChanged();
	void contextObjectChanged();

	void aboutToShow();
	void aboutToHide();
	// Signal: onOpened
	// Emitted when popup() is complete. May be handled in QML instances of WGContextMenu.
	void opened();

private:
	QtContextMenu* findMenu();

	struct Implementation;
	std::unique_ptr<Implementation> impl_;
};
} // end namespace wgt

QML_DECLARE_TYPE(wgt::WGContextMenu)
#endif // WG_CONTEXT_MENU_HPP
