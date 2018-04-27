#ifndef FUNCTION_PROPERTY_HPP
#define FUNCTION_PROPERTY_HPP

#include "base_property.hpp"

#include "core_common/assert.hpp"
#include "core_reflection/utilities/reflection_utilities.hpp"
#include "core_reflection/utilities/object_handle_reflection_utils.hpp"
#include "core_variant/type_id.hpp"
#include "core_variant/variant.hpp"
#include "core_variant/collection.hpp"
#include "core_logging/logging.hpp"
#include <functional>

namespace wgt
{
template <class T, bool isEnum = std::is_enum<T>::value>
struct value_type
{
	typedef typename std::underlying_type<T>::type type;
};

template <class T>
struct value_type<T, false>
{
	typedef typename std::decay<T>::type type;
};


struct FunctionCollectionCallbacks
{
	typedef CollectionImplBase SigBase;
	Signal<SigBase::ElementRangeCallbackSignature> onPreInsert_;
	Signal<SigBase::ElementRangeCallbackSignature> onPostInserted_;
	Signal<SigBase::ElementRangeCallbackSignature> onPreErase_;
	Signal<SigBase::ElementRangeCallbackSignature> onPostErased_;
	Signal<SigBase::ElementPreChangeCallbackSignature> onPreChange_;
	Signal<SigBase::ElementPostChangedCallbackSignature> onPostChanged_;
};

template <typename TargetType, typename BaseType>
class FunctionPropertySet : public BaseProperty
{
public:
	typedef void (BaseType::*SetterFunc)(const TargetType&);

	explicit FunctionPropertySet(const char* name, const TypeId& type, SetterFunc setterFunc)
	    : BaseProperty(name, type), setter_(setterFunc)
	{
	}

	virtual bool isValue() const override
	{
		return true;
	}

	bool isCollection() const override
	{
		return Collection::traits<TargetType>::is_supported;
	}

	bool isByReference() const override
	{
		return std::is_reference<TargetType>::value;
	}

	//==========================================================================
	bool set(const ObjectHandle& provider, const Variant& value,
	         const IDefinitionManager& definitionManager) const override
	{
		// TODO NGT-1649
		// TF_ASSERT( !this->readOnly() );
		return set_Value<std::is_same<TargetType, Variant>::value>::set(provider, setter_, value, definitionManager);
	}

protected:
	bool hasSetter() const
	{
		return setter_ != nullptr;
	}

private:
	SetterFunc setter_;

	template <bool is_Variant, typename _dummy = void>
	struct set_Value
	{
		static bool set(const ObjectHandle& provider, SetterFunc setter, const Variant& value,
		                const IDefinitionManager& definitionManager)
		{
			auto pBase = reflectedCast<BaseType>(provider.data(), provider.type(), definitionManager);
			if (pBase == nullptr || setter == nullptr)
			{
				return false;
			}
			(pBase->*setter)(value);
			return true;
		}
	};

	template <typename _dummy>
	struct set_Value<false, _dummy>
	{
		static bool set(const ObjectHandle& provider, SetterFunc setter, const Variant& value,
		                const IDefinitionManager& definitionManager)
		{
			auto pBase = reflectedCast<BaseType>(provider.data(), provider.type(), definitionManager);
			if (pBase == nullptr || setter == nullptr)
			{
				return false;
			}

			const bool is_reference = std::is_reference<TargetType>::value;
			const bool can_set =
			!std::is_same<TargetType, const char*>::value && !std::is_same<TargetType, const wchar_t*>::value;

			return set_impl<can_set, is_reference>::set(setter, value, definitionManager, pBase);
		}
	};

	template <bool can_set, bool is_reference, typename _dummy = void>
	struct set_impl
	{
		static bool set(SetterFunc, const Variant&, const IDefinitionManager&, BaseType*)
		{
			const std::string type(typeid(TargetType).name());
			NGT_WARNING_MSG(("Type " + type + " not supported\n").c_str());
			return false;
		}
	};

	template <typename _dummy>
	struct set_impl<true, false, _dummy>
	{
		static bool set(SetterFunc setter, const Variant& value, const IDefinitionManager& definitionManager,
		                BaseType* pBase)
		{
			typedef typename value_type<TargetType>::type my_type;
			my_type v;
			if (ReflectionUtilities::extract(value, v, definitionManager))
			{
				(pBase->*setter)(static_cast<TargetType>(v));
				return true;
			}
			return false;
		}
	};

