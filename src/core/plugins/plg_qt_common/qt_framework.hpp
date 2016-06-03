#ifndef QT_FRAMEWORK_HPP
#define QT_FRAMEWORK_HPP

#include "core_dependency_system/i_interface.hpp"
#include "core_qt_common/i_qt_framework.hpp"
#include "core_qt_common/qt_action_manager.hpp"
#include "core_script/type_converter_queue.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_dependency_system/di_ref.hpp"
#include <tuple>

class QUrl;
class QQmlComponent;
class QString;

namespace wgt
{
class IQtTypeConverter;
class QtScriptingEngine;
class IComponentContext;
class QtDefaultSpacing;
class QtGlobalSettings;
class QmlWindow;
class QtWindow;
class QtPreferences;
class ICommandManager;
class QmlComponent;

namespace QtFramework_Locals
{
	class QtCommandEventListener;
}

class QtFramework
	: public Implements< IQtFramework >
{
public:
	typedef std::tuple< const unsigned char *, const unsigned char *, const unsigned	char * > ResourceData;

	QtFramework( IComponentContext & contextManager );
	virtual ~QtFramework();

	void initialise( IComponentContext & contextManager );
	void finalise();

	// IQtFramework
	QQmlEngine * qmlEngine() const override;
	const QtPalette * palette() const override;
	QtGlobalSettings * qtGlobalSettings() const override;
	void addImportPath( const QString& path );

	void registerTypeConverter( IQtTypeConverter & converter ) override;
	bool registerResourceData( const unsigned char * qrc_struct, const unsigned char * qrc_name,
		const unsigned char * qrc_data ) override;
	void deregisterTypeConverter( IQtTypeConverter & converter ) override;
	QVariant toQVariant( const Variant & variant, QObject* parent ) const override;
	Variant toVariant( const QVariant & qVariant ) const override;

	QQmlComponent * toQmlComponent( IComponent & component ) override;
	QWidget * toQWidget( IView & view ) override;
	void retainQWidget( IView & view ) override;

	std::unique_ptr< IAction > createAction(
		const char * id, std::function<void( IAction* )> func, 
		std::function<bool( const IAction* )> enableFunc, 
		std::function<bool( const IAction* )> checkedFunc ) override;
	std::unique_ptr< IComponent > createComponent( 
		const char * resource, ResourceType type ) override;

	std::unique_ptr< IView > createView(
		const char * resource, ResourceType type,
		const ObjectHandle & context ) override; 
	std::unique_ptr< IView > createView(const char* uniqueName,
		const char * resource, ResourceType type,
		const ObjectHandle & context ) override;

	void createViewAsync( 
		const char* uniqueName,
		const char * resource, ResourceType type,
		const ObjectHandle & context,
		std::function< void(std::unique_ptr< IView > &) > loadedHandler ) override;

	std::unique_ptr< IWindow > createWindow( 
		const char * resource, ResourceType type,
		const ObjectHandle & context ) override;

	void loadActionData( const char * resource, ResourceType type ) override;
	void registerComponent( const char * id, IComponent & component ) override;
	void registerComponentProvider( IComponentProvider & provider ) override;
	IComponent * findComponent( const TypeId & typeId, 
		std::function< bool ( size_t ) > & predicate ) const override;

	virtual void setPluginPath( const std::string& path ) override;
	virtual const std::string& getPluginPath() const override;

	int displayMessageBox( const char* title, const char* message, int buttons ) override;

	IPreferences * getPreferences() override;

protected:
	virtual QmlWindow * createQmlWindow();
	virtual QtWindow * createQtWindow( QIODevice & source );

private:
	QmlComponent * createComponent( const QUrl & resource );
	void createViewInternal(
		const char* uniqueName,
		const char * resource, ResourceType type,
		const ObjectHandle & context,
		std::function< void(std::unique_ptr< IView > &) > loadedHandler, bool async );

	void registerDefaultComponents();
	void registerDefaultComponentProviders();
	void registerDefaultTypeConverters();
	void unregisterResources();

	std::unique_ptr< QQmlEngine > qmlEngine_;
	std::unique_ptr< QtScriptingEngine > scriptingEngine_;
	std::unique_ptr< QtPalette > palette_;
	std::unique_ptr< QtDefaultSpacing > defaultQmlSpacing_;
	std::unique_ptr< QtGlobalSettings > globalQmlSettings_;
	std::unique_ptr< QtPreferences > preferences_;
	std::vector< std::unique_ptr< IComponent > > defaultComponents_;
	std::vector< std::unique_ptr< IComponentProvider > > defaultComponentProviders_;
	std::vector< std::unique_ptr< IQtTypeConverter > > defaultTypeConverters_;

	std::map< std::string, IComponent * > components_;
	std::vector< IComponentProvider * > componentProviders_;
	std::vector< ResourceData > registeredResources_;

	typedef TypeConverterQueue< IQtTypeConverter, QVariant > QtTypeConverters;
	QtTypeConverters typeConverters_;

	std::string pluginPath_;

	QtActionManager actionManager_;

	DIRef< ICommandManager > commandManager_;
	std::unique_ptr< QtFramework_Locals::QtCommandEventListener > commandEventListener_;
};
} // end namespace wgt
#endif
