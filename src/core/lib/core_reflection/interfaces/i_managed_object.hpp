#ifndef I_MANAGED_OBJECT_HPP
#define I_MANAGED_OBJECT_HPP

#pragma once

#include "core_reflection/object/object_handle.hpp"
#include "core_reflection/reflection_dll.hpp"
#include "core_common/wg_hash.hpp"
#include "core_reflection/object/object_handle.hpp"
#include <memory>

namespace wgt
{
class RootObjectReference;
class ObjectStorage;
class ObjectHandle;
class IDefinitionManager;

class REFLECTION_DLL IManagedObject 
{
public:
	virtual ~IManagedObject(){}
	virtual ObjectHandle getHandle() const = 0;
    virtual Variant getVariant() const = 0;

    /**
    * @see object_handle_reflection_utils for more information
    */
	template<typename T>
	ObjectHandleT<T> getHandleT() const
	{
		return reflectedCast<T>(getHandle(), *getDefinitionManager());
	}

	virtual TypeId type() const = 0;
	virtual bool set(std::shared_ptr<RootObjectReference>&& reference, std::shared_ptr<ObjectStorage>&& storage) = 0;

	virtual const IDefinitionManager * getDefinitionManager() const = 0;
};

typedef std::unique_ptr<IManagedObject> ManagedObjectPtr;

} // end namespace wgt

namespace wgt
{
template <>
struct hash<wgt::IManagedObject>
	: public std::unary_function<wgt::IManagedObject, size_t>
{
public:
	size_t operator()(const wgt::IManagedObject & v) const
	{
		wgt::hash<wgt::ObjectHandle> hash_fn;
		return hash_fn(v.getHandle());
	}
}; 
}

#endif //I_MANAGED_OBJECT_HPP