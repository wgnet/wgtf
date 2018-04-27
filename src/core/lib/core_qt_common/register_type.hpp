#ifndef REGISTER_TYPE_HPP
#define REGISTER_TYPE_HPP

#include <qqml.h>
#include <core_ui_framework/i_ui_framework.hpp>

namespace wgt
{
template <typename T>
QQmlPrivate::RegisterType TryRegisterType(const char* uri, int versionMajor, int versionMinor, const char* qmlName,
                                          int metaObjectRevision)
{
	QML_GETTYPENAMES

	QQmlPrivate::RegisterType type = { metaObjectRevision == 0 ? 0 : 1,

		                               qRegisterNormalizedMetaType<T*>(pointerName.constData()),
		                               qRegisterNormalizedMetaType<QQmlListProperty<T>>(listName.constData()),
		                               sizeof(T),
		                               QQmlPrivate::createInto<T>,
		                               QString(),

		                               uri,
		                               versionMajor,
		                               versionMinor,
		                               qmlName,
		                               &T::staticMetaObject,

		                               QQmlPrivate::attachedPropertiesFunc<T>(),
		                               QQmlPrivate::attachedPropertiesMetaObject<T>(),

		                               QQmlPrivate::StaticCastSelector<T, QQmlParserStatus>::cast(),
		                               QQmlPrivate::StaticCastSelector<T, QQmlPropertyValueSource>::cast(),
		                               QQmlPrivate::StaticCastSelector<T, QQmlPropertyValueInterceptor>::cast(),

		                               0,
		                               0,

		                               0,
		                               metaObjectRevision };

	return type;
}

template <typename T>
void registerType(IUIFramework& framework, const char* uri, int versionMajor, int versionMinor, const char* qmlName,
                  int metaObjectRevision = 0)
{
	auto type = TryRegisterType<T>(uri, versionMajor, versionMinor, qmlName, metaObjectRevision);
	ObjectHandleT<QQmlPrivate::RegisterType> handleToType(type);
	framework.registerQmlType(handleToType);
}
}

#endif // REGISTER_TYPE_HPP