	template <typename _dummy>
	struct set_impl<true, true, _dummy>
	{
		static bool set(SetterFunc setter, const Variant& value, const IDefinitionManager& definitionManager,
		                BaseType* pBase)
		{
			typedef typename std::function<void(TargetType)> fn_type;

			fn_type fn = std::bind(setter, pBase, std::placeholders::_1);
			return ReflectionUtilities::extract(value, fn, definitionManager);
		}
	};
};

template <typename TargetType, typename BaseType, bool ByValue, bool ByArg>
class FunctionPropertyGet : public FunctionPropertySet<TargetType, BaseType>
{
};

template <typename TargetType, typename BaseType>
class FunctionPropertyGet<TargetType, BaseType, true, false> : public FunctionPropertySet<TargetType, BaseType>
{
public:
	typedef TargetType (BaseType::*GetterFunc)() const;

	explicit FunctionPropertyGet(const char* name, GetterFunc getterFunc,
	                             typename FunctionPropertySet<TargetType, BaseType>::SetterFunc setterFunc,
	                             const TypeId& type)
	    : FunctionPropertySet<TargetType, BaseType>(name, type, setterFunc), getterFunc_(getterFunc)
	{
	}

	virtual bool isValue() const override
	{
		return true;
	}

	virtual bool isByReference() const override
	{
		return false;
	}

	//==========================================================================
	Variant get(const ObjectHandle& provider, const IDefinitionManager& definitionManager) const override
	{
		TF_ASSERT(this->isValue());
		auto pBase = reflectedCast<BaseType>(provider.data(), provider.type(), definitionManager);
		TF_ASSERT(pBase != nullptr);
		TargetType result = (pBase->*getterFunc_)();
		return ReflectionUtilities::copy(result);
	}

private:
	GetterFunc getterFunc_;
};

template <typename TargetType, typename BaseType>
class FunctionPropertyGet<TargetType, BaseType, false, false> : public FunctionPropertySet<TargetType, BaseType>
{
public:
	typedef const TargetType& (BaseType::*GetterFunc)() const;

	explicit FunctionPropertyGet(const char* name, GetterFunc getterFunc,
	                             typename FunctionPropertySet<TargetType, BaseType>::SetterFunc setterFunc,
	                             const TypeId& type)
	    : FunctionPropertySet<TargetType, BaseType>(name, type, setterFunc), getterFunc_(getterFunc)
	{
	}

	virtual bool isByReference() const override
	{
		return true;
	}

	//==========================================================================
	Variant get(const ObjectHandle& provider, const IDefinitionManager& definitionManager) const override
	{
		auto pBase = reflectedCast<BaseType>(provider.data(), provider.type(), definitionManager);
		TF_ASSERT(pBase != nullptr);
		return ReflectionUtilities::reference((pBase->*getterFunc_)());
	}

private:
	GetterFunc getterFunc_;
};

template <typename TargetType, typename BaseType>
class FunctionPropertyGet<TargetType, BaseType, true, true> : public FunctionPropertySet<TargetType, BaseType>
{
public:
	typedef void (BaseType::*GetterFunc)(TargetType*) const;

	explicit FunctionPropertyGet(const char* name, GetterFunc getterFunc,
	                             typename FunctionPropertySet<TargetType, BaseType>::SetterFunc setterFunc,
	                             const TypeId& type)
	    : FunctionPropertySet<TargetType, BaseType>(name, type, setterFunc), getterFunc_(getterFunc)
	{
	}

	virtual bool isByReference() const override
	{
		return false;
	}

	//==========================================================================
	Variant get(const ObjectHandle& provider, const IDefinitionManager& definitionManager) const override
	{
		return CollectionHelper<>::get(provider, definitionManager, getterFunc_);
	}

private:
	GetterFunc getterFunc_;

	template <bool IsCollection =
	          !std::is_same<typename collection_details::CollectionImpl<TargetType>::type, void>::value,
	          typename _dummy = void>
	struct CollectionHelper
	{
		static Variant get(const ObjectHandle& provider, const IDefinitionManager& definitionManager,
		                   GetterFunc getterFunc)
		{
			auto pBase = reflectedCast<BaseType>(provider.data(), provider.type(), definitionManager);
			TargetType dummyRef;
			(pBase->*getterFunc)(&dummyRef);
			return ReflectionUtilities::copy(dummyRef);
		}
	};

