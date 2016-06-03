#include "reflected_tree_model.hpp"
#include "core_reflection/reflected_object.hpp"
#include "core_data_model/i_item_role.hpp"
#include "core_data_model/reflection/reflected_property_item.hpp"
#include "core_reflection/property_accessor_listener.hpp"

namespace wgt
{
class ReflectedTreeModelPropertyListener
	: public PropertyAccessorListener
{
public:
	ReflectedTreeModelPropertyListener( ReflectedObjectItem & item )
		: rootItem_( item )
	{
	}

	// PropertyAccessorListener
	void preSetValue(
		const PropertyAccessor & accessor, const Variant & value ) override;
	void postSetValue(
		const PropertyAccessor & accessor, const Variant & value ) override;
private:
	ReflectedObjectItem & rootItem_;
};


//==============================================================================
ReflectedTreeModel::ReflectedTreeModel(
	const ObjectHandle & object,
	IDefinitionManager & definitionManager,
	IReflectionController * controller )
	: base( 2 )
	, rootItem_( object )
	, definitionManager_( definitionManager )
	, listener_( new ReflectedTreeModelPropertyListener( rootItem_ ) )
{
	definitionManager_.registerPropertyAccessorListener( listener_ );

	rootItem_.setController( controller );
	rootItem_.setDefinitionManager( &definitionManager_ );
	addRootItem( &rootItem_ );
}


//==============================================================================
ReflectedTreeModel::~ReflectedTreeModel()
{
	this->removeRootItem( &rootItem_ );
	definitionManager_.deregisterPropertyAccessorListener( listener_ );
}


void ReflectedTreeModel::addRootItem( GenericTreeItem * item ) /* override */
{
	// ReflectedTreeModel does not support multiple roots
	assert( item == &rootItem_ );
	base::addRootItem( item );
}


void ReflectedTreeModel::removeRootItem( GenericTreeItem * item ) /* override */
{
	// ReflectedTreeModel does not support multiple roots
	assert( item == &rootItem_ );
	base::removeRootItem( item );
}

//==============================================================================
void ReflectedTreeModelPropertyListener::preSetValue( 
	const PropertyAccessor & accessor, const Variant & value )
{
	rootItem_.preSetValue( accessor, value );
}


//==============================================================================
void ReflectedTreeModelPropertyListener::postSetValue( 
	const PropertyAccessor & accessor, const Variant & value )
{
	rootItem_.postSetValue( accessor, value );
}
} // end namespace wgt
