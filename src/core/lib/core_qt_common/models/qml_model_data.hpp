#pragma once

#include "core_qt_common/qt_new_handler.hpp"

#include <QObject>
#include <QMap>
#include <QVariant>
#include <QString>
#include <QtQml>

namespace wgt
{
class QmlModelData : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QVariantMap roles READ roles WRITE setRoles NOTIFY rolesChanged)

	DECLARE_QT_MEMORY_HANDLER

public:
	QmlModelData();
	virtual ~QmlModelData();

	QVariantMap& roles();

signals:
	void rolesChanged();

private:
	void setRoles(const QVariantMap& roles);

	QVariantMap roles_;
};
} // end namespace wgt

QML_DECLARE_TYPE(wgt::QmlModelData)
