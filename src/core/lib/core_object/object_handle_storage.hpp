#ifndef OBJECT_HANDLE_STORAGE_HPP
#define OBJECT_HANDLE_STORAGE_HPP

#include "core_common/assert.hpp"
#include "core_reflection/reflection_dll.hpp"
#include "core_object/i_object_handle_storage.hpp"
#include "core_object/i_object_handle_provider.hpp"
#include "core_reflection/ref_object_id.hpp"
#include <cassert>
#include <type_traits>
#include <typeinfo>
#include <functional>

namespace wgt
{
class RefObjectId;
class IClassDefinition;
class IDefinitionManager;

typedef std::function<void*()> DataGetter;

//==============================================================================
class ParentedObjectHandleStorage : public IObjectHandleStorage
{
public:
    template<typename T = void>
    ParentedObjectHandleStorage()
    {
        ENFORCE_OBJECT_HANDLE_CONTEXT
    }

	~ParentedObjectHandleStorage();

	Connection connectDestroyed(StorageDestroyedCallback cb) override;
	void setParent(const ObjectHandleStoragePtr& parent, const std::string& path) override;
	ObjectHandleStoragePtr parent() const override;
	std::string path() const override;
	const RefObjectId& id() const override;
	bool operator==(const ParentedObjectHandleStorage & other) const;
	bool operator==(const IObjectHandleStorage & other) const override;
	uint64_t getHashcode() const override;

private:
	Signal<StorageDestroyed> destroyedSignal_;
	ObjectHandleStoragePtr parent_;
	std::string path_;
};

//==============================================================================
template <typename T>
class ObjectHandleStorageBase : public ParentedObjectHandleStorage
{
public:
    ObjectHandleStorageBase()
    {
        ENFORCE_OBJECT_HANDLE_CONTEXT
    }

	virtual T* getPointer() const = 0;

	virtual void* data() const override
	{
		const void* data = getPointer();
		return const_cast<void*>(data);
	}

	virtual TypeId type() const override
	{
		static TypeId s_Type = TypeId::getType<T>();
		return s_Type;
	}

	virtual bool isValid() const override
	{
		return true;
	}

    IObjectHandleProvider* provider() const override
    {
        return getProvider();
    }

private:
    template<typename T1 = T>
    typename std::enable_if<std::is_base_of<IObjectHandleProvider, T1>::value, IObjectHandleProvider*>::type
    getProvider() const
    {
        return dynamic_cast<IObjectHandleProvider*>(getPointer());
    }

    template<typename T1 = T>
    typename std::enable_if<!std::is_base_of<IObjectHandleProvider, T1>::value, IObjectHandleProvider*>::type
    getProvider() const
    {
        return nullptr;
    }
};

//==============================================================================
template <typename T>
class ObjectHandleStoragePtrBase : public ObjectHandleStorageBase<T>
{
public:
	ObjectHandleStoragePtrBase(T* pointer)
	    : ObjectHandleStorageBase<T>()
		, pointer_(pointer)
	{
        ENFORCE_OBJECT_HANDLE_CONTEXT
	}

	T* getPointer() const override
	{
		return pointer_;
	}

	bool operator==(const IObjectHandleStorage & other) const override
	{
		if (auto pOther = dynamic_cast<const ObjectHandleStoragePtrBase<T>*>(&other))
		{
			return operator==(*pOther);
		}
		return false;
	}

	bool operator==(const ObjectHandleStoragePtrBase< T > & other) const
	{
		return pointer_ == other.pointer_;
	}


	uint64_t getHashcode() const override
	{
		return reinterpret_cast<uint64_t>(pointer_);
	}

private:
	T* pointer_;
};

//==============================================================================
template <typename T>
class ObjectHandleStorage : public ObjectHandleStoragePtrBase<T>
{
public:
	ObjectHandleStorage(T& temp)
	    : ObjectHandleStoragePtrBase<T>(&storage_)
		, storage_(temp)
	{
        ENFORCE_OBJECT_HANDLE_CONTEXT
	}

private:
	T storage_;
};

//==============================================================================
template <typename T>
class ObjectHandleStorage<T*> : public ObjectHandleStoragePtrBase<T>
{
public:
	ObjectHandleStorage(T* pointer) 
		: ObjectHandleStoragePtrBase<T>(pointer)
	{
        ENFORCE_OBJECT_HANDLE_CONTEXT
	}
};

//==============================================================================
template <typename T>
class ObjectHandleStorage<std::unique_ptr<T>> : public ObjectHandleStoragePtrBase<T>
{
public:
	ObjectHandleStorage(std::unique_ptr<T>&& pointer)
	    : ObjectHandleStoragePtrBase<T>(pointer.get())
		, pointer_(std::move(pointer))
	{
        ENFORCE_OBJECT_HANDLE_CONTEXT
	}

	bool operator==(const IObjectHandleStorage & other) const override
	{
		auto pOther = dynamic_cast<const ObjectHandleStorage< std::unique_ptr< T > > * >( &other );
		if (pOther == nullptr)
		{
			return false;
		}
		return ObjectHandleStoragePtrBase<T>::operator==(*pOther);
	}

private:
	const std::unique_ptr<T> pointer_;
};

//==============================================================================
template <typename T>
class ObjectHandleStorage<std::shared_ptr<T>> : public ObjectHandleStoragePtrBase<T>
{
public:
	ObjectHandleStorage(std::shared_ptr<T>& pointer)
	    : ObjectHandleStoragePtrBase<T>(pointer.get())
		, pointer_(pointer)
	{
        ENFORCE_OBJECT_HANDLE_CONTEXT
	}

	bool operator==(const IObjectHandleStorage & other) const override
	{
		auto pOther = dynamic_cast<const ObjectHandleStorage< std::shared_ptr< T > > *>(&other);
		if (pOther == nullptr)
		{
			return false;
		}
		return ObjectHandleStoragePtrBase<T>::operator==( *pOther );
	}

private:
	const std::shared_ptr<T> pointer_;
};

} // end namespace wgt
#endif // OBJECT_HANDLE_STORAGE_HPP
