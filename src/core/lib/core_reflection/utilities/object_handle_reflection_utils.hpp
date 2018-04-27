#ifndef OBJECT_HANDLE_STORAGE_REFLECTED_CAST_HPP
#define OBJECT_HANDLE_STORAGE_REFLECTED_CAST_HPP

#include "core_object/i_object_handle_storage.hpp"
#include "core_reflection/reflection_dll.hpp"
#include "core_reflection/object/object_handle.hpp"

namespace wgt
{

class IDefinitionManager;
class TypeId;

class ObjectHandleReflectedUtils
{
public:
	REFLECTION_DLL static void* cast(
		void* source, const TypeId& typeIdSource, const TypeId& typeIdDest, 
		const IDefinitionManager& definitionManager);
	REFLECTION_DLL static ObjectHandle cast(
		const ObjectHandle& source, const TypeId& typeIdDest, 
		const IDefinitionManager& definitionManager);
	REFLECTION_DLL static ObjectHandle root(
		const ObjectHandle& source, 
		const IDefinitionManager& definitionManager);
};

/**
* Returns source cast to the destination type
* @note requires typeIdDest to have a registered definition and be a reflected type
*/
REFLECTION_DLL void* reflectedCast(
	void* source, const TypeId& typeIdSource, const TypeId& typeIdDest, 
	const IDefinitionManager& definitionManager);

/**
* Returns an object handle cast to the destination type
* @note requires typeIdDest to have a registered definition and be a reflected type
*/
REFLECTION_DLL ObjectHandle reflectedCast(
	const ObjectHandle& other, const TypeId& typeIdDest,
	const IDefinitionManager& definitionManager);

/**
* Returns the original storage from source stripping out any reflected casts
*/
REFLECTION_DLL ObjectHandle reflectedRoot(
	const ObjectHandle& source, const IDefinitionManager& definitionManager);

/**
* Returns recursive hash combined with class definition hash
*/
REFLECTION_DLL uint64_t reflectedHash(
	const ObjectHandle& source, const IDefinitionManager& definitionManager);

/**
* Returns an object handle cast to T
* @note requires T to have a registered definition and be a reflected type
*/
template <typename T>
ObjectHandleT<T> reflectedCast(const ObjectHandle& other, const IDefinitionManager& definitionManager)
{
	return reinterpretCast<T>(reflectedCast(other, TypeId::getType<T>(), definitionManager));
}

/**
* Returns source cast to T
* @note requires T to have a registered definition and be a reflected type
*/
template <typename T>
T* reflectedCast(void* source, const TypeId& typeIdSource, const IDefinitionManager& definitionManager)
{
	return reinterpret_cast<T*>(reflectedCast(source, typeIdSource, TypeId::getType<T>(), definitionManager));
}

} // end namespace wgt
#endif // OBJECT_HANDLE_STORAGE_HPP