	template <typename _dummy>
	struct CollectionHelper<true, _dummy>
	{
		static Variant get(const ObjectHandle& provider, const IDefinitionManager& definitionManager,
		                   GetterFunc getterFunc)
		{
			auto pBase = reflectedCast<BaseType>(provider.data(), provider.type(), definitionManager);
			auto pImpl = std::make_shared<CollectionHolder<TargetType>>();
			Collection collection(pImpl);
			(pBase->*getterFunc)(&pImpl->storage());
			return ReflectionUtilities::copy(collection);
		}
	};
};

template <typename TargetType, typename BaseType, bool ByValue, bool ByArg>
class FunctionProperty : public FunctionPropertyGet<TargetType, BaseType, ByValue, ByArg>
{
public:
	explicit FunctionProperty(const char* name,
	                          typename FunctionPropertyGet<TargetType, BaseType, ByValue, ByArg>::GetterFunc getterFunc,
	                          typename FunctionPropertySet<TargetType, BaseType>::SetterFunc setterFunc,
	                          const TypeId& type)
	    : FunctionPropertyGet<TargetType, BaseType, ByValue, ByArg>(name, getterFunc, setterFunc, type)
	{
	}

	bool readOnly(const ObjectHandle&) const override
	{
		return !this->hasSetter();
	}
};

template <typename TKey, typename TValue, typename TBase, typename GetKeyFunc, typename GetValueFunc >
class FunctionCollectionIteratorImpl : public CollectionIteratorImplBase
{
public:
	typedef FunctionCollectionIteratorImpl<TKey, TValue, TBase, GetKeyFunc, GetValueFunc > this_type;

private:
	TBase * pBase_;
	GetValueFunc getValueFunc_;
	GetKeyFunc getKeyFunc_;
	size_t index_;

public:
	FunctionCollectionIteratorImpl(
		TBase * pBase,
		GetKeyFunc getKeyFunc, GetValueFunc getValueFunc, size_t index)
	    : pBase_( pBase )
		, getKeyFunc_( getKeyFunc )
		, getValueFunc_(getValueFunc)
		, index_(index)
	{
	}

	//==========================================================================
	const TypeId& keyType() const override
	{
		return TypeId::getType<TKey>();
	}

	//==========================================================================
	const TypeId& valueType() const override
	{
		return TypeId::getType<TValue>();
	}

	//==========================================================================
	Variant key() const override
	{
		if (getKeyFunc_)
		{
			return (pBase_->*getKeyFunc_)(index_);
		}
		return index_;
	}

	//==========================================================================
	Variant value() const override
	{
		return (pBase_->*getValueFunc_)(index_);
	}

	template< bool canSet >
	struct SetValueHelper
	{
		template< typename T, typename TBase >
		static bool setValue(const Variant& v, TBase & pBase, T & getValueFunc )
		{
			TValue& value = (pBase->*getValueFunc_)(index_);
			return v.tryCast(value);
		}
	};


	template<>
	struct SetValueHelper< false >
	{
		template< typename T, typename TBase >
		static bool setValue(const Variant& v, TBase &, T & )
		{
			//Do nothing
			return false;
		}
	};


	template< typename T >
	struct SignatureExtractor
	{
		typedef T signature;
	};

	template< template< typename > typename C, typename T >
	struct SignatureExtractor< C < T > >
	{
		typedef T signature;
	};


	//--------------------------------------------------------------------------
	bool setValue(const Variant& v) const override
	{
		return SetValueHelper<
			std::is_reference< SignatureExtractor< GetValueFunc >::signature >::value >::setValue(v, pBase_, getValueFunc_ );
	}

	//==========================================================================
	void inc( size_t advAmount) override
	{
		index_+= advAmount;
	}

	//==========================================================================
	bool equals(const CollectionIteratorImplBase& that) const override
	{
		const this_type* pThis = dynamic_cast<const this_type*>(&that);
		if (pThis == nullptr)
		{
			return false;
		}
		return index_ == pThis->index_;
	}

	//==========================================================================
	CollectionIteratorImplPtr clone() const override
	{
		return std::make_shared<this_type>(*this);
	}
};

template <
	typename TKey,
	typename TValue,
	typename TBase,
	typename GetValueFunc,
	typename GetSizeFunc,
	typename GetKeyFunc >
class FunctionCollection : public CollectionImplBase
{
public:
	typedef std::function<size_t(const TKey&)> ManipulateKeyFunc;
	typedef FunctionCollectionIteratorImpl<TKey, TValue, TBase, GetKeyFunc, GetValueFunc > iterator_impl_type;

