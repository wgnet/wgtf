#ifndef OBJECT_HANDLE_HPP
#define OBJECT_HANDLE_HPP

#include "core_object/object_handle_variant_storage.hpp"
#include "core_logging/logging.hpp"
#include "core_variant/type_id.hpp"
#include "core_variant/variant.hpp"
#include "core_common/deprecated.hpp"

/**
* An ObjectHandle contains a data type and its associated ClassDefintion.
* ObjectHandles store reflection data at runtime and are flexible enough to store
* most types, including references, pointers and structures.
* Use an ObjectHandle when a model or property needs to be exposed to QML.
* Also use when working with reflection containers or reflected objects.
* Details: Search for NGT Reflection System and Object Handle in Confluence
**/
namespace wgt
{
template <typename T> class ObjectHandleT;

//==============================================================================
class ObjectHandle
{
public:
	ObjectHandle() : storage_(nullptr)
	{
	}

	ObjectHandle(const std::nullptr_t&) : storage_(nullptr)
	{
	}

	ObjectHandle(const ObjectHandle& other) : storage_(other.storage_)
	{
	}

	ObjectHandle(ObjectHandle&& other) : storage_(std::move(other.storage_))
	{
	}

	ObjectHandle(const ObjectHandleStoragePtr& storage) : storage_(storage)
	{
	}

	template<typename _dummy = void>
	ObjectHandle(const Variant& variant, const IClassDefinition* definition = nullptr)
	{
		if (initialiseFromVariant(variant))
		{
			return;
		}
		storage_ = std::make_shared<ObjectHandleVariantStorage>(variant);
	}

	template<typename _dummy = void>
	ObjectHandle(Variant* variant, const IClassDefinition* definition = nullptr)
	{
		if (initialiseFromVariant(*variant))
		{
			return;
		}
		storage_ = std::make_shared<ObjectHandleVariantStorage>(variant);
	}

	template <typename _dummy = void>
	ObjectHandle(std::shared_ptr<void> data, TypeId type, DataGetter getter = nullptr)
	    : storage_(new ObjectHandleStorageVoid(data, type, getter))
	{
	}

	template<typename T>
	ObjectHandle(const ObjectHandleT<T>& other);

	template <class T>
	T* getBase() const
	{
		if (storage_ == nullptr)
		{
			return nullptr;
		}
		static const TypeId s_Type = TypeId::getType<T>();
		if (s_Type != storage_->type())
		{
			return nullptr;
		}
		return static_cast<T*>(storage_->data());
	}

	void* data() const
	{
		return storage_ != nullptr ? storage_->data() : nullptr;
	}

	TypeId type() const
	{
		return storage_ != nullptr ? storage_->type() : nullptr;
	}

	bool isValid() const
	{
		return data() != nullptr;
	}

	ObjectHandleStoragePtr storage() const
	{
		return storage_;
	}

	ObjectHandle parent() const
	{
		return storage_ == nullptr ? ObjectHandle() : storage_->parent();
	}

	std::string path() const
	{
		return storage_ == nullptr ? "" : storage_->path();
	}

	void setParent(const ObjectHandle& parent, const std::string& path)
	{
		if (storage_ == nullptr)
		{
			return;
		}

		storage_->setParent(parent.storage(), path);
	}

	template<typename _dummy = void>
	DEPRECATED const IClassDefinition* getDefinition(const IDefinitionManager& definitionManager) const
	{
		static_assert(!std::is_same<_dummy, _dummy>::value,
		              "This method is now deprecated. Please use IDefinitionManager::getDefinition");
	}

	bool getId(RefObjectId& o_Id) const
	{
		if (storage_ == nullptr)
		{
			return false;
		}

		return storage_->getId(o_Id);
	}

	bool operator==(const ObjectHandle& other) const
	{
		if (storage_ == other.storage_)
		{
			return true;
		}

		if (storage_ == nullptr || other.storage_ == nullptr)
		{
			return false;
		}

		auto left = storage_->data();
		auto right = other.storage_->data();
		if (left == right)
		{
			if (storage_->type() == other.storage_->type())
			{
				return true;
			}
		}
		return false;
	}

