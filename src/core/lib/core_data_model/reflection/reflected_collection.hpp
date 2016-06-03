#ifndef REFLECTED_COLLECTION_HPP
#define REFLECTED_COLLECTION_HPP

#include "core_variant/collection.hpp"
#include "core_reflection/property_accessor.hpp"

namespace wgt
{
class IReflectionController;

class ReflectedCollection : public CollectionImplBase
{
public:
	ReflectedCollection( const PropertyAccessor & pa, IReflectionController * controller );
	virtual ~ReflectedCollection();

	void reset();

	virtual size_t size() const override;

	virtual CollectionIteratorImplPtr begin() override;

	virtual CollectionIteratorImplPtr end() override;

	virtual std::pair< CollectionIteratorImplPtr, bool > get(const Variant& key, GetPolicy policy) override;

	virtual CollectionIteratorImplPtr erase(const CollectionIteratorImplPtr& pos) override;

	virtual size_t erase(const Variant& key) override;

	virtual CollectionIteratorImplPtr erase(const CollectionIteratorImplPtr& first, const CollectionIteratorImplPtr& last) override;

	virtual const TypeId& keyType() const override;

	virtual const TypeId& valueType() const override;

	virtual const TypeId& containerType() const override;

	virtual const void* container() const override;

	virtual int flags() const override;

	Connection connectPreInsert( ElementRangeCallback callback ) override
	{
		return onPreInsert_.connect( callback );
	}

	Connection connectPostInserted( ElementRangeCallback callback ) override
	{
		return onPostInserted_.connect( callback );
	}

	Connection connectPreErase( ElementRangeCallback callback ) override
	{
		return onPreErase_.connect( callback );
	}

	Connection connectPostErased(ElementRangeCallback callback) override
	{
		return onPostErased_.connect( callback );
	}

	Connection connectPreChange( ElementPreChangeCallback callback ) override
	{
		return onPreChange_.connect( callback );
	}

	Connection connectPostChanged( ElementPostChangedCallback callback ) override
	{
		return onPostChanged_.connect( callback );
	}

public:
	PropertyAccessor pa_;
	std::shared_ptr< PropertyAccessorListener > listener_;
	IReflectionController * controller_;
	Collection collection_;

	Signal< ElementRangeCallbackSignature > onPreInsert_;
	Signal< ElementRangeCallbackSignature > onPostInserted_;
	Signal< ElementRangeCallbackSignature > onPreErase_;
	Signal< ElementRangeCallbackSignature > onPostErased_;
	Signal< ElementPreChangeCallbackSignature > onPreChange_;
	Signal< ElementPostChangedCallbackSignature > onPostChanged_;
};
} // end namespace wgt
#endif // REFLECTED_COLLECTION_HPP
