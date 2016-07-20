#ifndef I_UI_FRAMEWORK_HPP
#define I_UI_FRAMEWORK_HPP

#include "core_reflection/object_handle.hpp"
#include "core_common/wg_future.hpp"
#include "core_common/signal.hpp"
#include <memory>

namespace wgt
{
class IAction;
class IComponent;
class IComponentProvider;
class IView;
class IWindow;
class IPreferences;

/**
 * The UI Framework interface
 */
class IUIFramework
{
	typedef Signal<void(void)> SignalVoid;
public:
    /**
     * Resource types
     */
	enum class ResourceType
	{
		Buffer, ///< Buffer resource type
		File,   ///< File resource type
		Url     ///< Url resource type
	};

	virtual ~IUIFramework() {}

	/**
     * Create an action
     *
     * @param func  Action function
     * @param enableFunc Enable function for this action
	 * @param checkedFunc checked function for this action
     * @return IAction* The created action instance
     */
	virtual std::unique_ptr< IAction > createAction(
		const char * id, std::function<void( IAction* )> func, 
		std::function<bool( const IAction* )> enableFunc = [] ( const IAction* ) { return true; },
		std::function<bool( const IAction* )> checkedFunc = std::function<bool( const IAction* )>( nullptr ) ) = 0;

    /**
     * Create component
     *
     * @param resource Resource data
     * @param type The resource type
     * @return IComponent The created component instance
     */
	virtual std::unique_ptr< IComponent > createComponent( 
		const char * resource, ResourceType type ) = 0;

	/**
	 * DEPRECATED
	 */
	virtual std::unique_ptr< IView > createView(
		const char * resource, ResourceType type,
		const ObjectHandle & context = ObjectHandle()) = 0;
	virtual std::unique_ptr< IView > createView(const char* uniqueName,
		const char * resource, ResourceType type,
		const ObjectHandle & context = ObjectHandle()) = 0;
	virtual std::unique_ptr< IWindow > createWindow( 
		const char * resource, ResourceType type,
		const ObjectHandle & context = ObjectHandle() ) = 0;

	virtual void enableAsynchronousViewCreation( bool enabled ) = 0;

	virtual wg_future< std::unique_ptr<IView> > createViewAsync(
		const char * uniqueName,
		const char * resource, ResourceType type, 
		const ObjectHandle & context = ObjectHandle(),
		std::function< void( IView & ) > loadedHandler =
			[] ( IView & ){} ) = 0;
	virtual void createWindowAsync( 
		const char * resource, ResourceType type,
		const ObjectHandle & context = ObjectHandle(),
		std::function< void(std::unique_ptr< IWindow > & ) > loadedHandler =
		[] ( std::unique_ptr< IWindow > & ){} ) = 0;

	virtual void loadActionData( const char * resource, ResourceType type ) = 0;
	virtual void registerComponent( const char * id, IComponent & component ) = 0;
	virtual void registerComponentProvider( IComponentProvider & provider ) = 0;
	virtual IComponent * findComponent( const TypeId & typeId, 
		std::function< bool ( size_t ) > & predicate ) const = 0;

	virtual void setPluginPath( const std::string& path ) = 0;
	virtual const std::string& getPluginPath() const = 0; 

	virtual void showShortcutConfig() const = 0;

	enum MessageBoxButtons
	{
		Ok = 0x1,
		Cancel = 0x2,
		Save = 0x4,
		SaveAll = 0x8,
		Yes = 0x10,
		No = 0x20,
	};

	virtual int displayMessageBox( const char* title, const char* message, int buttons ) = 0;

	virtual void registerQmlType( ObjectHandle type ) = 0;
	virtual IPreferences * getPreferences() = 0;
	virtual void doOnUIThread( std::function< void() > ) = 0;

	SignalVoid signalKeyBindingsChanged;
};
} // end namespace wgt
#endif//I_UI_FRAMEWORK_HPP
