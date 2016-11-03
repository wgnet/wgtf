#ifndef CLASS_DEFINITION_HPP
#define CLASS_DEFINITION_HPP

#include "interfaces/i_class_definition.hpp"
#include "interfaces/i_class_definition_modifier.hpp"

#include "core_variant/type_id.hpp"

#include <memory>
#include <unordered_map>

namespace wgt
{
class IClassDefinitionDetails;

class REFLECTION_DLL ClassDefinition : public IClassDefinition
{ 
public: 
	ClassDefinition( std::unique_ptr<IClassDefinitionDetails> details );
	virtual ~ClassDefinition();

	const IClassDefinitionDetails & getDetails() const override;

	// Range for all properties contained in this and its parents' definitions
	PropertyIteratorRange allProperties() const override;
	// Range for only properties contain in this definition
	PropertyIteratorRange directProperties() const override;

	PropertyAccessor bindProperty(
		const char * name, const ObjectHandle & object ) const override;

	IClassDefinition * getParent() const override;

	bool isGeneric() const override;
	bool canBeCastTo( const IClassDefinition & definition ) const override;
	void * castTo( const IClassDefinition & definition, void * object ) const override;
	void * upCast( void * object ) const override;

	IDefinitionManager * getDefinitionManager() const override;

	const char * getName() const override;
	MetaHandle getMetaData() const override;
	ObjectHandle create() const override;
	ObjectHandle createManagedObject( const RefObjectId & id = RefObjectId::zero() ) const override;

protected:
	ObjectHandle registerObject( ObjectHandle & pObj, 
		const RefObjectId & id = RefObjectId::zero() ) const;

private:
	std::unique_ptr< const IClassDefinitionDetails > details_;
	IDefinitionManager *			defManager_;

	friend class PropertyIterator;
	friend class PropertyAccessor;

	IBasePropertyPtr findProperty( const char * name, size_t length ) const override;
	void setDefinitionManager( IDefinitionManager * defManager ) override;

	void bindPropertyImpl(
		const char * name,
		const ObjectHandle & pBase,
		PropertyAccessor & o_PropertyAccessor ) const override;
};
} // end namespace wgt
#endif // #define CLASS_DEFINITION_HPP
