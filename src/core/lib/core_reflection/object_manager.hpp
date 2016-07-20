#ifndef OBJECT_MANAGER_HPP
#define OBJECT_MANAGER_HPP

#include <unordered_set>
#include <unordered_map>
#include <map>

#include "i_object_manager.hpp"
#include "reflected_object.hpp"
#include "ref_object_id.hpp"
#include "core_serialization/serializer/i_serializer.hpp"
#include "reflection_dll.hpp"

namespace wgt
{
struct ObjectMetaData;

typedef std::unordered_set< ObjectMetaData * > ObjIdSet;

/**
 *	Default implementation of IObjectManager.
 *	
 *	Note that ObjectManager holds strong references to "unmanaged" objects.
 *	So the owner must unregister the object with ObjectManager before deletion.
 */
class REFLECTION_DLL ObjectManager 
	: public Implements< IObjectManager >
{
public:
	// IObjectManager
	ObjectHandle getObject( const RefObjectId& id ) const override;
	ObjectHandle getObject( const void * pObj ) const override;
	ObjectHandle getUnmanagedObject( const void * pObj ) const override;
	bool getUnmanagedObjectId(const void * pObj, RefObjectId & id) const override;

	bool getContextObjects( IDefinitionManager * context,
		std::vector< RefObjectId > & o_objects ) const override;
	void getObjects( std::vector< ObjectHandle > & o_objects ) const override;

	ObjectHandle registerObject(
		ObjectHandle & object, 
		const RefObjectId& id = RefObjectId::zero() ) override;

	RefObjectId registerUnmanagedObject(
		const ObjectHandle & object, 
		const RefObjectId& id = RefObjectId::zero() ) override;

	bool registerContext( IDefinitionManager * context ) override;
	bool deregisterContext( IDefinitionManager * context ) override;

	void registerListener( IObjectManagerListener * listener ) override;
	void deregisterListener( IObjectManagerListener * listener ) override;

	bool saveObjects( IDefinitionManager& contextDefinitionManager, ISerializer& serializer ) override;
	bool loadObjects( ISerializer& serializer ) override;
	void addObjectLinks( const std::string & objId, const IBasePropertyPtr & property, const ObjectHandle & parent ) override;

	ObjectManager();
	virtual ~ObjectManager();
	void init( IDefinitionManager * pDefManager );

private:
	ObjectHandle createObject( 
		const RefObjectId & id,
		const std::string & classDef ) const override;

	ObjectHandle create( 
		const std::string & classDef ) const override;

	void resolveObjectLink( const RefObjectId & objId, const ObjectHandle& object );

	void deregisterMetaData( ObjectMetaData & metaData );

	void NotifyObjectRegistred( const ObjectHandle & handle ) const;
	void NotifyObjectDeregistred( const ObjectHandle & handle ) const;

private:
	// all references to a reflected object
	// maps id to reflected object & its references
	std::unordered_map< const RefObjectId, std::weak_ptr< ObjectMetaData > > idMap_;
	std::unordered_map< const void *, std::weak_ptr< ObjectMetaData > > metaDataMap_;
	std::unordered_map< const void *, std::shared_ptr< ObjectMetaData > > unmanagedMetaDataMap_;
	mutable std::mutex objectsLock_;

	// all objects of a definition context
	// maps definition context to all its objects
	typedef std::unordered_map< IDefinitionManager *, std::shared_ptr< ObjIdSet > > ContextObjects;
	ContextObjects contextObjects_;

	IDefinitionManager * pDefManager_;

	typedef std::vector< IObjectManagerListener * > ObjectManagerListener;
	ObjectManagerListener listeners_;

	mutable std::mutex listenersLock_;
	typedef std::pair< IBasePropertyPtr, ObjectHandle > LinkPair;
	std::unordered_map< const RefObjectId, LinkPair > objLink_;
	mutable std::mutex objLinkLock_;
};
} // end namespace wgt
#endif // OBJECT_MANAGER_HPP
