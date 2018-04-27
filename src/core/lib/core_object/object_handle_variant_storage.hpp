#ifndef OBJECT_HANDLE_STORAGE_VARIANT_HPP_INCLUDED
#define OBJECT_HANDLE_STORAGE_VARIANT_HPP_INCLUDED

#include "object_handle_storage.hpp"
#include "core_variant/variant.hpp"

namespace wgt
{
class ObjectHandleVariantStorage : public ParentedObjectHandleStorage
{
public:
	template<typename T = void>
	ObjectHandleVariantStorage(const Variant& variant)
		: variant_(variant), variantPtr_(&variant_)
	{
		ENFORCE_OBJECT_HANDLE_CONTEXT
	}

	template<typename T = void>
	ObjectHandleVariantStorage(Variant* variant)
		: variant_(), variantPtr_(variant)
	{
		ENFORCE_OBJECT_HANDLE_CONTEXT
	}

	virtual void* data() const override
	{
		return const_cast<void*>(variantPtr_->value<const void*>());
	}

	virtual TypeId type() const override
	{
		return variantPtr_->type()->typeId();
	}

	virtual bool isValid() const override
	{
		return variantPtr_ != nullptr;
	}

    virtual IObjectHandleProvider* provider() const override
    {
        return nullptr;
    }

	bool operator==(const IObjectHandleStorage & other) const override
	{
		auto pOther = dynamic_cast< const ObjectHandleVariantStorage * >(&other);
		if (pOther == nullptr)
		{
			return false;
		}
		if (variantPtr_ == pOther->variantPtr_)
		{
			return ParentedObjectHandleStorage::operator==(*pOther);
		}
		if (variantPtr_ == nullptr || pOther->variantPtr_ == nullptr)
		{
			return false;
		}
		return *variantPtr_ == *pOther->variantPtr_ &&
			ParentedObjectHandleStorage::operator==(*pOther);;
	}

	uint64_t getHashcode() const override
	{
		if (variantPtr_ == nullptr)
		{
			return 0;
		}
		return variantPtr_->getHashCode();
	}

private:
	mutable Variant variant_;
	Variant* variantPtr_;
};
} // end namespace wgt
#endif // OBJECT_HANDLE_STORAGE_VARIANT_HPP_INCLUDED
