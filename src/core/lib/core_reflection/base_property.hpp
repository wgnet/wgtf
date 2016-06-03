#ifndef BASE_REFLECTED_PROPERTY_HPP
#define BASE_REFLECTED_PROPERTY_HPP

#include "interfaces/i_base_property.hpp"
#include "core_variant/type_id.hpp"

namespace wgt
{
class BaseProperty
	: public IBaseProperty
{
public:
	BaseProperty( const char * name, const TypeId & type );

	const TypeId & getType() const override;
	const char * getName() const override;
	uint64_t getNameHash() const override;

	MetaHandle getMetaData() const override;
	virtual bool readOnly() const override;

	virtual bool isMethod() const override;
	virtual bool isValue() const override;

	virtual bool set( const ObjectHandle & handle,
		const Variant & value,
		const IDefinitionManager & definitionManager ) const override;

	virtual Variant get( const ObjectHandle & handle,
		const IDefinitionManager & definitionManager ) const override;

	virtual Variant invoke( const ObjectHandle& object,
		const IDefinitionManager & definitionManager,
		const ReflectedMethodParameters& parameters ) override;

	virtual size_t parameterCount() const override;

protected:
	void setType( const TypeId & type ); 
	void setName( const char * name );

private:
	const char *		name_;
	TypeId				type_;
	uint64_t			hash_;

	friend class PropertyAccessor;
};
} // end namespace wgt
#endif // BASE_REFLECTED_PROPERTY_HPP
