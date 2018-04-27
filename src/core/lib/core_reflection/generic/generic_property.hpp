#ifndef GENERIC_PROPERTY_HPP
#define GENERIC_PROPERTY_HPP

#include "../reflected_property.hpp"

namespace wgt
{
class GenericObject;

class GenericProperty : public BaseProperty
{
public:
	GenericProperty(const char* name, const TypeId& typeName, bool isCollection);

	bool isValue() const override;
	Variant get(const ObjectHandle& pBase, const IDefinitionManager& definitionManager) const override;
	bool set(const ObjectHandle& pBase, const Variant& value,
	         const IDefinitionManager& definitionManager) const override;

	bool isCollection() const override;
	bool isByReference() const override;
protected:
	friend class GenericObject;

private:
	const std::string propertyName_;
	const std::string typeName_;
	bool isCollection_;
};
} // end namespace wgt
#endif // GENERIC_PROPERTY_HPP
