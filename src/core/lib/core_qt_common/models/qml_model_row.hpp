#pragma once

#include "qml_model_data.hpp"
#include "core_qt_common/qt_new_handler.hpp"
#include "core_qt_common/qt_connection_holder.hpp"

#include <QObject>
#include <QVector>
#include <QQmlListProperty>
#include <QtQml>

namespace wgt
{
class QmlModelRow : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QQmlListProperty<wgt::QmlModelData> columns READ columnsProperty)
	Q_PROPERTY(QQmlListProperty<wgt::QmlModelRow> children READ childrenProperty)
	Q_CLASSINFO("DefaultProperty", "columns")

	DECLARE_QT_MEMORY_HANDLER

public:
	QmlModelRow();
	virtual ~QmlModelRow();

	const QVector<QmlModelData*>& columns() const;
	const QVector<QmlModelRow*>& children() const;
	QmlModelRow* parent() const;

signals:
	void columnAdded(int index, QmlModelData* column);
	void columnChanged(int index, QmlModelData* column);
	void columnsCleared();

	void addingChild(QmlModelRow* child);
	void childAdded(QmlModelRow* child);
	void clearingChildren(QmlModelRow* parent);
	void childrenCleared(QmlModelRow* parent);

private:
	QQmlListProperty<QmlModelData> columnsProperty() const;
	static void appendColumns(QQmlListProperty<QmlModelData>* property, QmlModelData* column);
	static int countColumns(QQmlListProperty<QmlModelData>* property);
	static QmlModelData* columnAt(QQmlListProperty<QmlModelData>* property, int index);
	static void clearColumns(QQmlListProperty<QmlModelData>* property);

	QQmlListProperty<QmlModelRow> childrenProperty() const;
	static void appendChildren(QQmlListProperty<QmlModelRow>* property, QmlModelRow* child);
	static int countChildren(QQmlListProperty<QmlModelRow>* property);
	static QmlModelRow* childAt(QQmlListProperty<QmlModelRow>* property, int index);
	static void clearChildren(QQmlListProperty<QmlModelRow>* property);

	QVector<QmlModelData*> columns_;
	QVector<QmlModelRow*> children_;
	QtConnectionHolder columnConnections_;
	QtConnectionHolder rowConnections_;
	QmlModelRow* parent_;
};
} // end namespace wgt

QML_DECLARE_TYPE(wgt::QmlModelRow)
