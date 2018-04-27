#pragma once

#include "core_qt_common/qt_new_handler.hpp"
#include <QQuickItem>
#include <QtQml>
#include <memory>

class QString;
class QVariant;

namespace wgt
{
class IListModel;
class QtContextMenu;

class WGContextMenuSimple : public QQuickItem
{
	Q_OBJECT

	// Represents the path of the context menu actions you wish to load. Actions are loaded from
	// XML and include path data. The *.ui file will indicate what your QtContextMenu's path
	// will be and all actions with that path will be added to that menu.
	Q_PROPERTY(QString path READ getPath WRITE setPath NOTIFY pathChanged)

	// The object, which may be required for determining if an action should be enabled, checked, or
	// passed in to the handler function.
	Q_PROPERTY(QVariant contextObject READ getContextObject WRITE setContextObject NOTIFY contextObjectChanged)

	DECLARE_QT_MEMORY_HANDLER

protected:
	void componentComplete();

public:
	WGContextMenuSimple(QQuickItem* parent = NULL);
	virtual ~WGContextMenuSimple();

	QString getPath() const;
	void setPath(const QString& path);

	QVariant getContextObject() const;
	void setContextObject(const QVariant& object);

	// QML invokable function to locate the menu based on the provided parameters (windowId and path) and
	// display it to the end-user.
	Q_INVOKABLE void popup();

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

QML_DECLARE_TYPE(wgt::WGContextMenuSimple)
