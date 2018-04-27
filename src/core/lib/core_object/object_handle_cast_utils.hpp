#pragma once

#include "core_reflection/object_handle.hpp"
#include "wg_types/hash_utilities.hpp"

namespace wgt
{
class ObjectHandleCastUtils
{
public:
	template <typename T>
	static ObjectHandleT<T> reinterpretCast(const ObjectHandle& other)
	{
		return ObjectHandleT<T>(other.storage_, other.castType_);
	}

	template <typename T1, typename T2>
	static ObjectHandleT<T1> dynamicCast(const ObjectHandleT<T2>& other)
	{
		if (other != nullptr)
		{
			auto cast = makeCaster(other);
			CastType castType;
			castType.type = TypeId::getType<T1>();
			castType.cast = [cast](void* data, const TypeId& dataType) -> void*
			{
				return dynamic_cast<T1*>(static_cast<T2*>(cast(data, dataType)));
			};
			return ObjectHandleT<T1>(other.storage_, castType);
		}
		return nullptr;
	}

	template <typename T1, typename T2>
	static ObjectHandleT<T1> staticCast(const ObjectHandleT<T2>& other)
	{
		if (other != nullptr)
		{
			auto cast = makeCaster(other);
			CastType castType;
			castType.type = TypeId::getType<T1>();
			castType.cast = [cast](void* data, const TypeId& dataType) -> void*
			{
				return static_cast<T1*>(static_cast<T2*>(cast(data, dataType)));
			};
			return ObjectHandleT<T1>(other.storage_, castType);
		}
		return nullptr;
	}

private:
	template <typename T>
	static std::function<void*(void*, const TypeId&)> makeCaster(const ObjectHandleT<T>& object)
	{
		const auto type = TypeId::getType<T>();
		return object.castType_.type == type ? object.castType_.cast :
			[type](void* data, const TypeId& dataType) -> void*
		{
			return dataType == type ? data : nullptr;
		};
	}
};

template <typename T1, typename T2>
ObjectHandleT<T1> staticCast(const ObjectHandleT<T2>& other)
{
	return ObjectHandleCastUtils::staticCast<T1, T2>(other);
}

template <typename T1, typename T2>
ObjectHandleT<T1> dynamicCast(const ObjectHandleT<T2>& other)
{
	return ObjectHandleCastUtils::dynamicCast<T1, T2>(other);
}

template <typename T>
ObjectHandleT<T> reinterpretCast(const ObjectHandle& other)
{
    return ObjectHandleCastUtils::reinterpretCast<T>(other);
}

template <typename T>
ObjectHandleT<T> safeCast(const ObjectHandle& other)
{
    if (other.type() == TypeId::getType<T>())
    {
		return ObjectHandleCastUtils::reinterpretCast<T>(other);
    }
    return nullptr;
}

} // end namespace wgt
