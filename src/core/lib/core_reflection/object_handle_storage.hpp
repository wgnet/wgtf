#ifndef OBJECT_HANDLE_STORAGE_HPP
#define OBJECT_HANDLE_STORAGE_HPP

#include "core_variant/type_id.hpp"
#include "interfaces/i_class_definition.hpp"
#include "reflection_dll.hpp"
#include <cassert>
#include <memory>
#include <unordered_set>
#include <type_traits>
#include <typeinfo>

namespace wgt
{
class RefObjectId;
class IParticleEffectTreeModel;

class REFLECTION_DLL IObjectHandleStorage
{
public:
	virtual ~IObjectHandleStorage()
	{
	}
	virtual void* data() const = 0;
	virtual TypeId type() const = 0;

	virtual std::shared_ptr<IObjectHandleStorage> inner() const
	{
		return nullptr;
	}

	virtual bool getId(RefObjectId& id) const = 0;
};

//==============================================================================
class REFLECTION_DLL ObjectHandleStorageVoid : public IObjectHandleStorage
{
public:
	ObjectHandleStorageVoid(std::shared_ptr<void> data, TypeId type, DataGetter getter)
	    : data_(data), type_(type), getter_(getter)
	{
	}

	void* data() const
	{
		if (getter_)
			return getter_();
		return data_.get();
	}

	TypeId type() const
	{
		return type_;
	}

	bool getId(RefObjectId& /*id*/) const
	{
		return false;
	}

	void* storedData() const
	{
		return data_.get();
	}

private:
	std::shared_ptr<void> data_;
	TypeId type_;
	DataGetter getter_;
};

//==============================================================================
template <typename T>
class ObjectHandleStorageBase : public IObjectHandleStorage
{
public:
	ObjectHandleStorageBase(const IClassDefinition* definition = nullptr) : definition_(definition)
	{
	}

	virtual T* getPointer() const = 0;

	void* data() const override
	{
		const void* data = getPointer();
		return const_cast<void*>(data);
	}

	TypeId type() const override
	{
		static TypeId s_Type = TypeId::getType<T>();
		return s_Type;
	}

	bool getId(RefObjectId& id) const override
	{
		return false;
	}

private:
	const IClassDefinition* definition_;
};

//==============================================================================
template <typename T>
class ObjectHandleStoragePtr : public ObjectHandleStorageBase<T>
{
public:
	ObjectHandleStoragePtr(T* pointer, const IClassDefinition* definition)
	    : ObjectHandleStorageBase<T>(definition), pointer_(pointer)
	{
	}

	T* getPointer() const override
	{
		return pointer_;
	}

private:
	T* pointer_;
};

//==============================================================================
template <typename T>
class ObjectHandleStorage : public ObjectHandleStoragePtr<T>
{
public:
	ObjectHandleStorage(T& temp, const IClassDefinition* definition)
	    : ObjectHandleStoragePtr<T>(&storage_, definition), storage_(temp)
	{
	}

private:
	T storage_;
};

//==============================================================================
template <typename T>
class ObjectHandleStorage<T*> : public ObjectHandleStoragePtr<T>
{
public:
	ObjectHandleStorage(T* pointer, const IClassDefinition* definition) : ObjectHandleStoragePtr<T>(pointer, definition)
	{
	}
};

//==============================================================================
template <typename T>
class ObjectHandleStorage<std::unique_ptr<T>> : public ObjectHandleStoragePtr<T>
{
public:
	ObjectHandleStorage(std::unique_ptr<T>&& pointer, const IClassDefinition* definition = nullptr)
	    : ObjectHandleStoragePtr<T>(pointer.get(), definition), pointer_(std::move(pointer))
	{
	}

private:
	const std::unique_ptr<T> pointer_;
};

//==============================================================================
template <typename T>
class ObjectHandleStorage<std::shared_ptr<T>> : public ObjectHandleStoragePtr<T>
{
public:
	ObjectHandleStorage(std::shared_ptr<T>& pointer, const IClassDefinition* definition = nullptr)
	    : ObjectHandleStoragePtr<T>(pointer.get(), definition), pointer_(pointer)
	{
	}

private:
	const std::shared_ptr<T> pointer_;
};

//==============================================================================
template <typename T, typename T2>
class ObjectHandleStorageStaticCast : public ObjectHandleStorageBase<T>
{
public:
	ObjectHandleStorageStaticCast(const std::shared_ptr<IObjectHandleStorage>& storage) : storage_(storage)
	{
	}

	T* getPointer() const override
	{
		if (storage_ == nullptr)
		{
			return nullptr;
		}

		return reinterpret_cast<T2*>(storage_->data());
	}

	std::shared_ptr<IObjectHandleStorage> inner() const override
	{
		return storage_;
	}

	bool getId(RefObjectId& id) const override
	{
		if (storage_ == nullptr)
		{
			return false;
		}

		return storage_->getId(id);
	}

private:
	std::shared_ptr<IObjectHandleStorage> storage_;
};

//==============================================================================
class REFLECTION_DLL ObjectHandleStorageReflectedCast : public IObjectHandleStorage
{
public:
	ObjectHandleStorageReflectedCast(const std::shared_ptr<IObjectHandleStorage>& storage, const TypeId& typeId,
	                                 const IDefinitionManager& definitionManager);

	void* data() const override;
	TypeId type() const override;

	std::shared_ptr<IObjectHandleStorage> inner() const override;

	bool getId(RefObjectId& id) const override;

private:
	std::shared_ptr<IObjectHandleStorage> storage_;
	TypeId typeId_;
	const IDefinitionManager& definitionManager_;
};
} // end namespace wgt
#endif // OBJECT_HANDLE_STORAGE_HPP