	bool operator!=(const ObjectHandle& other) const
	{
		return !operator==(other);
	}

	bool operator<(const ObjectHandle& other) const
	{
		if (storage_ == other.storage_)
		{
			return false;
		}

		if (storage_ == nullptr)
		{
			return true;
		}

		if (other.storage_ == nullptr)
		{
			return false;
		}

		auto left = storage_->data();
		auto right = other.storage_->data();
		if (left == right)
		{
			return storage_->type() < other.storage_->type();
		}
		return left < right;
	}

	ObjectHandle& operator=(const std::nullptr_t&)
	{
		storage_ = nullptr;
		return *this;
	}

	ObjectHandle& operator=(const ObjectHandle& other)
	{
		storage_ = other.storage_;
		return *this;
	}

	ObjectHandle& operator=(ObjectHandle&& other)
	{
		storage_ = std::move(other.storage_);
		return *this;
	}

	ObjectHandle& operator=(const ObjectHandleStoragePtr& storage)
	{
		storage_ = storage;
		return *this;
	}

	template <typename T>
	ObjectHandle& operator=(const ObjectHandleT<T>& other);

	template <typename T>
	DEPRECATED ObjectHandle& operator=(const T& value)
	{
		static_assert(!std::is_same<T, T>::value, "This method is now deprecated. Please use ObjectHandleT.");
	}

	template <typename T>
	DEPRECATED ObjectHandle& operator=(std::unique_ptr<T>&& value)
	{
		static_assert(!std::is_same<T, T>::value, "This method is now deprecated. Please use ObjectHandleT.");
	}

	template <typename T>
	DEPRECATED ObjectHandle(const T& value, const IClassDefinition* definition = nullptr)
	{
		static_assert(!std::is_same<T, T>::value, "This method is now deprecated. Please use ObjectHandleT.");
	}

	template <typename T>
	DEPRECATED ObjectHandle(std::unique_ptr<T>&& value, const IClassDefinition* definition = nullptr)
	{
		static_assert(!std::is_same<T, T>::value, "This method is now deprecated. Please use ObjectHandleT.");
	}

private:
	bool initialiseFromVariant(const Variant& variant)
	{
		if (auto handlePtr = variant.value<ObjectHandle*>())
		{
			// avoid pointless nesting
			storage_ = handlePtr->storage_;
			return true;
		}

		auto handle = variant.value<ObjectHandle>();

		if (handle.isValid())
		{
			storage_ = handle.storage_;
			return true;
		}

		return false;
	}

	ObjectHandleStoragePtr storage_;
};

//==============================================================================
template <typename T>
class ObjectHandleT
{
public:
	ObjectHandleT() : storage_(nullptr)
	{
	}

	ObjectHandleT(const std::nullptr_t&) : storage_(nullptr)
	{
	}

	ObjectHandleT(const ObjectHandleT& other) : storage_(other.storage_)
	{
	}

	ObjectHandleT(const T& value, const IClassDefinition* definition = nullptr)
	    : storage_(new ObjectHandleStorage<T>(const_cast<T&>(value)))
	{
	}

	ObjectHandleT(const T* value, const IClassDefinition* definition = nullptr)
	    : storage_(value ? new ObjectHandleStorage<T*>(const_cast<T*>(value)) : nullptr)
	{
	}

	ObjectHandleT(std::unique_ptr<T>&& value, const IClassDefinition* definition = nullptr)
	    : storage_(new ObjectHandleStorage<std::unique_ptr<T>>(std::move(value)))
	{
	}

	ObjectHandleT(std::shared_ptr<T> value, const IClassDefinition* definition = nullptr)
	    : storage_(new ObjectHandleStorage<std::shared_ptr<T>>(value))
	{
	}

