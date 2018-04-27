#ifndef BASE_PROPERTY_WITH_METADATA_HPP
#define BASE_PROPERTY_WITH_METADATA_HPP

#include "interfaces/i_base_property.hpp"
#include "metadata/meta_base.hpp"
#include "reflection_dll.hpp"

#include <memory>

namespace wgt
{
class REFLECTION_DLL BasePropertyWithMetaData : public IBaseProperty
{
public:
	BasePropertyWithMetaData(const IBasePropertyPtr& property, MetaData metaData);
	virtual ~BasePropertyWithMetaData();

	std::shared_ptr< IPropertyPath> generatePropertyName(
		const std::shared_ptr< const IPropertyPath > & parent) const override;

	virtual const TypeId& getType() const override;

	virtual const char* getName() const override;

	virtual uint64_t getNameHash() const override;

	virtual const MetaData & getMetaData() const override;

	virtual bool readOnly(const ObjectHandle& handle) const override;

	// TODO: remove isMethod and add separate accessors to the class definition for properties and methods.
	virtual bool isMethod() const override;

	virtual bool isValue() const override;

	virtual bool isCollection() const override;

	virtual bool isByReference() const override;

	virtual bool set(const ObjectHandle& handle, const Variant& value,
	                 const IDefinitionManager& definitionManager) const override;

	virtual Variant get(const ObjectHandle& handle, const IDefinitionManager& definitionManager) const override;

	virtual Variant invoke(const ObjectHandle& object, const IDefinitionManager& definitionManager,
	                       const ReflectedMethodParameters& parameters) override;

	virtual size_t parameterCount() const override;

	IBasePropertyPtr baseProperty() const;

private:
	IBasePropertyPtr property_;
    MetaData metaData_;
};

} // end namespace wgt
#endif // BASE_REFLECTED_PROPERTY_HPP
