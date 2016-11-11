#pragma once
#ifndef PYTHON_PROPERTY_HPP
#define PYTHON_PROPERTY_HPP

#include "core_reflection/interfaces/i_base_property.hpp"

namespace wgt
{
class IComponentContext;
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
	Property(IComponentContext& context, const char* key, const PyScript::ScriptObject& pythonObject,
	         MetaHandle metaData);

	Property(IComponentContext& context, const char* key, const TypeId& typeId,
	         const PyScript::ScriptObject& pythonObject, MetaHandle metaData);

	const TypeId& getType() const override;

	const char* getName() const override;

	uint64_t getNameHash() const override;

	MetaHandle getMetaData() const override;

	bool readOnly() const override;

	bool isMethod() const override;
	bool isValue() const override;

	bool set(const ObjectHandle& handle, const Variant& value,
	         const IDefinitionManager& definitionManager) const override;

	Variant get(const ObjectHandle& handle, const IDefinitionManager& definitionManager) const override;

	Variant invoke(const ObjectHandle& object, const IDefinitionManager& definitionManager,
	               const ReflectedMethodParameters& parameters) override;

	size_t parameterCount() const override;

	void updatePropertyData(const char* name, const PyScript::ScriptObject& pythonObject, MetaHandle metaData);

private:
	class Implementation;
	std::unique_ptr<Implementation> impl_;
};

} // namespace ReflectedPython
} // end namespace wgt
#endif // PYTHON_PROPERTY_HPP
