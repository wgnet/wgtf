#ifndef REFLECTED_COLLECTION_IMPL_HPP
#define REFLECTED_COLLECTION_IMPL_HPP

#include "../reflection_dll.hpp"
#include "../interfaces/i_enum_generator.hpp"
#include "../interfaces/i_base_property.hpp"
#include "core_variant/collection.hpp"

namespace wgt
{
class REFLECTION_DLL ReflectedCollectionImpl : public IEnumGenerator
{
public:
	ReflectedCollectionImpl(const IBasePropertyPtr& baseProperty);
	virtual ~ReflectedCollectionImpl();

	Collection getCollection(const ObjectHandle& provider, const IDefinitionManager& definitionManager) override;

private:
	IBasePropertyPtr baseProperty_;
};
} // end namespace wgt
#endif // REFLECTED_COLLECTION_IMPL_HPP
