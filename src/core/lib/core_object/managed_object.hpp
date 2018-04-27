#pragma once

#include "core_common/assert.hpp"
#include "core_reflection/managed_object_base.hpp"
#include "core_reflection/type_class_definition.hpp"
#include "core_object/object_reference.hpp"

namespace wgt
{
/**
* Use an ManagedObject when a model or property needs to be exposed to QML.
* Also use when working with reflection containers or reflected objects.
* Details: Search for NGT Reflection System and Object Handle in Confluence
**/
template<class T>
class ManagedObject : public ManagedObjectBase
{
public:
	ManagedObject(const std::nullptr_t& = nullptr);
	ManagedObject(const T& value, const RefObjectId& id = RefObjectId::zero());
	ManagedObject(const T* value, const RefObjectId& id = RefObjectId::zero());
	ManagedObject(std::unique_ptr<T>&& value, const RefObjectId& id = RefObjectId::zero());
	ManagedObject(std::shared_ptr<T> value, const RefObjectId& id = RefObjectId::zero());
	ManagedObject(const ObjectHandleStoragePtr& storage, const RefObjectId& id = RefObjectId::zero());
	ManagedObject(ManagedObject&& rhs);
	template<class U>
	ManagedObject(ManagedObject<U>&& rhs);
	ManagedObject& operator=(ManagedObject&& rhs);
	ManagedObject& operator=(const std::nullptr_t&);
	bool operator!=(const std::nullptr_t&) const;
	bool operator==(const std::nullptr_t&) const;
    bool operator==(const ManagedObject&) const;
	virtual ~ManagedObject();

	TypeId type() const override
	{
		static const TypeId thisType = TypeId::getType<T>();
		return thisType;
	}

	ObjectHandleT<T> getHandleT() const;

	T* operator->() const
	{
		return getPointer<T>();
	}

    T& operator*() const
    {
        auto ptr = getPointer<T>();
        TF_ASSERT(ptr);
        return *ptr;
    }

	template<class CastType = T>
	CastType* getPointer() const
	{
		static const TypeId castType = TypeId::getType<CastType>();
		if (castType != type())
		{
			return nullptr;
		}
		return static_cast<T*>(getStoragePointer());
	}


    /**
    * Create an object registered with the Object Manager
    * Usage: ManagedObject<T>::make_iunique(callback)
    * @note type T must be have a registered definition
    * @return an untyped ManagedObject as a std::unique_ptr
    */
    static ManagedObjectPtr make_iunique_fn(std::function<void(T&)> onCreate)
    {
        auto obj = ManagedObject<T>::make_unique();
        if (onCreate)
        {
            onCreate(*(*obj));
        }
        return obj;
    }

    /**
    * Create an object registered with the Object Manager
    * Usage: ManagedObject<T>::make_iunique(arg1, arg2...)
    * @note type T must be have a registered definition
    * @return an untyped ManagedObject as a std::unique_ptr
    */
    template <typename... Args>
    static ManagedObjectPtr make_iunique(Args&&... args)
    {
        return ManagedObject<T>::make_unique(std::forward<Args>(args)...);
    }

    /**
    * Create an object registered with the Object Manager
    * Usage: ManagedObject<T>::make_unique(arg1, arg2...)
    * @note type T must be have a registered definition
    * @return a typed ManagedObject as a std::unique_ptr
    */
    template <typename... Args>
    static std::unique_ptr<ManagedObject<T>> make_unique(Args&&... args)
    {
        auto storage = TypeClassDefinition<T>::createObjectStorage(std::forward<Args>(args)...);
        return std::make_unique<ManagedObject<T>>(storage);
    }

    /**
    * Create an object registered with the Object Manager
    * Usage: ManagedObject<T>::make(arg1, arg2...)
    * @note type T must be have a registered definition
    * @return a typed ManagedObject
    */
	template <typename... Args>
	static ManagedObject<T> make(Args&&... args)
	{
        auto storage = TypeClassDefinition<T>::createObjectStorage(std::forward<Args>(args)...);
		return ManagedObject<T>(storage);
	}

