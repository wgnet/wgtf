#ifndef OBJECT_HANDLE_HPP
#define OBJECT_HANDLE_HPP

#define DEPRECATE_OBJECT_HANDLE 0
#if DEPRECATE_OBJECT_HANDLE
#define DEPRECATE_OBJECT_HANDLE_MSG(format, ...)        \
	{                                                   \
		char buffer[1024];                              \
		sprintf_s(buffer, 1024, format, ##__VA_ARGS__); \
		wchar_t msg[1024];                              \
		mbstowcs(msg, buffer, 1024);                    \
		_wassert(msg, _CRT_WIDE(__FILE__), __LINE__);   \
	\
}
#define DEPRECATE_OBJECT_HANDLE_FUNC __declspec(deprecated)
#else
#define DEPRECATE_OBJECT_HANDLE_MSG(format, ...) \
	{                                            \
		NGT_TRACE_MSG(format, ##__VA_ARGS__);    \
	\
}
#define DEPRECATE_OBJECT_HANDLE_FUNC
#endif

/*
An ObjectHandle contains a data type and its associated ClassDefintion.
ObjectHandles store reflection data at runtime and are flexible enough to store
most types, including references, pointers and structures.
Use an ObjectHandle when a model or property needs to be exposed to QML.
Also use when working with reflection containers or reflected objects.
Details: Search for NGT Reflection System on the Wargaming Confluence
*/

#include "reflection_dll.hpp"

#include "object_handle_storage.hpp"
#include "core_common/strong_type.hpp"
#include "core_logging/logging.hpp"
#include "core_variant/type_id.hpp"
#include "core_variant/variant.hpp"
#include "interfaces/i_class_definition_details.hpp"
#include "interfaces/i_class_definition.hpp"
#include <cassert>
#include <memory>
#include <map>
#include <unordered_map>

namespace wgt
{
template <typename T>
class ObjectHandleT;

class TextStream;
class BinaryStream;
class Variant;

//==============================================================================
class REFLECTION_DLL ObjectHandle
{
public:
	ObjectHandle();
	ObjectHandle(const ObjectHandle& other);
	ObjectHandle(ObjectHandle&& other);
	ObjectHandle(const std::shared_ptr<IObjectHandleStorage>& storage);
	ObjectHandle(const std::nullptr_t&);
	ObjectHandle(const Variant& variant, const IClassDefinition* definition);
	ObjectHandle(Variant* variant, const IClassDefinition* definition);
	ObjectHandle(std::shared_ptr<void> data, TypeId type, DataGetter getter = nullptr);

	//--------------------------------------------------------------------------
	template <typename T>
	ObjectHandle(const ObjectHandleT<T>& other);

	//--------------------------------------------------------------------------
	template <typename T>
	/*DEPRECATE_OBJECT_HANDLE_FUNC*/ ObjectHandle(const T& value, const IClassDefinition* definition = nullptr)
	    : storage_(new ObjectHandleStorage<T>(const_cast<T&>(value), definition))
	{
	}

	//--------------------------------------------------------------------------
	template <typename T>
	/*DEPRECATE_OBJECT_HANDLE_FUNC*/ ObjectHandle(std::unique_ptr<T>&& value,
	                                              const IClassDefinition* definition = nullptr)
	    : storage_(new ObjectHandleStorage<std::unique_ptr<T>>(std::move(value), definition))
	{
	}

	//--------------------------------------------------------------------------
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

	void* data() const;
	TypeId type() const;
	bool isValid() const;
	std::shared_ptr<IObjectHandleStorage> storage() const;

	/**
	 *	Get the class definition for this instance.
	 *	If it is a generic object, different instances may have different
	 *	definitions.
	 *	e.g. if a property has been added at runtime to an object, it will
	 *	have a different definition to before.
	 */
	const IClassDefinition* getDefinition(const IDefinitionManager& definitionManager) const;
	bool getId(RefObjectId& o_Id) const;
	bool operator==(const ObjectHandle& other) const;
	bool operator!=(const ObjectHandle& other) const;
	ObjectHandle& operator=(const std::nullptr_t&);
	ObjectHandle& operator=(const ObjectHandle& other);
	ObjectHandle& operator=(ObjectHandle&& other);
	ObjectHandle& operator=(const std::shared_ptr<IObjectHandleStorage>& storage);
	bool operator<(const ObjectHandle& other) const;

	//--------------------------------------------------------------------------
	template <typename T>
	/*DEPRECATE_OBJECT_HANDLE_FUNC*/ ObjectHandle& operator=(const T& value)
	{
		static_assert(std::is_copy_constructible<T>::value,
		              "Type is not copy constructable, try using std::move(value)");
		storage_.reset(new ObjectHandleStorage<T>(const_cast<T&>(value)));
		return *this;
	}

	//--------------------------------------------------------------------------
	template <typename T>
	/*DEPRECATE_OBJECT_HANDLE_FUNC*/ ObjectHandle& operator=(std::unique_ptr<T>&& value)
	{
		storage_.reset(new ObjectHandleStorage<std::unique_ptr<T>>(std::move(value)));
		return *this;
	}

	template <typename T>
	ObjectHandle& operator=(const ObjectHandleT<T>& other);

private:
	std::shared_ptr<IObjectHandleStorage> storage_;
};

//==============================================================================
template <typename T>
class ObjectHandleT
{
public:
	//--------------------------------------------------------------------------
	ObjectHandleT() : storage_(nullptr)
	{
	}

	//--------------------------------------------------------------------------
	ObjectHandleT(const ObjectHandleT& other) : storage_(other.storage_)
	{
	}

	//--------------------------------------------------------------------------
	ObjectHandleT(const T& value, const IClassDefinition* definition = nullptr)
	    : storage_(new ObjectHandleStorage<T>(const_cast<T&>(value), definition))
	{
	}

	//--------------------------------------------------------------------------
	ObjectHandleT(const T* value, const IClassDefinition* definition = nullptr)
	    : storage_(value ? new ObjectHandleStorage<T*>(const_cast<T*>(value), definition) : nullptr)
	{
	}

	//--------------------------------------------------------------------------
	ObjectHandleT(std::unique_ptr<T>&& value, const IClassDefinition* definition = nullptr)
	    : storage_(new ObjectHandleStorage<std::unique_ptr<T>>(std::move(value), definition))
	{
	}

	//--------------------------------------------------------------------------
	ObjectHandleT(std::shared_ptr<T> value, const IClassDefinition* definition = nullptr)
	    : storage_(new ObjectHandleStorage<std::shared_ptr<T>>(value, definition))
	{
	}

	//--------------------------------------------------------------------------
	template <typename T2>
	ObjectHandleT(const ObjectHandleT<T2>& other);

	//--------------------------------------------------------------------------
	T* get() const
	{
		auto handle = ObjectHandle(*this);
		return handle.getBase<T>();
	}

	//--------------------------------------------------------------------------
	const IClassDefinition* getDefinition(IDefinitionManager& definitionManager) const
	{
		auto handle = ObjectHandle(*this);
		return handle.getDefinition(definitionManager);
	}

	//--------------------------------------------------------------------------
	bool getId(RefObjectId& o_Id) const
	{
		return storage_ != nullptr ? storage_->getId(o_Id) : false;
	}

	//--------------------------------------------------------------------------
	T& operator*() const
	{
		auto pObject = get();
		assert(pObject != nullptr);
		return *pObject;
	}

	//--------------------------------------------------------------------------
	T* operator->() const
	{
		return get();
	}

	//--------------------------------------------------------------------------
	bool operator==(const void* p) const
	{
		return get() == p;
	}

	//--------------------------------------------------------------------------
	bool operator!=(const void* p) const
	{
		return !operator==(p);
	}

	//--------------------------------------------------------------------------
	bool operator==(const ObjectHandle& other) const
	{
		return ObjectHandle(*this) == other;
	}

	//--------------------------------------------------------------------------
	bool operator!=(const ObjectHandle& other) const
	{
		return ObjectHandle(*this) != other;
	}

	//--------------------------------------------------------------------------
	bool operator<(const ObjectHandle& other) const
	{
		return ObjectHandle(*this) < other;
	}

private:
	//--------------------------------------------------------------------------
	ObjectHandleT(const std::shared_ptr<IObjectHandleStorage>& storage) : storage_(storage)
	{
	}

	std::shared_ptr<IObjectHandleStorage> storage_;

	friend class ObjectHandle;
	template <typename T1>
	friend ObjectHandleT<T1> reinterpretCast(const ObjectHandle& other);
};

template <typename T>
ObjectHandleT<T> reinterpretCast(const ObjectHandle& other)
{
	return ObjectHandleT<T>(other.storage());
}

template <typename T>
ObjectHandleT<T> safeCast(const ObjectHandle& other)
{
	if (other.type() == TypeId::getType<T>())
	{
		return reinterpretCast<T>(other);
	}
	if (other == nullptr)
	{
		return ObjectHandleT<T>();
	}
	assert(false);
	return ObjectHandleT<T>();
}

template <typename T1, typename T2>
ObjectHandleT<T1> staticCast(const ObjectHandleT<T2>& other)
{
	std::shared_ptr<IObjectHandleStorage> storage =
	std::make_shared<ObjectHandleStorageStaticCast<T1, T2>>(ObjectHandle(other).storage());
	return reinterpretCast<T1>(storage);
}

// TODO: Move out of object_handle
template <typename T>
ObjectHandleT<T> reflectedCast(const ObjectHandle& other, const IDefinitionManager& definitionManager)
{
	std::shared_ptr<IObjectHandleStorage> storage =
	std::make_shared<ObjectHandleStorageReflectedCast>(other.storage(), TypeId::getType<T>(), definitionManager);
	return reinterpretCast<T>(storage);
}

REFLECTION_DLL ObjectHandle reflectedCast(const ObjectHandle& other, const TypeId& typeIdDest,
                                          const IDefinitionManager& definitionManager);

template <typename T>
T* reflectedCast(void* source, const TypeId& typeIdSource, const IDefinitionManager& definitionManager)
{
	return reinterpret_cast<T*>(reflectedCast(source, typeIdSource, TypeId::getType<T>(), definitionManager));
}

REFLECTION_DLL void* reflectedCast(void* source, const TypeId& typeIdSource, const TypeId& typeIdDest,
                                   const IDefinitionManager& definitionManager);

REFLECTION_DLL ObjectHandle reflectedRoot(const ObjectHandle& source, const IDefinitionManager& defintionManager);

template <typename T>
ObjectHandle::ObjectHandle(const ObjectHandleT<T>& other) : storage_(other.storage_)
{
}

template <typename T>
ObjectHandle& ObjectHandle::operator=(const ObjectHandleT<T>& other)
{
	storage_ = other.storage_;
	return *this;
}

template <typename T1>
template <typename T2>
ObjectHandleT<T1>::ObjectHandleT(const ObjectHandleT<T2>& other)
{
	*this = staticCast<T1>(other);
}

REFLECTION_DLL TextStream& operator<<(TextStream& stream, StrongType<const ObjectHandle&> value);
REFLECTION_DLL TextStream& operator>>(TextStream& stream, ObjectHandle& value);

REFLECTION_DLL BinaryStream& operator<<(BinaryStream& stream, StrongType<const ObjectHandle&> value);
REFLECTION_DLL BinaryStream& operator>>(BinaryStream& stream, ObjectHandle& value);

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

} // end namespace wgt

META_TYPE_NAME(wgt::ObjectHandle, "object")

#endif // OBJECT_HANDLE_HPP
