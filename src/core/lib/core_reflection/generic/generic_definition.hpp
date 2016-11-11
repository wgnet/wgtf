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
	ObjectHandle create(const IClassDefinition& definition) const override;
	bool isAbstract() const override
	{
		return false;
	}
	bool isGeneric() const override
	{
		return true;
	}
	MetaHandle getMetaData() const override
	{
		return nullptr;
	}
	const char* getParentName() const override
	{
		return nullptr;
	}
	const char* getName() const override;

	void* upCast(void* object) const override
	{
		return nullptr;
	}

	PropertyIteratorImplPtr getPropertyIterator() const override;
	IClassDefinitionModifier* getDefinitionModifier() const override
	{
		return const_cast<GenericDefinition*>(this);
	}

	IBasePropertyPtr addProperty(const char* name, const TypeId& typeId, MetaHandle metaData) override;

private:
	const std::string name_;
	PropertyStorage properties_;
};
} // end namespace wgt
#endif // GENERIC_DEFINITION_HPP
