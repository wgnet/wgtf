#pragma once
#ifndef PYTHON_PROPERTY_HPP
#define PYTHON_PROPERTY_HPP

#include "core_reflection/interfaces/i_base_property.hpp"

namespace wgt
{
namespace PyScript
{
class ScriptObject;
} // namespace PyScript

namespace ReflectedPython
{
/**
 *	Python-specific property that belongs to a Definition.
 */
class Property : public IBaseProperty
{
public:
	/**
	 *	Construct a property given the attribute name and value.
	 *
	 *	@param key name of the attribute. The property will copy the string
	 *		to its own storage.
	 *	@param attribute value of the attribute. Keeps a reference.
	 */
	Property(const char* key, const PyScript::ScriptObject& pythonObject, MetaData metaData);

	Property(const char* key, const TypeId& typeId, const PyScript::ScriptObject& pythonObject, MetaData metaData);

	std::shared_ptr<IPropertyPath> generatePropertyName(
		const std::shared_ptr<const IPropertyPath> & parent) const override;

	const TypeId& getType() const override;

	const char* getName() const override;

	uint64_t getNameHash() const override;

	const MetaData & getMetaData() const override;

	bool readOnly(const ObjectHandle&) const override;

	bool isMethod() const override;
	bool isValue() const override;
	bool isCollection() const override;
	bool isByReference() const override;

	bool set(const ObjectHandle& handle, const Variant& value,
	         const IDefinitionManager& definitionManager) const override;

	Variant get(const ObjectHandle& handle, const IDefinitionManager& definitionManager) const override;

	Variant invoke(const ObjectHandle& object, const IDefinitionManager& definitionManager,
	               const ReflectedMethodParameters& parameters) override;

	size_t parameterCount() const override;

	void updatePropertyData(const char* name, const PyScript::ScriptObject& pythonObject, MetaData metaData);

private:
	class Implementation;
	std::unique_ptr<Implementation> impl_;
};

} // namespace ReflectedPython
} // end namespace wgt
#endif // PYTHON_PROPERTY_HPP
