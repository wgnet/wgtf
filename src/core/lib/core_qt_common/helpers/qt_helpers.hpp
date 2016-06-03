#ifndef QT_HELPERS_HPP
#define QT_HELPERS_HPP

#include <string>
#include "core_variant/variant.hpp"
#include <QVariant>

class QObject;
class QQuickItem;
class QUrl;
class QQmlEngine;

namespace wgt
{
class ObjectHandle;
class TypeId;

namespace QtHelpers
{
	QVariant toQVariant( const Variant & variant, QObject* parent );
	QVariant toQVariant( const ObjectHandle & object, QObject* parent );
	Variant toVariant( const QVariant & qVariant );

	QVariant convertToQTTypeInstance( const TypeId & type, const void * data );
	QQuickItem * findChildByObjectName(
		QObject * parent, const char * controlName );

	QUrl resolveQmlPath( const QQmlEngine & qmlEngine, const char * relativePath );
};
} // end namespace wgt
#endif //QT_HELPERS_HPP
