#ifndef I_QT_FRAMEWORK_HPP
#define I_QT_FRAMEWORK_HPP

#include "core_dependency_system/i_interface.hpp"
#include "core_ui_framework/i_ui_framework.hpp"

class QQmlComponent;
class QQmlEngine;
class QVariant;
class QWidget;
class QObject;

namespace wgt
{
class IQtTypeConverter;
class QtGlobalSettings;
class QtPalette;
class Variant;

class IQtFramework : public Implements< IUIFramework >
{
public:
	virtual ~IQtFramework() {}

	virtual QQmlEngine * qmlEngine() const = 0;
	virtual void setIncubationTime( int msecs ) = 0;
	virtual void incubate() = 0;
	virtual const QtPalette * palette() const = 0;
	virtual QtGlobalSettings * qtGlobalSettings() const = 0;
	virtual void registerTypeConverter( IQtTypeConverter & converter ) = 0;
	virtual bool registerResourceData( const unsigned char * qrc_struct, const unsigned char * qrc_name,
		const unsigned char * qrc_data ) = 0;
	virtual void deregisterTypeConverter( IQtTypeConverter & converter ) = 0;
	virtual QVariant toQVariant( const Variant & variant, QObject* parent = nullptr ) const = 0;
	virtual Variant toVariant( const QVariant & qVariant ) const = 0;

	// TODO: Move into a ui adaption system
	virtual QQmlComponent * toQmlComponent( IComponent & component ) = 0;
	virtual QWidget * toQWidget( IView & view ) = 0;
	virtual void retainQWidget( IView & view ) = 0;
};
} // end namespace wgt
#endif//I_QT_FRAMEWORK
