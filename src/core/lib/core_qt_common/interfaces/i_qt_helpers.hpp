#ifndef I_QT_HELPERS_HPP
#define I_QT_HELPERS_HPP

#include "core_variant/variant.hpp"

class QObject;
class QQuickItem;
class QUrl;
class QQmlEngine;
class QVariant;

namespace wgt
{
class ObjectHandle;
class TypeId;

class IQtHelpers
{
public:
	virtual QVariant toQVariant(const Variant& variant, QObject* parent) = 0;
	virtual QVariant toQVariant(const ObjectHandle& object, QObject* parent) = 0;
	virtual Variant toVariant(const QVariant& qVariant) = 0;

	virtual QQuickItem* findChildByObjectName(QObject* parent, const char* controlName) = 0;
};

} // end namespace wgt
#endif // I_QT_HELPERS_HPP