	template <typename TSizeFunc, typename TKeyFunc, typename TGetValueFunc, typename TManipulateKeyFunc >
	explicit FunctionCollection(
		TBase * pBase,
		TSizeFunc& getSizeFunc,
		TKeyFunc& getKeyFunc,
		TGetValueFunc & getValueFunc,
		TManipulateKeyFunc & addKeyFunc,
		TManipulateKeyFunc & removeKeyFunc )
	    : pBase_( pBase )
		, getSizeFunc_( getSizeFunc )
		, getKeyFunc_( getKeyFunc )
		, getValueFunc_( getValueFunc )
		, addKeyFunc_( addKeyFunc ? std::bind(addKeyFunc, pBase, std::placeholders::_1) : decltype( addKeyFunc_ )() )
		, removeKeyFunc_( removeKeyFunc ? std::bind(removeKeyFunc, pBase, std::placeholders::_1) : decltype(removeKeyFunc_)() )
	{
	}

	//==========================================================================
	const TypeId& keyType() const override
	{
		static auto s_KeyType = TypeId::getType<TKey>();
		return s_KeyType;
	}

	//==========================================================================
	const TypeId& valueType() const override
	{
		static auto s_ValueType = TypeId::getType<TValue>();
		return s_ValueType;
	}

	//==========================================================================
	const TypeId& containerType() const override
	{
		return TypeId::getType<FunctionCollection>();
	}

	//==========================================================================
	const void* container() const override
	{
		return this;
	}

	//==========================================================================
	bool empty() const
	{
		return (this->size() == 0);
	}

	//==========================================================================
	size_t size() const
	{
		return (pBase_->*getSizeFunc_)();
	}

	//==========================================================================
	CollectionIteratorImplPtr begin()
	{
		return generateIterator(0);
	}

	//==========================================================================
	CollectionIteratorImplPtr end()
	{
		return generateIterator(size());
	}

	//==========================================================================
	std::pair<CollectionIteratorImplPtr, bool> get(const Variant& key, GetPolicy policy)
	{
		typedef std::pair<CollectionIteratorImplPtr, bool> result_type;

		TKey i;
		if (!key.tryCast(i))
		{
			return result_type(end(), false);
		}

		size_t index = 0;
		if (getKeyFunc_)
		{
			index = (pBase_->*getKeyFunc_)(i);
		}
		else
		{
			index = i;
		}
		switch (policy)
		{
		case GET_EXISTING:
			if (index < size())
			{
				return result_type(generateIterator(index), false);
			}
			else
			{
				return result_type(end(), false);
			}

		case GET_NEW:
			if (addKeyFunc_)
			{
				if (getKeyFunc_ == nullptr)
				{
					index = size() ? size() - 1 : 0;
				}
				auto insertIt = generateIterator(index);
				callbacks_.onPreInsert_(insertIt, 1);
				index = addKeyFunc_( index );
				callbacks_.onPostInserted_(insertIt, 1);
				return result_type(insertIt, true);
			}
			else
			{
				return result_type(end(), false);
			}

		case GET_AUTO:
		{
			bool found = index < size();
			if (!found)
			{
				index = addKeyFunc_(i);
			}

			return result_type(generateIterator(index), !found);
		}

		default:
			return result_type(end(), false);
		}
	}


	//==========================================================================
	CollectionIteratorImplPtr insert(const Variant& key, const Variant& value) override
	{
		auto insertVal = get( key, GET_NEW );

		return insertVal.first;
	}


	//==========================================================================
	CollectionIteratorImplPtr erase(const CollectionIteratorImplPtr& pos)
	{
		return end();
	}


	//--------------------------------------------------------------------------
	size_t eraseKey(const Variant& key)
	{
		if (removeKeyFunc_)
		{
			TKey i;
			if (!key.tryCast(i))
			{
				return size();
			}
			callbacks_.onPreErase_(generateIterator( i ), 1);
			auto newIt = removeKeyFunc_( i );
			callbacks_.onPostErased_(generateIterator( newIt ), 1);
			return newIt;
		}
		return size();
	}

	//==========================================================================
	size_t erase(const Variant& key)
	{
		return size();
	}

	//==========================================================================
	CollectionIteratorImplPtr erase(const CollectionIteratorImplPtr& first, const CollectionIteratorImplPtr& last)
	{
		return end();
	}

