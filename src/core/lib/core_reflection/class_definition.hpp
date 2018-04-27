#ifndef CLASS_DEFINITION_HPP
#define CLASS_DEFINITION_HPP

#include "interfaces/i_class_definition.hpp"
#include "interfaces/i_class_definition_modifier.hpp"
#include "interfaces/i_property_path.hpp"

#include "core_variant/type_id.hpp"

#include <memory>
#include <unordered_map>

namespace wgt
{
class IClassDefinitionDetails;
class Variant;
class ObjectReference;

class REFLECTION_DLL ClassDefinition : public IClassDefinition
{
public:
	ClassDefinition(std::unique_ptr<IClassDefinitionDetails> details);
	virtual ~ClassDefinition();

	const IClassDefinitionDetails& getDetails() const override;

	/**
	* Range for all properties contained in this and its parents' definitions
	*/
	PropertyIteratorRange allProperties() const override;

	/**
	* Range for only properties contain in this definition
	*/
	PropertyIteratorRange directProperties() const override;


	PropertyAccessor bindProperty( IPropertyPath::ConstPtr & path, const ObjectHandle& object) const override;
	PropertyAccessor bindProperty(const char* name, const ObjectHandle& object) const override;

	const std::vector< std::string > & getParentNames() const override;

	bool isGeneric() const override;
	bool canBeCastTo(const IClassDefinition& definition) const override;
	void* castTo(const IClassDefinition& definition, void* object) const override;

	IDefinitionManager* getDefinitionManager() const override;

	const char* getName() const override;
	const MetaData & getMetaData() const override;
	ObjectHandleStoragePtr createObjectStorage() const override;
	ManagedObjectPtr createManaged(RefObjectId id = RefObjectId::zero()) const override;
	ObjectHandle createShared(RefObjectId id = RefObjectId::zero()) const override;

private:
	friend class PropertyIterator;

	IBasePropertyPtr findProperty(const char* name, size_t length) const override;
	IBasePropertyPtr findProperty( IPropertyPath::ConstPtr & path ) const override;
	void setDefinitionManager(IDefinitionManager* defManager) override;

	std::shared_ptr<ObjectReference> getChildReference(
		const std::shared_ptr<ObjectReference>& parent, const char* path, const Variant& value) const;

	struct Impl;
	std::unique_ptr< Impl > impl_;
};
} // end namespace wgt
#endif // #define CLASS_DEFINITION_HPP
