#ifndef GENERIC_DEFINITION_HPP
#define GENERIC_DEFINITION_HPP

#include "core_reflection/interfaces/i_class_definition_details.hpp"
#include "core_reflection/interfaces/i_class_definition_modifier.hpp"
#include "core_reflection/metadata/meta_base.hpp"
#include "core_reflection/property_storage.hpp"
#include <string>

namespace wgt
{
class IDefinitionManager;

/**
 *	GenericDefinition
 */
class GenericDefinition : public IClassDefinitionDetails, public IClassDefinitionModifier
{
private:
	friend class DefinitionManager;
	GenericDefinition(const char* name);

public:
	ObjectHandleStoragePtr createObjectStorage(const IClassDefinition& definition) const override;
	ManagedObjectPtr createManaged(const IClassDefinition& definition, RefObjectId id = RefObjectId::zero()) const override;

	bool isAbstract() const override
	{
		return false;
	}
	bool isGeneric() const override
	{
		return true;
	}
	const MetaData & getMetaData() const override
	{
		static MetaData metaData_;
		return metaData_;
	}

	const char* getName() const override;

	bool canDirectLookupProperty() const override
	{
		return true;
	}

	IBasePropertyPtr directLookupProperty(const char* name) const override
	{
		return properties_.findProperty( name );
	}

	PropertyIteratorImplPtr getPropertyIterator() const override;
	IClassDefinitionModifier* getDefinitionModifier() const override
	{
		return const_cast<GenericDefinition*>(this);
	}

	IBasePropertyPtr addProperty(const char* name, const TypeId& typeId, MetaData metaData,
	                             bool isCollection) override;
	void removeProperty(const char* name) override;

private:
	const std::string name_;
	PropertyStorage properties_;
};
} // end namespace wgt
#endif // GENERIC_DEFINITION_HPP