	template <typename T1 = T, typename T2>
	ObjectHandleT(const ObjectHandleT<T2>& other)
	{
		storage_ = std::make_shared<ObjectHandleStorageStaticCast<T1, T2>>(other.storage());
	}

	ObjectHandleT<T>& operator=(const ObjectHandleT<T>& other)
	{
		storage_ = other.storage_;
		return *this;
	}

	ObjectHandleT<T>& operator=(ObjectHandleT<T>&& other)
	{
		storage_ = std::move(other.storage_);
		return *this;
	}

	ObjectHandleT<T>& operator=(const std::nullptr_t&)
	{
		storage_ = nullptr;
		return *this;
	}

	template<typename T1 = T>
	T1* get() const
	{
		if (storage_ == nullptr)
		{
			return nullptr;
		}
		static const TypeId s_Type = TypeId::getType<T1>();
		if (s_Type != storage_->type())
		{
			return nullptr;
		}
		return static_cast<T*>(storage_->data());
	}

	DEPRECATED const IClassDefinition* getDefinition(const IDefinitionManager& definitionManager) const
	{
		static_assert(!std::is_same<T, T>::value,
		              "This method is now deprecated. Please use IDefinitionManager::getDefinition");
	}

	bool getId(RefObjectId& o_Id) const
	{
		return storage_ != nullptr ? storage_->getId(o_Id) : false;
	}

	T& operator*() const
	{
		auto pObject = get();
		assert(pObject != nullptr);
		return *pObject;
	}

	T* operator->() const
	{
		return get();
	}

	bool operator==(const void* p) const
	{
		return get() == p;
	}

	bool operator!=(const void* p) const
	{
		return !operator==(p);
	}

	bool operator==(const ObjectHandleT<T>& other) const
	{
		if (storage_ == other.storage_)
		{
			return true;
		}

		if (storage_ == nullptr || other.storage_ == nullptr)
		{
			return false;
		}

		auto left = storage_->data();
		auto right = other.storage_->data();
		if (left == right)
		{
			if (storage_->type() == other.storage_->type())
			{
				return true;
			}
		}
		return false;
	}

	bool operator!=(const ObjectHandleT<T>& other) const
	{
		return !operator==(other);
	}

	bool operator<(const ObjectHandleT<T>& other) const
	{
		if (storage_ == other.storage_)
		{
			return false;
		}

		if (storage_ == nullptr)
		{
			return true;
		}

		if (other.storage_ == nullptr)
		{
			return false;
		}

		auto left = storage_->data();
		auto right = other.storage_->data();
		if (left == right)
		{
			return storage_->type() < other.storage_->type();
		}
		return left < right;
	}

	ObjectHandleStoragePtr storage() const
	{
		return storage_;
	}

	ObjectHandle parent() const
	{
		return storage_ == nullptr ? ObjectHandle() : storage_->parent();
	}

	std::string path() const
	{
		return storage_ == nullptr ? "" : storage_->path();
	}

	void setParent(const ObjectHandle& parent, const std::string& path)
	{
		if (storage_ == nullptr)
		{
			return;
		}

		storage_->setParent(parent.storage(), path);
	}

private:
	ObjectHandleT(const ObjectHandleStoragePtr& storage)
		: storage_(storage)
	{
	}

	ObjectHandleStoragePtr storage_;
	friend class ObjectHandle;
	template <typename T1> friend ObjectHandleT<T1> reinterpretCast(const ObjectHandle& other);
};

//==============================================================================

template<typename T>
ObjectHandle& ObjectHandle::operator=(const ObjectHandleT<T>& other)
{
	storage_ = other.storage_;
	return *this;
}

template<typename T>
ObjectHandle::ObjectHandle(const ObjectHandleT<T>& other) 
	: storage_(other.storage_)
{
}

//==============================================================================
} // end namespace wgt

META_TYPE_NAME(wgt::ObjectHandle, "object")

#endif // OBJECT_HANDLE_HPP
