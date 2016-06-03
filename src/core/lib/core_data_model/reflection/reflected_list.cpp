#include "reflected_list.hpp"

#include "core_reflection/property_accessor.hpp"
#include "core_reflection/property_accessor_listener.hpp"
#include "core_reflection/definition_manager.hpp"

#include "core_data_model/i_item_role.hpp"

namespace wgt
{
class ReflectedListListener
	: public PropertyAccessorListener
{
public:
	ReflectedListListener( const ReflectedList & list )
		: list_( list )
	{
	}

	// PropertyAccessorListener
	void preSetValue( const PropertyAccessor & accessor,
		const Variant & value ) override;
	void postSetValue( const PropertyAccessor & accessor,
		const Variant & value ) override;

private:
	const IItem* find(const PropertyAccessor & accessor);
	size_t findIndex(const Variant obj);

	const ReflectedList & list_;
};

ReflectedList::ReflectedList( IDefinitionManager* defManager )
	: listener_( new ReflectedListListener( *this ) )
	, defManager_( defManager )
{
	defManager_->registerPropertyAccessorListener( listener_ );
}

ReflectedList::~ReflectedList()
{
	defManager_->deregisterPropertyAccessorListener( listener_ );
}

const IItem* ReflectedListListener::find(const PropertyAccessor & accessor)
{
	const Variant obj = accessor.getRootObject();
	auto it = std::find_if( list_.cbegin(), list_.cend(),
		[&](const Variant& item) { return obj == item; } );
	return (it != list_.cend()) ? list_.item( it - list_.cbegin() ) : nullptr;
}

size_t ReflectedListListener::findIndex(const Variant obj)
{
	auto it = std::find_if( list_.cbegin(), list_.cend(),
		[&](const Variant& item) { return obj == item; } );
	return (it != list_.cend()) ? it - list_.cbegin() : -1;
}

void ReflectedListListener::preSetValue( const PropertyAccessor & accessor, const Variant & value )
{
	if (auto item = find(accessor))
	{
		list_.signalPreItemDataChanged( item, 0, DefinitionRole::roleId_, value );
	}
}

void ReflectedListListener::postSetValue( 
	const PropertyAccessor & accessor, const Variant & value )
{
	if (auto item = find(accessor))
	{
		list_.signalPostItemDataChanged( item, 0, DefinitionRole::roleId_, value );
	}
}
} // end namespace wgt
