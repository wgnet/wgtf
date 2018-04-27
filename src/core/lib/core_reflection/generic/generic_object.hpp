#ifndef GENERIC_OBJECT_HPP
#define GENERIC_OBJECT_HPP

#include "base_generic_object.hpp"

#include "core_common/assert.hpp"
#include "core_reflection/reflection_dll.hpp"
#include "core_reflection/interfaces/i_class_definition_details.hpp"
#include "core_reflection/ref_object_id.hpp"
#include "core_reflection/object/object_handle.hpp"
#include "core_reflection/property_accessor.hpp"
#include "core_reflection/interfaces/i_base_property.hpp"
#include "core_object/managed_object.hpp"
#include "core_object/i_object_manager.hpp"
#include "core_object/object_handle_provider.hpp"
#include "core_dependency_system/depends.hpp"
#include <unordered_map>

namespace wgt
{
typedef ObjectHandleT<class GenericObject> GenericObjectPtr;
class GenericDefinition;
class GenericProperty;
class TypeId;

/**
 *	GenericObject is an object that has a "generic type".
 *	And it has storage for any properties that are added/removed.
 */
class GenericObject : public BaseGenericObject
                    , public ObjectHandleProvider<GenericObject>
{
public:
	static ManagedObject<GenericObject> create(const RefObjectId& id = RefObjectId::zero(),
											   const char* classDefinitionName = nullptr)
	{
		auto defManager = DependsLocal<IDefinitionManager>().get<IDefinitionManager>();
		TF_ASSERT(defManager);
		auto defDetails = defManager->createGenericDefinition(classDefinitionName);
		auto definition = defManager->registerDefinition(std::move(defDetails));
		return ManagedObject<GenericObject>(definition->createObjectStorage(), id);
	}

    void operator=(const GenericObjectPtr& otherHandle)
	{
        if (handle() != otherHandle)
        {
            properties_.clear();
            auto definition = otherHandle->getDefinition();
            TF_ASSERT(definition);
            for (auto property : definition->allProperties())
            {
                const char* name = property->getName();
                set(name, definition->bindProperty(name, otherHandle).getValue());
            }
        }
	}

private:
	friend class GenericProperty;
	friend class GenericDefinition;

	GenericObject() = default;
	GenericObject(const GenericObject&) = delete;
	const GenericObject& operator=(const GenericObject&) = delete;

	virtual ObjectHandle getDerivedType() const override
	{
		return handle();
	}

	virtual ObjectHandle getDerivedType() override
	{
		return handle();
	}

	mutable std::unordered_map<const GenericProperty*, Variant> properties_;
};
} // end namespace wgt
#endif // GENERIC_OBJECT_HPP
