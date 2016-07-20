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
	BasePropertyWithMetaData( const IBasePropertyPtr & property, MetaHandle metaData );
	~BasePropertyWithMetaData();

	virtual const TypeId & getType() const override;

	virtual const char * getName() const override;

	virtual uint64_t getNameHash() const override;

	virtual MetaHandle getMetaData() const override;

	virtual bool readOnly() const override;

	//TODO: remove isMethod and add separate accessors to the class definition for properties and methods.
	virtual bool isMethod() const override;

	virtual bool isValue() const override;

	virtual bool set( const ObjectHandle & handle,
		const Variant & value,
		const IDefinitionManager & definitionManager ) const override;

	virtual Variant get( const ObjectHandle & handle,
		const IDefinitionManager & definitionManager ) const override;

	virtual Variant invoke( const ObjectHandle & object,
		const IDefinitionManager & definitionManager,
		const ReflectedMethodParameters & parameters ) override;

	virtual size_t parameterCount() const override;

private:
	IBasePropertyPtr property_;
	MetaHandle metaData_;
};

} // end namespace wgt
#endif // BASE_REFLECTED_PROPERTY_HPP
