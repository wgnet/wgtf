#include "reflection_utilities.hpp"

#include "core_common/assert.hpp"
#include "core_reflection/property_accessor.hpp"
#include "core_reflection/base_property.hpp"
#include "core_reflection/interfaces/i_class_definition.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/metadata/meta_base.hpp"
#include "core_reflection/metadata/meta_impl.hpp"
#include "core_object/object_reference.hpp"
#include "core_logging/logging.hpp"
#include "core_reflection/metadata/meta_types.hpp"
#include "../private/collection_element_holder.hpp"

namespace wgt
{
namespace ReflectionUtilities
{
// =============================================================================
bool isStruct(const PropertyAccessor& pa)
{
	auto type = pa.getType();
	if (type.isPointer())
	{
		return false;
	}

	auto value = pa.getValue();
	ObjectHandle handle;
	if (!value.tryCast(handle))
	{
		return false;
	}

	return pa.getDefinitionManager()->getDefinition(handle) != nullptr;
}

// =============================================================================
template <>
Variant copy<Variant>(Variant& value)
{
	return value;
}

// =============================================================================
template <>
Variant copy<const Variant>(const Variant& value)
{
	return value;
}

// =============================================================================
template <>
Variant reference<Variant>(Variant& value)
{
	return value;
}

// =============================================================================
template <>
Variant reference<const Variant>(const Variant& value)
{
	return value;
}

// =============================================================================
void copyProperties(IDefinitionManager& definitionManager, const ObjectHandle& src, const ObjectHandle& dst,
                    bool notify)
{
	if (!src.isValid() && !dst.isValid())
	{
		return;
	}

	if (!src.isValid() || !dst.isValid() || src.type() != dst.type())
	{
		NGT_ERROR_MSG("src and dst must be the same type and valid");
		return;
	}

	auto definitionSrc = definitionManager.getDefinition(src);
	auto definitionDst = definitionManager.getDefinition(dst);

	for (auto property : definitionDst->allProperties())
	{
		const char* name = property->getName();

		PropertyAccessor accessorSrc = definitionSrc->bindProperty(name, src);
		PropertyAccessor accessorDst = definitionDst->bindProperty(name, dst);
		if (!accessorSrc.isValid() || !accessorDst.isValid())
		{
			NGT_ERROR_MSG("src and dst must both include property %s", name);
			continue;
		}

		if (!accessorSrc.canGetValue() || !accessorDst.canGetValue())
		{
			continue;
		}

		auto valueSrc = accessorSrc.getValue();
		auto valueDst = accessorDst.getValue();

		if (valueSrc.canCast<ObjectHandle>())
		{
			auto handleSrc = valueSrc.cast<ObjectHandle>();
			auto handleDst = valueDst.cast<ObjectHandle>();
			copyProperties(definitionManager, handleSrc, handleDst, notify);
		}
		else if (valueSrc != valueDst)
		{
			if (notify)
			{
				accessorDst.setValue(valueSrc);
			}
			else
			{
				accessorDst.setValueWithoutNotification(valueSrc);
			}
		}
	}
}

std::tuple<IBasePropertyPtr, bool, std::string> parseProperty(const char* path, const Variant& object,
	const IBasePropertyPtr& parentProperty, const IClassDefinition* definition, const IDefinitionManager& definitionManager)
{
	auto pathPosition = path;

	if (*pathPosition == Collection::getIndexOpen())
	{
		Collection collection;

		if (!object.tryCast(collection))
		{
			ObjectHandle handle;
			bool castSuccessful = object.tryCast(handle);
			Collection* collectionPointer = handle.getBase<Collection>();

			if (!castSuccessful || collectionPointer == nullptr)
			{
				// index operator is applicable to collections only
				return std::make_tuple(nullptr, false, "");
			}

			collection = *collectionPointer;
		}

		// determine key type (heterogeneous keys are not supported yet)
		const auto begin = collection.begin();
		const auto end = collection.end();

		if (begin == end)
		{
			// can't index empty collection
			return std::make_tuple(nullptr, false, "");
		}

		auto keyBegin = pathPosition;
		Variant key = Collection::parseKey(begin.key().type(), pathPosition);
		auto keyEnd = pathPosition;
		auto it = collection.find(key);
		std::string name(keyBegin, keyEnd - keyBegin);
		// TODO Cache these against the parent reference
		auto property = std::make_shared<CollectionElementHolder>(collection, it, collection.valueType(), name,
			parentProperty, definitionManager);
		bool lookFurther = it != end && *pathPosition != 0;
		return std::make_tuple(property, lookFurther, name);
	}

	ObjectHandle handle;
	TF_ASSERT(definition != nullptr);

	if (!object.tryCast(handle))
	{
		// error: properties only applicable to objects only
		return std::make_tuple(nullptr, false, "");
	}

	static const char INDEX_OPEN = Collection::getIndexOpen();
	static const char DOT_OPERATOR = IClassDefinition::DOT_OPERATOR;

	for (; *pathPosition && *pathPosition != INDEX_OPEN && *pathPosition != DOT_OPERATOR; ++pathPosition)
		;

	auto name = path;
	auto length = pathPosition - name;
	auto property = definition->findProperty(name, length);
	bool anythingLeft = *pathPosition != 0;
	return std::make_tuple(property, anythingLeft, std::string(name, length));
}
}
} // end namespace wgt
