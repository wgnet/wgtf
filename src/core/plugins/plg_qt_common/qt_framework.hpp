#ifndef QT_FRAMEWORK_HPP
#define QT_FRAMEWORK_HPP

#include "core_dependency_system/i_interface.hpp"
#include "core_qt_common/i_qt_framework.hpp"
#include "core_script/type_converter_queue.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_ui_framework/interfaces/i_view_creator.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_dependency_system/depends.hpp"
#include "core_command_system/i_command_manager.hpp"
#include <tuple>

class QUrl;
struct QMetaObject;
class QQmlComponent;
class QString;
class QIODevice;
class QQmlIncubationController;

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
class ActionManager;

namespace QtFramework_Locals
{
	class QtCommandEventListener;
}

class QtFramework
	: public Implements< IQtFramework >
	, public Depends< ICommandManager, IUIApplication, IViewCreator >
{
public:
	typedef std::tuple< const unsigned char *, const unsigned char *, const unsigned	char * > ResourceData;

	QtFramework( IComponentContext & contextManager );
	virtual ~QtFramework();

	void initialise( IComponentContext & contextManager );
	void finalise();

	// IQtFramework
	QQmlEngine * qmlEngine() const override;
	virtual void setIncubationTime( int msecs ) override;
	virtual void incubate() override;
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
	std::unique_ptr< IWindow > createWindow( 
		const char * resource, ResourceType type,
		const ObjectHandle & context ) override;

	wg_future< std::unique_ptr<IView> > createViewAsync( 
		const char* uniqueName,
		const char * resource, ResourceType type,
		const ObjectHandle & context,
		std::function< void( IView & ) > loadedHandler ) override;
	void createWindowAsync( 
		const char * resource, ResourceType type,
		const ObjectHandle & context,
		std::function< void(std::unique_ptr< IWindow > & ) > loadedHandler ) override;

	void enableAsynchronousViewCreation( bool enabled ) override;
	void loadActionData( const char * resource, ResourceType type ) override;
	void registerComponent( const char * id, IComponent & component ) override;
	void registerComponentProvider( IComponentProvider & provider ) override;
	IComponent * findComponent( const TypeId & typeId, 
		std::function< bool ( size_t ) > & predicate ) const override;

	virtual void setPluginPath( const std::string& path ) override;
	virtual const std::string& getPluginPath() const override;

	int displayMessageBox( const char* title, const char* message, int buttons ) override;

	IPreferences * getPreferences() override;
    void doOnUIThread( std::function< void() > ) override;
    


	virtual void registerQmlType( ObjectHandle type ) override;

	virtual void showShortcutConfig() const override;

protected:
	virtual QmlWindow * createQmlWindow();
	virtual QtWindow * createQtWindow( QIODevice & source );

private:
	QmlComponent * createComponent( const QUrl & resource );
    wg_future<std::unique_ptr<IView>> createViewInternal(
        const char* uniqueName,
        const char * resource, ResourceType type,
        const ObjectHandle & context,
        std::function< void(IView &) > loadedHandler, bool async );
	void createWindowInternal( 
		const char * resource, ResourceType type,
		const ObjectHandle & context, 
		std::function< void(std::unique_ptr< IWindow > &) > loadedHandler, 
		bool async );

	void registerDefaultComponents();
	void registerDefaultComponentProviders();
	void registerDefaultTypeConverters();
	void unregisterResources();
	void onApplicationStartUp();

	std::unique_ptr< QQmlEngine > qmlEngine_;
	std::unique_ptr< QtScriptingEngine > scriptingEngine_;
	std::unique_ptr< QtPalette > palette_;
	std::unique_ptr< QtDefaultSpacing > defaultQmlSpacing_;
	std::unique_ptr< QtGlobalSettings > globalQmlSettings_;
	std::vector< std::unique_ptr< IComponent > > defaultComponents_;
	std::vector< std::unique_ptr< IComponentProvider > > defaultComponentProviders_;
	std::vector< std::unique_ptr< IQtTypeConverter > > defaultTypeConverters_;

	std::map< std::string, IComponent * > components_;
	std::vector< IComponentProvider * > componentProviders_;
	std::vector< ResourceData > registeredResources_;

	QMetaObject* constructMetaObject( QMetaObject* original );
	std::vector <std::unique_ptr<QMetaObject>> registeredTypes_;

	typedef TypeConverterQueue< IQtTypeConverter, QVariant > QtTypeConverters;
	QtTypeConverters typeConverters_;

	std::string pluginPath_;

	std::unique_ptr<ActionManager> actionManager_;
	std::unique_ptr< QtFramework_Locals::QtCommandEventListener > commandEventListener_;
	IComponentContext & context_;
	std::unique_ptr< QObject > worker_;
	std::unique_ptr< IWindow > shortcutDialog_;
	ConnectionHolder connections_;
	std::atomic<int> incubationTime_;
	std::unique_ptr<QQmlIncubationController> incubationController_;
	IInterface* preferences_;

	bool useAsyncViewLoading_;
};
} // end namespace wgt
#endif
