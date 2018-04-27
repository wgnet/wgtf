#ifndef COLLECTION_ELEMENT_HOLDER_HPP
#define COLLECTION_ELEMENT_HOLDER_HPP

#include "core_reflection/base_property.hpp"

namespace wgt
{

class CollectionElementHolder : public BaseProperty
{
	typedef BaseProperty base;

public:
	CollectionElementHolder(const Collection& collection, const Collection::Iterator& collectionIt,
		const TypeId& valueType, std::string propName, const IBasePropertyPtr& parentProperty, const IDefinitionManager& definitionManager)
		: base("", valueType)
		, collection_(collection)
		, collectionIt_(collectionIt)
		, propName_(std::move(propName))
		, parentHolder_(std::dynamic_pointer_cast<CollectionElementHolder>(parentProperty))
		, parentProperty_(parentProperty)
	{
		setName(propName_.c_str());

		auto itemMetadata = parentProperty ? findFirstMetaData<MetaCollectionItemMetaObj>(*parentProperty, definitionManager) : nullptr;
		metaData_ = itemMetadata != nullptr ? &itemMetadata->getMetaData() : nullptr;
	}

	const TypeId& getType() const override
	{
		return collectionIt_.valueType();
	}

	const MetaData & getMetaData() const override
	{
		if (metaData_ == nullptr)
		{
			static MetaData s_Empty;
			return s_Empty;
		}
		return *metaData_;
	}

	virtual bool isValue() const override
	{
		return true;
	}

	virtual bool isCollection() const override
	{
		Collection value;
		return collectionIt_.value().tryCast<Collection>(value);
	}

	virtual bool isByReference() const override
	{
		return parentProperty_ ? parentProperty_->isByReference() : false;
	}

	Variant get(const ObjectHandle& pBase, const IDefinitionManager& definitionManager) const override
	{
		TF_ASSERT(this->isValue());
		return collectionIt_.value();
	}

	bool set(const ObjectHandle& handle, const Variant& value, const IDefinitionManager& definitionManager) const override
	{
		TF_ASSERT(!this->readOnly(handle));

		Variant variant = value;
		auto valueType = collectionIt_.valueType();
		if (valueType.isPointer())
		{
			auto targetType = valueType.removePointer();
			ObjectHandle source;
			if (value.tryCast(source))
			{
				variant = reflectedCast(source, targetType, definitionManager);
			}
		}

		if (!collectionIt_.setValue(variant))
		{
			return false;
		}

		std::shared_ptr<ObjectReference> reference = std::dynamic_pointer_cast<ObjectReference>(handle.storage());
		if (reference != nullptr)
		{
			if (parentHolder_ != nullptr)
			{
				return parentHolder_->set(handle, collection_, definitionManager);
			}
		}

		if (parentProperty_)
		{
			auto callback = findFirstMetaData<MetaCallbackObj>(*parentProperty_, definitionManager);
			if (callback != nullptr)
			{
				callback->invoke(handle);
			}
		}

		return true;
	}

private:
	Collection collection_; // need to keep Collection data alive to keep iterator valid
	Collection::Iterator collectionIt_;
	std::string propName_;
	std::shared_ptr<CollectionElementHolder> parentHolder_;
	IBasePropertyPtr parentProperty_;
	const MetaData * metaData_;
};

}

#endif //COLLECTION_ELEMENT_HOLDER_HPP