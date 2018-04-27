#ifndef I_OBJECT_MANAGER_HPP
#define I_OBJECT_MANAGER_HPP

#include "i_managed_object.hpp"
#include "core_reflection/reflected_object.hpp"
#include "core_dependency_system/i_interface.hpp"
#include "core_reflection/ref_object_id.hpp"
#include "core_common/deprecated.hpp"
#include <tuple>
#include <memory>
#include <string>

namespace wgt
{
class GenericObject;
class ObjectHandle;
class RefObjectId;
class Collection;
class ISerializer;
class ObjectReference;
class RootObjectReference;
class ObjectStorage;
class IDefinitionManager;
typedef std::shared_ptr<class IBaseProperty> IBasePropertyPtr;
typedef std::shared_ptr<class IObjectHandleStorage> ObjectHandleStoragePtr;

/**
 *	Interface for receiving notifications when objects are registered or
 *	deregistered with IObjectManager.
 */
class IObjectManagerListener
{
public:
	virtual ~IObjectManagerListener()
	{
	}

	/**
	 *	Notify that an object has been registered.
	 *	@param pObj the object that has been registered.
	 */
	virtual void onObjectRegistered(const ObjectHandle& pObj)
	{
	}

	/**
	 *	Notify that an object has been deregistered.
	 *	@param pObj the object that has been deregistered.
	 */
	virtual void onObjectDeregistered(const ObjectHandle& pObj)
	{
	}
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
    virtual ~IObjectManager() = default;

	/**
	 *	Get an existing object by an ID.
	 *	Object must be managed by IObjectManager.
	 *	@param id the ID of the object to look up.
	 *	@return found object or null.
	 */
	virtual ObjectHandle getObject(const RefObjectId& id) const = 0;

	/**
	 *	Get an existing, managed object by a pointer to another object.
	 *	Convert your unregistered object to one registered with IObjectManager.
	 *	@param pObj pointer to object to be found.
	 *	@return found object or null.
	 */
	virtual ObjectHandle getObject(const void* pObj) const = 0;

	/**
	 *	Get an existing object reference from an ID and optional path.
	 *	Object must be managed by IObjectManager.
	 *	@param id of the object to look up.
	 *  @param path of the object to look up.
	 *	@return found object reference or null.
	 */
	virtual std::shared_ptr<ObjectReference> getObject(const RefObjectId& id, const std::string& path) = 0;

	/**
	 *	Updates storage for existing reference.
	 *	@param reference whose storage is to be updated.
	 *  @param value of the storage.
	 */
	virtual void updateObjectStorage(const std::shared_ptr<ObjectReference>& reference, const Variant& value) const = 0;

	/**
	*	Clears storage for reference and its children.
	*	@param reference whose storage is to be updated.
	*/
	virtual void clearStorageRecursively(const std::shared_ptr<ObjectReference>& reference) const = 0;

	virtual ManagedObjectPtr createGenericObject(ObjectHandleStoragePtr storage, RefObjectId id = RefObjectId::zero()) const = 0;
    /**
    * Creates object storage to be used in a Managed Object
    * @note should only be used by core reflection
    */
    virtual ObjectHandleStoragePtr createObjectStorage(const Variant& variant) const = 0;

    /**
    * Creates object storage to be used in a Managed Object
    * @note should only be used by core reflection
    */
    virtual ObjectHandleStoragePtr createObjectStorage(std::unique_ptr<GenericObject> object) const = 0;

	/**
	 *	Get a list of all of the managed object IDs that are registered for
	 *	the given definition context.
	 *	@param context the context to get all registered objects under or null to use the current context
	 *	@param o_objects found object IDs are output here.
	 *	@return true if o_objects were output.
	 *		False if the given context could not be found.
	 */
	virtual bool getContextObjects(std::vector<RefObjectId>& o_objects, IObjectManager* context = nullptr) const = 0;

	/**
	 *	Register a managed object.
	 *	@param storage of the object to register.
	 *	@param id the ID of the object to register Or zero to generate a new ID.
	 *  @param context The context to register the object under or null to use the current context
	 *	@return the registered object reference.
	 */
    typedef std::tuple<std::shared_ptr<ObjectStorage>, std::shared_ptr<RootObjectReference>> ObjectTuple;
	virtual ObjectTuple registerObject(const ObjectHandleStoragePtr& storage, const RefObjectId& id) = 0;

	/**
	*	Unregister a managed object.
	*	@param id the ID of the object to unregister.
	*/
	virtual bool unregisterObject(const RefObjectId& id) = 0;

	/**
	 *	Register a definition context.
	 *	@param context the context to be registered or null to use the current context
	 *	@return true if the definition context was registered,
	 *		false if it was already registered.
	 */
	virtual bool registerContext(IObjectManager* context = nullptr) = 0;

	/**
	 *	Deregister a definition context.
	 *	@param context the context to be deregistered or null to use the current context
	 *	@return true if the definition context was deregistered,
	 *		false if it was not found (not registered in the first place).
	 */
	virtual bool deregisterContext(IObjectManager* context = nullptr) = 0;

	/**
	 *	Register a listener that is notified when objects are registered or
	 *	deregistered.
	 *
	 *	@pre listener must not already be registered.
	 *
	 *	@param listener the listener to be registered.
	 */
	virtual void registerListener(IObjectManagerListener* listener) = 0;

	/**
	 *	Deregister a listener that is notified when objects are registered or
	 *	deregistered.
	 *
	 *	@pre listener must have already been registered.
	 *
	 *	@param listener the listener to be deregistered.
	 */
	virtual void deregisterListener(IObjectManagerListener* listener) = 0;

	/**
	 *	Save all managed objects for the given definition context.
	 *	@param context The context to find all objects under or null to use the current context
	 *	@param serializer to be used for saving the found objects.
	 *	@return true on success.
	 */
	virtual bool saveObjects(ISerializer& serializer, IObjectManager* context = nullptr) = 0;

	/**
	 *	Load all of the objects from the given serializer and register them to
	 *	be managed.
	 *	@param serializer to be used for loading objects.
	 *	@return true on success.
	 */
	virtual bool loadObjects(ISerializer& serializer) = 0;

	/**
	 *	Link child objects to their parent object.
	 *	@param objId the ID of the child object.
	 *	@param property containing the child object.
	 *	@param parent the parent object to be linked to the property.
	 */
	virtual void addObjectLinks(const std::string& objId, const IBasePropertyPtr& property,
	                            const ObjectHandle& parent) = 0;

    //--------------------------------------------------------------------------------------------

    template<typename T = void>
    DEPRECATED ObjectHandle createObject(const RefObjectId& id, const std::string& classDef) const
    {
        static_assert(!std::is_same<T, T>::value,
            "This method is now deprecated. Please use ManagedObject");
    }

    template<typename T = void>
    DEPRECATED ObjectHandle createObject(const std::string& classDef) const
    {
        static_assert(!std::is_same<T, T>::value,
            "This method is now deprecated. Please use ManagedObject");
    }

    template<typename T = void>
    DEPRECATED ObjectHandle registerObject(const ObjectHandle& pObj,
                                           RefObjectId id = RefObjectId::zero(),
                                           IObjectManager* context = nullptr)
    {
        static_assert(!std::is_same<T, T>::value,
            "This method is now deprecated. Please use ManagedObject");
    }

    template<typename T = void>
    DEPRECATED void getObjects(std::vector<ObjectHandle>& o_objects) const
    {
        static_assert(!std::is_same<T, T>::value,
            "This method is now deprecated. Please use ManagedObject");
    }

};
} // end namespace wgt
#endif // I_OBJECT_MANAGER_HPP