	virtual int flags() const override
	{
		return (( addKeyFunc_ && removeKeyFunc_ ) ? RESIZABLE : 0);
	}


private:
	CollectionIteratorImplPtr generateIterator(size_t index) const
	{
		return std::make_shared<iterator_impl_type>( pBase_, getKeyFunc_, getValueFunc_, index);
	}

	TBase * pBase_;
	GetSizeFunc getSizeFunc_;
	ManipulateKeyFunc addKeyFunc_;
	ManipulateKeyFunc removeKeyFunc_;
	typename GetValueFunc getValueFunc_;
	GetKeyFunc getKeyFunc_;
	FunctionCollectionCallbacks callbacks_;


	//--------------------------------------------------------------------------
	Connection connectPreInsert(ElementRangeCallback callback) override
	{
		return callbacks_.onPreInsert_.connect(callback);
	}

	//--------------------------------------------------------------------------
	Connection connectPostInserted(ElementRangeCallback callback) override
	{
		return callbacks_.onPostInserted_.connect(callback);
	}


	//--------------------------------------------------------------------------
	Connection connectPreErase(ElementRangeCallback callback) override
	{
		return callbacks_.onPreErase_.connect(callback);
	}


	//--------------------------------------------------------------------------
	Connection connectPostErased(ElementRangeCallback callback) override
	{
		return callbacks_.onPostErased_.connect(callback);
	}


	//--------------------------------------------------------------------------
	Connection connectPreChange(ElementPreChangeCallback callback) override
	{
		return callbacks_.onPreChange_.connect(callback);
	}


	//--------------------------------------------------------------------------
	Connection connectPostChanged(ElementPostChangedCallback callback) override
	{
		return callbacks_.onPostChanged_.connect(callback);
	}
};


template <typename TKey, typename TValue, typename BaseType, typename GetValueSig >
class FunctionCollectionProperty : public BaseProperty
{
	typedef size_t (BaseType::*GetSizeFunc)() const;
	typedef size_t (BaseType::*ManipulateKeyFunc)(const TKey&);
	typedef TKey& (BaseType::*GetKeyFunc)(size_t) const;

public:
	explicit FunctionCollectionProperty(
		const char* name,
		GetSizeFunc getSizeFunc,
		GetKeyFunc getKeyFunc,
		GetValueSig getValueFunc,
		ManipulateKeyFunc addKeyFunc,
		ManipulateKeyFunc removeKeyFunc )
	    : BaseProperty(name, TypeId::getType<FunctionCollectionProperty>())
		, getSizeFunc_(getSizeFunc)
		, getValueFunc_(getValueFunc)
		, getKeyFunc_(getKeyFunc)
		, addKeyFunc_(addKeyFunc)
		, removeKeyFunc_( removeKeyFunc )
	{
	} 

	virtual bool isValue() const override
	{
		return true;
	}

	virtual bool isCollection() const override
	{
		return true;
	}

	virtual bool isByReference() const override
	{
		return std::is_reference<TValue>::value;
	}

	//--------------------------------------------------------------------------
	Variant get(const ObjectHandle& provider, const IDefinitionManager& definitionManager) const override
	{
		TF_ASSERT(this->isValue());
		BaseType * pBase = reflectedCast<BaseType>(provider.data(), provider.type(), definitionManager);

		typedef FunctionCollection<
			TKey,
			TValue,
			BaseType,
			decltype( getValueFunc_ ),
			decltype( getSizeFunc_ ),
			decltype( getKeyFunc_ ) >
			FunctionCollectionType;
		return Collection(std::make_shared<FunctionCollectionType>(
			pBase,
			getSizeFunc_,
			getKeyFunc_,
			getValueFunc_,
			addKeyFunc_,
			removeKeyFunc_ ) );
	}


	//--------------------------------------------------------------------------
	bool readOnly(const ObjectHandle&) const override
	{
		return true;
	}

	//==========================================================================
	bool set(const ObjectHandle& handle, const Variant& value, const IDefinitionManager& definitionManager) const override
	{
		return false;
	}

private:
	GetSizeFunc getSizeFunc_;
	ManipulateKeyFunc addKeyFunc_;
	ManipulateKeyFunc removeKeyFunc_;
	GetValueSig getValueFunc_;
	GetKeyFunc getKeyFunc_;
};

} // end namespace wgt
#endif // FUNCTION_PROPERTY_HPP
