#ifndef REFLECTED_COLLECTION_IMPL_HPP
#define REFLECTED_COLLECTION_IMPL_HPP

#include "../interfaces/i_enum_generator.hpp"
#include "../interfaces/i_base_property.hpp"
#include "core_variant/collection.hpp"

namespace wgt
{
//TODO Move this into a CPP instead
// ==============================================================================
class ReflectedCollectionImpl
	: public IEnumGenerator
{
public:
	ReflectedCollectionImpl( const IBasePropertyPtr & baseProperty )
		: baseProperty_( baseProperty )
	{
	}

	//==========================================================================
	~ReflectedCollectionImpl()
	{
	}

	//==========================================================================
	Collection getCollection( const ObjectHandle & provider, const IDefinitionManager & definitionManager ) override
	{
		Variant value = baseProperty_->get( provider, definitionManager );
		Collection collection;
		bool ok = value.tryCast( collection );
		assert( ok );
		return collection;
	}

private:
	IBasePropertyPtr baseProperty_;
};
} // end namespace wgt
#endif // REFLECTED_COLLECTION_IMPL_HPP
