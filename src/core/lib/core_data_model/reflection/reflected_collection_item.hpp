#ifndef REFLECTED_COLLECTION_ITEM_HPP
#define REFLECTED_COLLECTION_ITEM_HPP

#include "core_reflection/interfaces/i_base_property.hpp"
#include "core_reflection/metadata/meta_base.hpp"
#include "core_variant/collection.hpp"

namespace wgt
{
struct ReflectedIteratorValue
{
	Collection::Iterator iterator;
	Variant value;
};

class ReflectedIteratorProperty : public IBaseProperty
{
public:
	ReflectedIteratorProperty(std::string&& name, const TypeId& type, IDefinitionManager& defMng);

	bool readOnly() const override;
	bool isValue() const override;
	bool set(const ObjectHandle& handle, const Variant& value,
	         const IDefinitionManager& definitionManager) const override;
	Variant get(const ObjectHandle& handle, const IDefinitionManager& definitionManager) const override;

	const TypeId& getType() const override;
	const char* getName() const override;
	uint64_t getNameHash() const override;

	MetaHandle getMetaData() const override;
	bool isMethod() const override;
	Variant invoke(const ObjectHandle& object, const IDefinitionManager& definitionManager,
	               const ReflectedMethodParameters& parameters) override;
	size_t parameterCount() const override;

private:
	std::string name;
	uint64_t nameHash;
	TypeId type;
	IDefinitionManager& definitionManager;
};

std::string BuildIteratorPropertyName(const Collection::Iterator& iterator);
} // namespace wgt

#endif