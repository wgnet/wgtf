#ifndef REFLECTED_PROPERTY_HPP
#define REFLECTED_PROPERTY_HPP

#include "base_property.hpp"
#include "utilities/reflection_utilities.hpp"

#include "core_variant/collection.hpp"
#include "core_variant/variant.hpp"

namespace wgt
{
template <typename TargetType, typename BaseType>
class ReflectedProperty : public BaseProperty
{
public:
	typedef ReflectedProperty<TargetType, BaseType> SelfType;
	typedef TargetType BaseType::*member_ptr;

	explicit ReflectedProperty(const char* name, member_ptr memPtr, const TypeId& type)
	    : BaseProperty(name, type), memberPtr_(memPtr)
	{
	}

	bool isValue() const override
	{
		return (memberPtr_ != nullptr);
	}

	//==========================================================================
	Variant get(const ObjectHandle& pBase, const IDefinitionManager& definitionManager) const override
	{
		assert(this->isValue());
		auto pObject = reflectedCast<BaseType>(pBase.data(), pBase.type(), definitionManager);
		if (pObject && memberPtr_)
		{
			return ReflectionUtilities::reference((pObject->*memberPtr_));
		}
		else
		{
			return Variant();
		}
	}

	bool readOnly() const override
	{
		return can_set_Value<std::is_same<TargetType, Variant>::value>::readOnly(memberPtr_);
	}

	//==========================================================================
	bool set(const ObjectHandle& pBase, const Variant& value,
	         const IDefinitionManager& definitionManager) const override
	{
		// assert( !this->readOnly() );
		return set_Value<std::is_same<TargetType, Variant>::value>::set(pBase, memberPtr_, value, definitionManager);
	}

private:
	member_ptr memberPtr_;

	template <bool is_Variant, typename _dummy = void>
	struct can_set_Value
	{
		static bool readOnly(member_ptr memberPtr)
		{
			return (memberPtr == nullptr);
		}
	};

	template <bool is_Variant, typename _dummy = void>
	struct set_Value
	{
		static bool set(const ObjectHandle& pBase, member_ptr memberPtr, const Variant& value,
		                const IDefinitionManager& definitionManager)
		{
			auto pObject = reflectedCast<BaseType>(pBase.data(), pBase.type(), definitionManager);
			if (pObject && memberPtr)
			{
				pObject->*memberPtr = value;
				return true;
			}
			else
			{
				return false;
			}
		}
	};

	template <typename _dummy>
	struct can_set_Value<false, _dummy>
	{
		static bool readOnly(member_ptr memberPtr)
		{
			return can_set_impl<Variant::traits<TargetType>::can_downcast>::readOnly(memberPtr);
		}
	};

	template <typename _dummy>
	struct set_Value<false, _dummy>
	{
		static bool set(const ObjectHandle& pBase, member_ptr memberPtr, const Variant& value,
		                const IDefinitionManager& definitionManager)
		{
			return set_impl<Variant::traits<TargetType>::can_downcast>::set(pBase, memberPtr, value, definitionManager);
		}
	};

	template <bool can_set, typename _dummy = void>
	struct can_set_impl
	{
		static bool readOnly(member_ptr memberPtr)
		{
			return (memberPtr == nullptr);
		}
	};

	template <bool can_set, typename _dummy = void>
	struct set_impl
	{
		static bool set(const ObjectHandle& pBase, member_ptr memberPtr, const Variant& value,
		                const IDefinitionManager& definitionManager)
		{
			auto pObject = reflectedCast<BaseType>(pBase.data(), pBase.type(), definitionManager);
			if (pObject && memberPtr)
			{
				return ReflectionUtilities::extract(value, pObject->*memberPtr, definitionManager);
			}
			else
			{
				return false;
			}
		}
	};

	template <typename _dummy>
	struct can_set_impl<false, _dummy>
	{
		static bool readOnly(member_ptr)
		{
			return true;
		}
	};

	template <typename _dummy>
	struct set_impl<false, _dummy>
	{
		static bool set(const ObjectHandle& pBase, member_ptr, const Variant&, const IDefinitionManager&)
		{
			// nop
			return false;
		}
	};
};
} // end namespace wgt
#endif // REFLECTED_PROPERTY_HPP
