#include "reflected_collection.hpp"

namespace wgt
{
//------------------------------------------------------------------------------
ReflectedCollectionImpl::ReflectedCollectionImpl(const IBasePropertyPtr& baseProperty) : baseProperty_(baseProperty)
{
}

//------------------------------------------------------------------------------
/*virtual */ ReflectedCollectionImpl::~ReflectedCollectionImpl()
{
}

//------------------------------------------------------------------------------
Collection ReflectedCollectionImpl::getCollection(const ObjectHandle& provider,
                                                  const IDefinitionManager& definitionManager)
{
	Variant value = baseProperty_->get(provider, definitionManager);
	Collection collection;
	bool ok = value.tryCast(collection);
	assert(ok);
	return collection;
}

} // end namespace wgt
