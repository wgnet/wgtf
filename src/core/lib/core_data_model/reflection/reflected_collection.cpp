#include "reflected_collection.hpp"

#include "core_common/assert.hpp"
#include "core_reflection/property_accessor_listener.hpp"
#include "core_reflection/interfaces/i_reflection_controller.hpp"
#include "core_reflection/i_definition_manager.hpp"

namespace wgt
{
namespace ReflectedCollectionDetails
{
class ReflectedCollectionIteratorImpl : public CollectionIteratorImplBase
{
public:
	ReflectedCollectionIteratorImpl(ReflectedCollection& reflectedCollection, IReflectionController* controller, CollectionIteratorImplPtr iterator)
		: reflectedCollection_(reflectedCollection)
		, controller_(controller)
		, iterator_(iterator)
	{

	}
	const TypeId& keyType() const override
	{
		return iterator_->keyType();
	}

	const TypeId& valueType() const override
	{
		return iterator_->valueType();
	}

	Variant key() const override
	{
		return iterator_->key();
	}

	Variant value() const override
	{
		std::string indexer = iterator_->indexer();
		if (indexer.empty())
		{
			TF_ASSERT(false);//it goes wrong if the key cannot be converted as a string.
			return Variant();
		}

		auto object = reflectedCollection_.pa_.getObject();
		auto defManager = reflectedCollection_.pa_.getDefinitionManager();
		std::string collectionPath = reflectedCollection_.pa_.getPath();
		std::string itemPath = collectionPath + indexer;
		auto definition = defManager->getObjectDefinition(object);
		TF_ASSERT(definition != nullptr);
		auto propertyAccessor = definition->bindProperty(itemPath.c_str(), object);
		return controller_ ? controller_->getValue(propertyAccessor) : propertyAccessor.getValue();
	}

	bool setValue(const Variant& v) const override
	{
		auto key = iterator_->key();
		std::string keyValue;
		if (!key.tryCast(keyValue))
		{
			TF_ASSERT(false);//it goes wrong if the key cannot be converted as a string.
			return false;
		}
		std::string itemPath = Collection::getIndexOpen() + keyValue + Collection::getIndexClose();
		auto object = reflectedCollection_.pa_.getObject();
		auto defManager = reflectedCollection_.pa_.getDefinitionManager();
		std::string collectionPath = reflectedCollection_.pa_.getPath();
		itemPath = collectionPath + itemPath;
		auto definition = defManager->getObjectDefinition(object);
		TF_ASSERT(definition != nullptr);
		auto propertyAccessor = definition->bindProperty(itemPath.c_str(), object);
		if (controller_ != nullptr)
		{
			controller_->setValue(propertyAccessor, v);
			return true;
		}
		else
		{
			return propertyAccessor.setValue(v);
		}
	}

	void inc(size_t advAmount) override
	{
		iterator_->inc(advAmount);
	}

	bool equals(const CollectionIteratorImplBase& that) const override
	{
		const ReflectedCollectionIteratorImpl* t = dynamic_cast<const ReflectedCollectionIteratorImpl*>(&that);
		if (!t)
		{
			return false;
		}
		if (t->iterator_ == nullptr)
		{
			return false;
		}
		return iterator_->equals(*t->iterator_);
	}

	CollectionIteratorImplPtr clone() const override
	{
		return std::make_shared<ReflectedCollectionIteratorImpl>(*this);
	}

	ReflectedCollection& reflectedCollection_;
	IReflectionController* controller_;
	CollectionIteratorImplPtr iterator_;
};

CollectionIteratorImplPtr makeIterator(ReflectedCollection& reflectedCollection, IReflectionController* controller, CollectionIteratorImplPtr iterator)
{
	return std::make_shared<ReflectedCollectionIteratorImpl>(reflectedCollection, controller, iterator);
}

class ReflectedCollectionListener : public PropertyAccessorListener
{
public:
	ReflectedCollectionListener(ReflectedCollection& reflectedCollection) : reflectedCollection_(reflectedCollection)
	{
		object_ = reflectedCollection_.pa_.getObject();
		path_ = reflectedCollection_.pa_.getFullPath();
	}

	void preSetValue(const PropertyAccessor& accessor, const Variant& value) override
	{
		if (object_ != accessor.getObject() || path_ != accessor.getFullPath())
		{
			return;
		}
	}

	void postSetValue(const PropertyAccessor& accessor, const Variant& value) override
	{
		if (object_ != accessor.getObject() || path_ != accessor.getFullPath())
		{
			return;
		}
	}

	void preInsert(const PropertyAccessor& accessor, size_t index, size_t count) override
	{
		if (object_ != accessor.getObject() || path_ != accessor.getFullPath())
		{
			return;
		}

		auto it = reflectedCollection_.begin();
		it->inc(index);
		reflectedCollection_.onPreInsert_(it, count);
	}

	void postInserted(const PropertyAccessor& accessor, size_t index, size_t count) override
	{
		if (object_ != accessor.getObject() || path_ != accessor.getFullPath())
		{
			return;
		}

		reflectedCollection_.reset();
		auto it = reflectedCollection_.begin();
		it->inc(index);
		reflectedCollection_.onPostInserted_(it, count);
	}

	void preErase(const PropertyAccessor& accessor, size_t index, size_t count) override
	{
		if (object_ != accessor.getObject() || path_ != accessor.getFullPath())
		{
			return;
		}

		auto it = reflectedCollection_.begin();
		it->inc(index);
		reflectedCollection_.onPreErase_(it, count);
	}

