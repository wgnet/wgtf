#ifndef GENERIC_PROPERTY_HPP
#define GENERIC_PROPERTY_HPP

#include "../reflected_property.hpp"

namespace wgt
{
class GenericObject;

class GenericProperty : public BaseProperty
{
public:
	GenericProperty( const char * name,	
					 const TypeId & typeName )
		: BaseProperty( name, typeName )
		, propertyName_( name )
		, typeName_( typeName.getName() )
	{
		setType( typeName_.c_str() );
	}
	const char * getName() const override;

	virtual bool isValue() const override;
	Variant get( const ObjectHandle & pBase, const IDefinitionManager & definitionManager ) const override;
	bool set( const ObjectHandle & pBase, const Variant & value, const IDefinitionManager & definitionManager ) const override;

	bool isCollection() const
	{
		//No support for collections for now
		return false;
	}

protected:
	friend class GenericObject;
private:
	const std::string propertyName_;
	const std::string typeName_;
};
} // end namespace wgt
#endif //GENERIC_PROPERTY_HPP
