#ifndef I_OBJECT_MANAGER_HPP
#define I_OBJECT_MANAGER_HPP

#include "reflected_object.hpp"
#include "i_definition_manager.hpp"
#include "core_dependency_system/i_interface.hpp"
#include <memory>
#include <string>

namespace wgt
{
class ObjectHandle;
class RefObjectId;
class ISerializer;
class IBaseProperty;


/**
 *	Interface for receiving notifications when objects are registered or
 *	deregistered with IObjectManager.
 */
class IObjectManagerListener
{
public:
	virtual ~IObjectManagerListener() {}

	/**
	 *	Notify that an object has been registered.
	 *	@param pObj the object that has been registered.
	 */
	virtual void onObjectRegistered( const ObjectHandle & pObj ) {}

	/**
	 *	Notify that an object has been deregistered.
	 *	@param pObj the object that has been deregistered.
	 */
	virtual void onObjectDeregistered( const ObjectHandle & pObj ) {}
};


/**
 *	This is an interface for providing registry of ObjectHandles.
 *	
 *	It can be used to request the same instance of an ObjectHandle, instead of
 *	creating a new instance each time.
 *	
 *	When a plugin unloads, the ObjectHandles it was providing can be set to
 *	null in any other plugins that were using the data.
 *	E.g. if plg_my_panel shares an object with plg_command_manager when a field
 *		in the panel is being edited.
 *		If plg_my_panel unloads, then the object in plg_command_manager is set
 *		to null.
 *		If plg_my_panel reloads, then the object in plg_command_manager is
 *		reset.
 *	
 *	"Managed" objects - IObjectManager controls deletion.
 *	"Unmanaged" objects - owner controls deletion.
 */
class IObjectManager
{
public:
	/**
	 *	Get an existing object or create a new one by an ID.
	 *	Object will be managed by IObjectManager.
	 *	@param id the ID of the object to look up or create.
	 *	@param classDef the definition to look up or create.
	 *	@return existing or new object.
	 */
	virtual ObjectHandle createObject( 
		const RefObjectId & id,
		const std::string & classDef ) const = 0;

	/**
	 *	Create a new object.
	 *	Object will be managed by IObjectManager.
	 *	@param classDef the definition to look up or create.
	 *	@return new object.
	 */
	virtual ObjectHandle create( 
		const std::string & classDef ) const = 0;

	/**
	 *	Get an existing object by an ID.
	 *	Object must be managed by IObjectManager.
	 *	@param id the ID of the object to look up.
	 *	@return found object or null.
	 */
	virtual ObjectHandle getObject( const RefObjectId & id ) const = 0;

	/**
	 *	Get an existing, managed object by a pointer to another object.
	 *	Convert your unregistered object to one registered with IObjectManager.
	 *	@param pObj pointer to object to be found.
	 *	@return found object or null.
	 */
	virtual ObjectHandle getObject( const void * pObj ) const = 0;

	/**
	 *	Get an existing, unmanaged object by a pointer to another object.
	 *	Convert your unregistered object to one registered with IObjectManager.
	 *	@param pObj pointer to object to be found.
	 *	@return found object or null.
	 */
	virtual ObjectHandle getUnmanagedObject( const void * pObj ) const = 0;

	/**
	 *	Get the ID of an existing, unmanaged object by a pointer to another object.
	 *	@param pObj pointer to object to be found.
	 *	@param id output for found object ID.
	 *	@return true if the ID was output.
	 */
	virtual bool getUnmanagedObjectId( const void * pObj, RefObjectId & id ) const = 0;

	/**
	 *	Get a list of all of the managed object IDs that are registered for
	 *	the given definition context.
	 *	@param context the definition context for which to find all of the
	 *		objects that have been registered.
	 *	@param o_objects found object IDs are output here.
	 *	@return true if o_objects were output.
	 *		False if the given context could not be found.
	 */
	virtual bool getContextObjects( IDefinitionManager * context,
		std::vector< RefObjectId > & o_objects ) const = 0;

	/**
	 *	Get a list of all of the managed object IDs that are registered.
	 *	@param o_objects found object IDs are output here.
	 */
	virtual void getObjects( std::vector< ObjectHandle > & o_objects ) const = 0;

	/**
	 *	Register a managed object.
	 *	@param pObj the object to register.
	 *	@param id the ID of the object to register.
	 *		Or zero to generate a new ID.
	 *	@return the registered object.
	 */
	virtual ObjectHandle registerObject(
		ObjectHandle & pObj, 
		const RefObjectId & id = RefObjectId::zero() ) = 0;

	/**
	 *	Register an unmanaged object.
	 *	@param pObj the object to register.
	 *	@param id the ID of the object to register.
	 *		Or zero to generate a new ID.
	 *	@return the registered object.
	 */
	virtual RefObjectId registerUnmanagedObject(
		const ObjectHandle & object, 
		const RefObjectId& id = RefObjectId::zero() ) = 0;
	
	/**
	 *	Register a definition context.
	 *	@param context the definition context to be registered.
	 *	@return true if the definition context was registered,
	 *		false if it was already registered.
	 */
	virtual bool registerContext( IDefinitionManager * context ) = 0;

	/**
	 *	Deregister a definition context.
	 *	@param context the definition context to be deregistered.
	 *	@return true if the definition context was deregistered,
	 *		false if it was not found (not registered in the first place).
	 */
	virtual bool deregisterContext( IDefinitionManager * context ) = 0;

	/**
	 *	Register a listener that is notified when objects are registered or
	 *	deregistered.
	 *	
	 *	@pre listener must not already be registered.
	 *	
	 *	@param listener the listener to be registered.
	 */
	virtual void registerListener( IObjectManagerListener * listener ) = 0;

	/**
	 *	Deregister a listener that is notified when objects are registered or
	 *	deregistered.
	 *	
	 *	@pre listener must have already been registered.
	 *	
	 *	@param listener the listener to be deregistered.
	 */
	virtual void deregisterListener( IObjectManagerListener * listener ) = 0;

	/**
	 *	Save all managed objects for the given definition context.
	 *	@param contextDefinitionManager find all of the objects registered with
	 *		this context.
	 *	@param serializer to be used for saving the found objects.
	 *	@return true on success.
	 */
	virtual bool saveObjects( IDefinitionManager& contextDefinitionManager, ISerializer& serializer ) = 0;

	/**
	 *	Load all of the objects from the given serializer and register them to
	 *	be managed.
	 *	@param serializer to be used for loading objects.
	 *	@return true on success.
	 */
	virtual bool loadObjects( ISerializer& serializer ) = 0;

	/**
	 *	Link child objects to their parent object.
	 *	@param objId the ID of the child object.
	 *	@param property containing the child object.
	 *	@param parent the parent object to be linked to the property.
	 */
	virtual void addObjectLinks( const std::string & objId, const IBasePropertyPtr & property, const ObjectHandle & parent ) = 0;
};
} // end namespace wgt
#endif // I_OBJECT_MANAGER_HPP
