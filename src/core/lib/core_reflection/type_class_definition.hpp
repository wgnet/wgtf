#ifndef TYPE_CLASS_DEFINITION_HPP
#define TYPE_CLASS_DEFINITION_HPP

#include "interfaces/i_class_definition_details.hpp"
#include "interfaces/i_class_definition_modifier.hpp"
#include "utilities/definition_helpers.hpp"
#include "metadata/meta_utilities.hpp"
#include "property_storage.hpp"
#include "core_object/i_object_handle_storage.hpp"
#include "core_object/managed_object.hpp"
#include <memory>

namespace wgt
{
template <typename Type>
class TypeClassDefinition : public IClassDefinitionDetails
{
	typedef Type SelfType;

	MetaData metaData_;
public:
	TypeClassDefinition();

	//--------------------------------------------------------------------------
	bool isAbstract() const override
	{
		return std::is_abstract<Type>();
	}

	//--------------------------------------------------------------------------
	bool isGeneric() const override
	{
		return false;
	}

	//--------------------------------------------------------------------------
	const char* getName() const override
	{
		return getClassIdentifier<Type>();
	}

	//--------------------------------------------------------------------------
	const MetaData & getMetaData() const override
	{
		return metaData_;
	}

	//--------------------------------------------------------------------------
	PropertyIteratorImplPtr getPropertyIterator() const override
	{
		return properties_.getIterator();
	}


	//--------------------------------------------------------------------------
	bool canDirectLookupProperty() const override
	{
		return true;
	}


	//--------------------------------------------------------------------------
	IBasePropertyPtr directLookupProperty(const char* name) const
	{
		return properties_.findProperty( name );
	}


	//--------------------------------------------------------------------------
	IBasePropertyPtr directLookupProperty(IPropertyPath::ConstPtr & path) const
	{
		return properties_.findProperty( path );
	}

	//--------------------------------------------------------------------------
	IClassDefinitionModifier* getDefinitionModifier() const override
	{
		return nullptr;
	}

	//--------------------------------------------------------------------------
    template <class... Parameter>
    DEPRECATED static ObjectHandleT<Type> create(const IClassDefinition& definition, Parameter&&... parameter)
	{
        static_assert(!std::is_same<Type, Type>::value,
            "This method is now deprecated. Please use ManagedObject");
	}

	//--------------------------------------------------------------------------
	virtual ObjectHandleStoragePtr createObjectStorage(const IClassDefinition&) const override
	{
		auto pointer = CreateHelper<Type>::create();
		return std::make_shared<ObjectHandleStorage<std::unique_ptr<Type>>>(std::unique_ptr<Type>(pointer));
	}

	//--------------------------------------------------------------------------
	virtual ManagedObjectPtr createManaged(const IClassDefinition& definition, RefObjectId id = RefObjectId::zero()) const override
	{
		auto storage = createObjectStorage(definition);
		return ManagedObjectPtr(new ManagedObject<Type>(storage, id));
	}

	//--------------------------------------------------------------------------
    template <class... Parameter>
    static ObjectHandleStoragePtr createObjectStorage(Parameter&&... parameter)
	{
        auto pointer = CreateHelper<Type>::create(std::forward<Parameter>(parameter)...);
        return std::make_shared<ObjectHandleStorage<std::unique_ptr<Type>>>(std::move(std::unique_ptr<Type>(pointer)));
	}

private:
	PropertyStorage properties_;
};
} // end namespace wgt
#endif // #define TYPE_CLASS_DEFINITION_HPP
