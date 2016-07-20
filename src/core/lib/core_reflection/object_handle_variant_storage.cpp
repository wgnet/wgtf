#include "i_definition_manager.hpp"
#include "object_handle_variant_storage.hpp"


namespace wgt
{
ObjectHandleVariantStorage::ObjectHandleVariantStorage( const Variant& variant, const IClassDefinition* definition ):
	variant_( variant ),
	variantPtr_( &variant_ ),
	definition_( definition )
{
}


ObjectHandleVariantStorage::ObjectHandleVariantStorage( Variant* variant, const IClassDefinition* definition ):
	variant_(),
	variantPtr_( variant ),
	definition_( definition )
{
}


void* ObjectHandleVariantStorage::data() const
{
	return variantPtr_->value< void* >();
}


TypeId ObjectHandleVariantStorage::type() const
{
	return variantPtr_->type()->typeId();
}


bool ObjectHandleVariantStorage::getId( RefObjectId& o_Id ) const
{
	return false;
}
} // end namespace wgt