	/**
	* Create an object registered with the Object Manager with the specified id
	* Usage: ManagedObject<T>::make(id, arg1, arg2...)
	* @note type T must be have a registered definition
	* @return a typed ManagedObject
	*/
	template <typename... Args>
	static ManagedObject<T> make_id(const RefObjectId& id, Args&&... args)
	{
		auto storage = TypeClassDefinition<T>::createObjectStorage(std::forward<Args>(args)...);
		return ManagedObject<T>(storage, id);
	}

private:
	template<class> friend class ManagedObject;
	ManagedObject(const ManagedObject& rhs) = delete;
	ManagedObject& operator=(const ManagedObject& rhs) = delete;
};

template<class T>
ManagedObject<T>::ManagedObject(const std::nullptr_t& = nullptr)
{
}

template<class T>
ManagedObject<T>::ManagedObject(const T& value, const RefObjectId& id)
	: ManagedObject(std::static_pointer_cast<IObjectHandleStorage>(std::make_shared<ObjectHandleStorage<T>>(const_cast<T&>(value))), id)
{
}

template<class T>
ManagedObject<T>::ManagedObject(const T* value, const RefObjectId& id)
	: ManagedObject(std::static_pointer_cast<IObjectHandleStorage>(value ? std::make_shared<ObjectHandleStorage<T*>>(const_cast<T*>(value)) : nullptr), id)
{
}

template<class T>
ManagedObject<T>::ManagedObject(std::unique_ptr<T>&& value, const RefObjectId& id)
	: ManagedObject(std::static_pointer_cast<IObjectHandleStorage>(std::make_shared<ObjectHandleStorage<std::unique_ptr<T>>>(std::move(value))), id)
{
}

template<class T>
ManagedObject<T>::ManagedObject(const ObjectHandleStoragePtr& storage, const RefObjectId& id)
	: ManagedObjectBase(storage, id)
{
	TF_ASSERT(type() == storage->type());
}

template<class T>
ManagedObject<T>::ManagedObject(std::shared_ptr<T> value, const RefObjectId& id)
	: ManagedObject(std::static_pointer_cast<IObjectHandleStorage>(std::make_shared<ObjectHandleStorage<std::shared_ptr<T>>>(value)), id)
{
}

template<class T>
ManagedObject<T>::ManagedObject(ManagedObject&& rhs)
	: ManagedObjectBase(std::move(rhs))
{
}

template<class T>
template<class U>
ManagedObject<T>::ManagedObject(ManagedObject<U>&& rhs)
	: ManagedObjectBase(std::move(rhs))
{
	static_assert(std::is_base_of_v<T, U>, "Type does not derive from base type");
}

template<class T>
ManagedObject<T>& ManagedObject<T>::operator=(ManagedObject&& rhs)
{
	ManagedObjectBase::operator=(std::move(rhs));
	return *this;
}

template<class T>
ManagedObject<T>& ManagedObject<T>::operator=(const std::nullptr_t& nullPtr)
{
	ManagedObjectBase::operator=(nullPtr);
	return *this;
}

template<class T>
bool ManagedObject<T>::operator==(const ManagedObject& obj) const
{
	return ManagedObjectBase::operator==(obj);
}

template<class T>
bool ManagedObject<T>::operator==(const std::nullptr_t& nullPtr) const
{
	return ManagedObjectBase::operator==(nullPtr);
}

template<class T>
bool ManagedObject<T>::operator!=(const std::nullptr_t& nullPtr) const
{
	return ManagedObjectBase::operator!=(nullPtr);
}

template<class T>
ManagedObject<T>::~ManagedObject()
{
}

template<class T>
ObjectHandleT<T> ManagedObject<T>::getHandleT() const
{
	return reference_ ? ObjectReference::asHandleT<T>(std::static_pointer_cast<ObjectReference>(reference_)) : nullptr;
}

} // end namespace wgt
