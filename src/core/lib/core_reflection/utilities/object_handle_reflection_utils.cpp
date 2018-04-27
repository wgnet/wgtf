#include "object_handle_reflection_utils.hpp"

#include "core_common/assert.hpp"
#include "core_object/object_handle_cast_utils.hpp"
#include "core_reflection/i_definition_manager.hpp"

namespace wgt
{

//------------------------------------------------------------------------------
void* ObjectHandleReflectedUtils::cast(
	void* source, const TypeId& typeIdSource, const TypeId& typeIdDest, 
	const IDefinitionManager& definitionManager)
{
	char* pRaw = static_cast<char*>(source);
	if (pRaw == nullptr)
	{
		return pRaw;
	}

	if (typeIdSource == typeIdDest)
	{
		return pRaw;
	}

	auto srcDefinition = definitionManager.getDefinition(typeIdSource.getName());
	if (srcDefinition != nullptr)
	{
		auto dstDefinition = definitionManager.getDefinition(typeIdDest.getName());
		return dstDefinition != nullptr ? srcDefinition->castTo(*dstDefinition, pRaw) : nullptr;
	}

	return nullptr;
}

//------------------------------------------------------------------------------
ObjectHandle ObjectHandleReflectedUtils::cast(
	const ObjectHandle& source, const TypeId& typeIdDest, 
	const IDefinitionManager& definitionManager)
{
    if (!source.isValid())
    {
        return source;
    }

    const TypeId& typeIdSource = source.type();
    if (typeIdSource == typeIdDest)
    {
        return source;
    }

    auto srcDefinition = definitionManager.getDefinition(typeIdSource.getName());
    if (srcDefinition != nullptr)
    {
        auto dstDefinition = definitionManager.getDefinition(typeIdDest.getName());
        if (dstDefinition != nullptr && srcDefinition->canBeCastTo(*dstDefinition))
        {
			CastType castType;
			castType.type = typeIdDest;
			castType.cast = [&definitionManager, typeIdDest](void* data, const TypeId& dataType) -> void*
			{
				return ObjectHandleReflectedUtils::cast(data, dataType, typeIdDest, definitionManager);
			};
			return ObjectHandle(source.storage(), castType);
        }
    }
    return nullptr;
}


//------------------------------------------------------------------------------
ObjectHandle ObjectHandleReflectedUtils::root(
	const ObjectHandle& source, const IDefinitionManager& definitionManager)
{
	return ObjectHandle(source.storage());
}


//------------------------------------------------------------------------------
void* reflectedCast(void* source, const TypeId& typeIdSource, const TypeId& typeIdDest, const IDefinitionManager& definitionManager)
{
	return ObjectHandleReflectedUtils::cast(source, typeIdSource, typeIdDest, definitionManager);
}


//------------------------------------------------------------------------------
ObjectHandle reflectedCast(const ObjectHandle& other, const TypeId& typeIdDest, const IDefinitionManager& definitionManager)
{
	return ObjectHandleReflectedUtils::cast(other, typeIdDest, definitionManager);
}


//------------------------------------------------------------------------------
ObjectHandle reflectedRoot(const ObjectHandle& source, const IDefinitionManager& definitionManager)
{
	return ObjectHandleReflectedUtils::root(source, definitionManager);
}

//------------------------------------------------------------------------------
uint64_t reflectedHash(const ObjectHandle& source, const IDefinitionManager& definitionManager)
{
	auto hash = source.getRecursiveHash();

	auto handle = reflectedRoot(source, definitionManager);
	if (!handle.isValid())
	{
		return hash;
	}

	if (auto definition = definitionManager.getDefinition(handle))
	{
		HashUtilities::combine(hash, HashUtilities::compute(definition->getName()));
	}

	return hash;
}

} // end namespace wgt

