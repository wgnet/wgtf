#ifndef OBJECT_HANDLE_HPP
#define OBJECT_HANDLE_HPP

#include "core_common/assert.hpp"
#include "core_variant/type_id.hpp"
#include "core_variant/variant.hpp"
#include "core_reflection/reflection_dll.hpp"
#include "core_common/signal.hpp"
#include "core_common/wg_hash.hpp"

namespace wgt
{

class ObjectHandle;
template <typename T> class ObjectHandleT;
typedef std::shared_ptr<class IObjectHandleStorage> ObjectHandleStoragePtr;
class RefObjectId;

struct CastType
{
	CastType(const std::nullptr_t& = nullptr) {}

	std::function<void*(void*, const TypeId&)> cast = nullptr;
	TypeId type = nullptr;
};

class REFLECTION_DLL ObjectHandleBase
{
public:
	RefObjectId id() const;
	TypeId type() const;
	ObjectHandle parent() const;
	std::string path() const;
	void setParent(const ObjectHandle& parent, const std::string& path);
	ObjectHandleStoragePtr storage() const;
	uint64_t getRecursiveHash() const;

	typedef std::function<void(void)> StorageDestroyedCallback;
	Connection connectStorageDestroyed(StorageDestroyedCallback cb) const;

protected:
	ObjectHandleBase();
	ObjectHandleBase(const ObjectHandleStoragePtr& storage, CastType castType);
	ObjectHandleBase(const ObjectHandleBase& other);
	ObjectHandleBase(ObjectHandleBase&& other);

	bool operator==(const ObjectHandleBase & other) const;
	bool operator<( const ObjectHandleBase & other) const;
	ObjectHandleBase & operator=(const ObjectHandleBase & other);
	ObjectHandleBase & operator=(ObjectHandleBase && other);
	ObjectHandleBase & operator=(const std::nullptr_t&);
	void * getData(const TypeId & typeId) const;
	uint64_t getHash() const;

	ObjectHandleStoragePtr storage_;
	CastType castType_;
};

//==============================================================================
class REFLECTION_DLL ObjectHandle
	: public ObjectHandleBase
{
public:
	ObjectHandle();
	ObjectHandle(const std::nullptr_t&);
	ObjectHandle(const ObjectHandle& other);
	ObjectHandle(ObjectHandle&& other);

    template<typename T>
    ObjectHandle(const ObjectHandleT<T>& other);

	template <class T>
	T* getBase() const
	{
		static const TypeId s_Type = TypeId::getType<T>();
		return static_cast< T * >( ObjectHandleBase::getData(s_Type) );
	}

	void* data() const;
	bool isValid() const;

	bool operator==(const std::nullptr_t&) const;
	bool operator!=(const std::nullptr_t&) const;
	bool operator==(const ObjectHandle& other) const;
	bool operator!=(const ObjectHandle& other) const;
	bool operator<(const ObjectHandle& other) const;
	ObjectHandle& operator=(const std::nullptr_t&);
	ObjectHandle& operator=(const ObjectHandle& other);
	ObjectHandle& operator=(ObjectHandle&& other);

    template <typename T>
    ObjectHandle& operator=(const ObjectHandleT<T>& other);

private:
	ObjectHandle(const ObjectHandleStoragePtr& storage, CastType castType = nullptr);

	friend class ObjectHandleBase;
	friend class ObjectManager;
	friend class ObjectReference;
	friend class ObjectHandleReflectedUtils;
	friend class ObjectHandleCastUtils;
	template <typename T> friend class ObjectHandleStorage;
	template <typename T> friend class ManagedObject;
	friend class ManagedObjectBase;
	friend class XMLReader;
	friend class PropertyAccessor;
};

//==============================================================================
template <typename T>
class ObjectHandleT
	: public ObjectHandleBase
{
public:
	ObjectHandleT() 
        : ObjectHandleBase()
	{
	}

	ObjectHandleT(const std::nullptr_t&) 
        : ObjectHandleBase()
	{
	}

	ObjectHandleT(const ObjectHandleT& other) 
        : ObjectHandleBase(other)
	{
	}

    ObjectHandleT(const ObjectHandleT&& other)
        : ObjectHandleBase(std::move(other))
    {
    }

	ObjectHandleT<T>& operator=(const ObjectHandleT<T>& other)
	{
		ObjectHandleBase::operator=(other);
		return *this;
	}

	ObjectHandleT<T>& operator=(ObjectHandleT<T>&& other)
	{
		ObjectHandleBase::operator=( std::move( other ));
		return *this;
	}

	ObjectHandleT<T>& operator=(const std::nullptr_t&)
	{
		ObjectHandleBase::operator=(nullptr);
		return *this;
	}

	template<typename T1 = T>
	T1* get() const
	{
		static const TypeId s_Type = TypeId::getType<T1>();
		return static_cast< T1 * >( ObjectHandleBase::getData( s_Type ) );
	}

	T& operator*() const
	{
		auto pObject = get();
		TF_ASSERT(pObject != nullptr);
		return *pObject;
	}

	T* operator->() const
	{
		return get();
	}

	bool operator==(const std::nullptr_t&) const
	{
		return get() == nullptr;
	}

	bool operator!=(const std::nullptr_t&) const
	{
		return get() != nullptr;
	}

    bool operator==(const ObjectHandle& other) const
    {
        return other == *this;
    }

    bool operator!=(const ObjectHandle& other) const
    {
        return other != *this;
    }

    template<typename T1 = T>
	bool operator==(const ObjectHandleT<T1>& other) const
	{
		return ObjectHandleBase::operator==(other);
	}

    template<typename T1 = T>
	bool operator!=(const ObjectHandleT<T1>& other) const
	{
		return !operator==(other);
	}

	bool operator<(const ObjectHandleT<T>& other) const
	{
		return ObjectHandleBase::operator<(other);
	}

private:
	ObjectHandleT(const ObjectHandleStoragePtr& storage, CastType castType = nullptr)
		: ObjectHandleBase( storage, castType )
	{
        TF_ASSERT(castType_.type == nullptr || castType_.type == TypeId::getType<T>());
	}

	friend class ObjectManager;
	friend class ObjectReference;
	friend class ObjectHandleReflectedUtils;
	friend class ObjectHandleCastUtils;
	friend class ObjectHandle;
	template <typename T> friend class ObjectHandleT;
	template <typename T> friend class ObjectHandleStorage;
	template <typename T> friend class ManagedObject;
	friend class ManagedObjectBase;
	friend class XMLReader;
	friend class PropertyAccessor;
};

//==============================================================================

template<typename T>
ObjectHandle& ObjectHandle::operator=(const ObjectHandleT<T>& other)
{
	ObjectHandleBase::operator=(other);
	return *this;
}

template<typename T>
ObjectHandle::ObjectHandle(const ObjectHandleT<T>& other) 
	: ObjectHandleBase(other)
{
}


template <typename T>
ObjectHandle upcast(const ObjectHandleT<T>& v)
{
	return ObjectHandle(v);
}

template <typename T>
bool downcast(ObjectHandleT<T>* v, const ObjectHandle& storage)
{
	if (v && (storage == nullptr || storage.type() == TypeId::getType<T>()))
	{
		*v = reinterpretCast<T>(storage);
		return true;
	}
	return false;
}

//==============================================================================
} // end namespace wgt


META_TYPE_NAME(wgt::ObjectHandle, "object")
DEFINE_CUST_HASH(wgt::ObjectHandle, getRecursiveHash)

#endif // OBJECT_HANDLE_HPP