	void postErased(const PropertyAccessor& accessor, size_t index, size_t count) override
	{
		if (object_ != accessor.getObject() || path_ != accessor.getFullPath())
		{
			return;
		}

		reflectedCollection_.reset();
		auto it = reflectedCollection_.begin();
		it->inc(index);
		reflectedCollection_.onPostErased_(it, count);
	}

private:
	ReflectedCollection& reflectedCollection_;
	ObjectHandle object_;
	std::string path_;
};
}

// TODO custom iterator

ReflectedCollection::ReflectedCollection(const PropertyAccessor& pa, IReflectionController* controller)
    : pa_(pa), listener_(new ReflectedCollectionDetails::ReflectedCollectionListener(*this)), controller_(controller)
{
	auto definitionManager = const_cast<IDefinitionManager*>(pa_.getDefinitionManager());
	definitionManager->registerPropertyAccessorListener(listener_);

	reset();
}

ReflectedCollection::~ReflectedCollection()
{
	auto definitionManager = const_cast<IDefinitionManager*>(pa_.getDefinitionManager());
	definitionManager->deregisterPropertyAccessorListener(listener_);
}

void ReflectedCollection::reset()
{
	auto value = pa_.getValue();
	collection_ = value.cast<Collection>();
	if(collection_.isValid())
	{
		connections_.clear();
		connections_.add(collection_.connectPreChange(
			[this](const Collection::Iterator& pos, const Variant& newValue){
			onPreChange_(ReflectedCollectionDetails::makeIterator(*this, controller_, pos.impl()), newValue);
		}));
		connections_.add(collection_.connectPostChanged(
			[this](const Collection::Iterator& pos, const Variant& oldValue){
			onPostChanged_(ReflectedCollectionDetails::makeIterator(*this, controller_, pos.impl()), oldValue);
		}));
		connections_.add(collection_.connectPreInsert(
			[this](const Collection::Iterator& pos, size_t count){
			onPreInsert_(ReflectedCollectionDetails::makeIterator(*this, controller_, pos.impl()), count);
		}));
		connections_.add(collection_.connectPostInserted(
			[this](const Collection::Iterator& pos, size_t count){
			onPostInserted_(ReflectedCollectionDetails::makeIterator(*this, controller_, pos.impl()), count);
		}));
		connections_.add(collection_.connectPreErase(
			[this](const Collection::Iterator& pos, size_t count){
			onPreErase_(ReflectedCollectionDetails::makeIterator(*this, controller_, pos.impl()), count);
		}));
		connections_.add(collection_.connectPostErased(
			[this](const Collection::Iterator& pos, size_t count){
			onPostErased_(ReflectedCollectionDetails::makeIterator(*this, controller_, pos.impl()), count);
		}));
	}
}

size_t ReflectedCollection::size() const
{
	return collection_.size();
}

CollectionIteratorImplPtr ReflectedCollection::begin()
{
	return ReflectedCollectionDetails::makeIterator(*this, controller_, collection_.begin().impl());
}

CollectionIteratorImplPtr ReflectedCollection::end()
{
	return ReflectedCollectionDetails::makeIterator(*this, controller_, collection_.end().impl());
}

std::pair<CollectionIteratorImplPtr, bool> ReflectedCollection::get(const Variant& key, GetPolicy policy)
{
	typedef std::pair<CollectionIteratorImplPtr, bool> result_type;

	auto it = collection_.find(key);
	if (policy == GetPolicy::GET_EXISTING)
	{
		return result_type(ReflectedCollectionDetails::makeIterator(*this, controller_, it.impl()), false);
	}

	if (it != collection_.end())
	{
		if (policy == GetPolicy::GET_AUTO)
		{
			return result_type(ReflectedCollectionDetails::makeIterator(*this, controller_, it.impl()), false);
		}
	}

	if (controller_)
	{
		controller_->insert(pa_, key, Variant());
	}
	else
	{
		pa_.insert(key, Variant());
	}
	it = collection_.find(key);
	return it != end() ? result_type(ReflectedCollectionDetails::makeIterator(*this, controller_, it.impl()), true) : result_type(end(), false);
}

CollectionIteratorImplPtr ReflectedCollection::insert(const Variant& key, const Variant& value) /* override */
{
	if (controller_)
	{
		controller_->insert(pa_, key, value);
	}
	else
	{
		pa_.insert(key, value);
	}
	auto it = collection_.find(key);
	return it != end() ? ReflectedCollectionDetails::makeIterator(*this, controller_, it.impl()) : end();
}

CollectionIteratorImplPtr ReflectedCollection::erase(const CollectionIteratorImplPtr& pos)
{
	TF_ASSERT(false);
	return end();
}

size_t ReflectedCollection::eraseKey(const Variant& key)
{
	auto count = collection_.size();
	if (controller_)
	{
		controller_->erase(pa_, key);
	}
	else
	{
		pa_.erase(key);
	}
	count -= collection_.size();
	return count;
}

CollectionIteratorImplPtr ReflectedCollection::erase(const CollectionIteratorImplPtr& first,
                                                     const CollectionIteratorImplPtr& last)
{
	TF_ASSERT(false);
	return end();
}

const TypeId& ReflectedCollection::keyType() const
{
	return collection_.keyType();
}

const TypeId& ReflectedCollection::valueType() const
{
	return collection_.valueType();
}

const TypeId& ReflectedCollection::containerType() const
{
	return collection_.impl()->containerType();
}

const void* ReflectedCollection::container() const
{
	return collection_.impl()->container();
}

int ReflectedCollection::flags() const
{
	return collection_.flags();
}
} // end namespace wgt
