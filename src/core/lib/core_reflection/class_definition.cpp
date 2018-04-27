#include "class_definition.hpp"

#include "base_property.hpp"
#include "reflected_object.hpp"
#include "property_accessor.hpp"
#include "property_iterator.hpp"
#include "i_definition_manager.hpp"
#include "i_object_manager.hpp"
#include "object/object_reference.hpp"

#include "interfaces/i_class_definition_details.hpp"
#include "interfaces/i_meta_utilities.hpp"

#include "metadata/meta_base.hpp"
#include "metadata/meta_impl.hpp"

#include "utilities/definition_helpers.hpp"
#include "utilities/reflection_utilities.hpp"

#include "core_common/assert.hpp"
#include "core_variant/variant.hpp"
#include "core_variant/collection.hpp"
#include "core_serialization/fixed_memory_stream.hpp"
#include "core_serialization/text_stream.hpp"
#include "core_serialization/text_stream_manip.hpp"
#include "private/reflection_cache.hpp"
#include "private/property_path.hpp"
#include "private/collection_element_holder.hpp"
#include <algorithm>
#include <utility>
#include "core_logging/logging.hpp"

namespace wgt
{

//------------------------------------------------------------------------------
struct ClassDefinition::Impl
{
	Impl(std::unique_ptr<IClassDefinitionDetails> details, ClassDefinition & self )
		: details_(std::move(details))
		, metaBasesHolder_(nullptr)
		, metaBaseInited_( false )
		, self_( self )
	{
	}

	std::unique_ptr<const IClassDefinitionDetails> details_;
	IDefinitionManager * defManager_ = nullptr;
	mutable std::vector<ManagedObjectPtr> sharedObjects_;
	mutable bool metaBaseInited_;
	mutable ObjectHandleT< MetaBasesHolderObj  > metaBasesHolder_;
	ClassDefinition & self_;

	const ObjectHandleT< MetaBasesHolderObj  > & getMetaBasesHolder() const
	{
		if (metaBaseInited_)
		{
			return metaBasesHolder_;
		}
		metaBaseInited_ = true;
		metaBasesHolder_ = findFirstMetaData< MetaBasesHolderObj >(self_, *self_.getDefinitionManager());
		return metaBasesHolder_;
	}

	PropertyAccessor bindProperty(
		const IClassDefinition & definition,
		IPropertyPath::ConstPtr & path, const Variant& base) const;

	void bindProperty(
		const ClassDefinition & definition,
		const char* path, 
		const Variant& base, PropertyAccessor& propertyAccessor) const;

	PropertyAccessor bindProperty(
		std::vector< std::remove_const< IPropertyPath::ConstPtr >::type > & o_Components,
		const Variant & base,
		IBasePropertyPtr& parentProperty,
		std::shared_ptr<ObjectReference> & o_ParentReference,
		const IClassDefinition * & o_ParentDefinition) const;



	//------------------------------------------------------------------------------
	const std::vector< std::string > & getParentNames() const
	{
		auto metaBasesHolder = getMetaBasesHolder();
		if (metaBasesHolder == nullptr)
		{
			static std::vector< std::string > s_EmptyCollection;
			return s_EmptyCollection;
		}
		return metaBasesHolder->getParents();
	}


	//------------------------------------------------------------------------------
	PropertyIteratorRange allProperties() const
	{
		return PropertyIteratorRange(PropertyIterator::ITERATE_PARENTS, self_ );
	}

	//------------------------------------------------------------------------------
	IBasePropertyPtr findProperty( IPropertyPath::ConstPtr & path) const
	{
		// Some definitions allow you to lookup by name directly
		if (details_->canDirectLookupProperty())
		{
			IBasePropertyPtr prop = details_->directLookupProperty(path);
			if (prop != nullptr)
			{
				return prop;
			}
			const auto & parentNames = getParentNames();

			for (const auto & parentName : parentNames)
			{
				auto parentDef =
					defManager_->getDefinition(parentName.c_str());
				if (parentDef == nullptr)
				{
					continue;
				}
				prop = parentDef->findProperty(path);
				if (prop != nullptr)
				{
					return prop;
				}
			}
			return nullptr;
		}

		// Otherwise, perform a search
		auto nameHash = path->getHash();
		auto properties = allProperties();
		for (auto it = properties.begin(); it != properties.end(); ++it)
		{
			if (it->getNameHash() == nameHash)
			{
				return *it;
			}
		}
		return nullptr;
	}
};


//------------------------------------------------------------------------------
ClassDefinition::ClassDefinition(std::unique_ptr<IClassDefinitionDetails> details)
	: impl_( std::make_unique< Impl >( std::move( details ), *this ) )
{
}

//------------------------------------------------------------------------------
ClassDefinition::~ClassDefinition()
{
}

//------------------------------------------------------------------------------
const IClassDefinitionDetails& ClassDefinition::getDetails() const
{
	return *impl_->details_;
}

//------------------------------------------------------------------------------
PropertyIteratorRange ClassDefinition::allProperties() const
{
	return impl_->allProperties();
}

//------------------------------------------------------------------------------
PropertyIteratorRange ClassDefinition::directProperties() const
{
	return PropertyIteratorRange(PropertyIterator::ITERATE_SELF_ONLY, *this);
}


//------------------------------------------------------------------------------
const std::vector< std::string > & ClassDefinition::getParentNames() const
{
	return impl_->getParentNames();
}


//------------------------------------------------------------------------------
PropertyAccessor ClassDefinition::bindProperty(
	IPropertyPath::ConstPtr & path, const ObjectHandle& object) const
{
	return impl_->bindProperty( *this, path, object );
}

//------------------------------------------------------------------------------
PropertyAccessor ClassDefinition::bindProperty(const char* name, const ObjectHandle& object) const
{
	TF_ASSERT(getDefinitionManager());
	TF_ASSERT(this == getDefinitionManager()->getDefinition(object));

	std::shared_ptr<ObjectReference> reference =
		std::dynamic_pointer_cast<ObjectReference>(object.storage());
	TF_ASSERT(reference);
	auto path = name ? name : "";

	PropertyAccessor propAccessor("", reference);
	impl_->bindProperty( *this, path, object, propAccessor);
	return std::move(propAccessor);
}

//------------------------------------------------------------------------------
PropertyAccessor ClassDefinition::Impl::bindProperty(
	const IClassDefinition & definition,
	IPropertyPath::ConstPtr & path, const Variant& base) const
{
	if (!path)
	{
		return PropertyAccessor();
	}

	auto cache = ReflectionPrivate::s_Cache
		? &(ReflectionPrivate::s_Cache->propCache_[&definition][base])
		: nullptr;

	auto paCache = ReflectionPrivate::s_Cache
		? &(ReflectionPrivate::s_Cache->valueCache_) : nullptr;

	auto propertyId = cache ? path->getRecursiveHash() : 0;

	PropertyAccessor accessor;
	IBasePropertyPtr parentProperty = nullptr;
	std::shared_ptr<ObjectReference> parentReference;
	const IClassDefinition * parentDefinition = &definition;
	Variant currentBase = base;

	auto tail = path;
	typedef std::remove_const< IPropertyPath::ConstPtr >::type PropertyPathStorage;
	std::vector< PropertyPathStorage > components;

	PropertyPathStorage componentsEnd = nullptr;

	//TODO? Cache the bases as well?
	if (cache)
	{
		auto subTail = tail;
		while (subTail != nullptr)
		{
			auto subId = cache ? subTail->getRecursiveHash() : 0;
			auto findIt = cache->find(subId);
			if (findIt != cache->end())
			{
				accessor = std::move(PropertyAccessor(findIt->second));
				componentsEnd = subTail;
				break;
			}
			subTail = subTail->getParent();
		}
	}
	while (
		tail != nullptr && 
		tail != componentsEnd)
	{
		components.push_back(tail);
		tail = tail->getParent();
	}

	if (components.empty())
	{
		return accessor;
	}

	if (accessor.isValid())
	{
		parentProperty = accessor.getProperty();
		parentReference = accessor.getData()->reference_;
		currentBase = accessor.getValue();
	}

	while (components.empty() == false)
	{
		accessor = bindProperty(
			components,
			currentBase, parentProperty, parentReference, parentDefinition );
		if (accessor.isValid() == false )
		{
			return PropertyAccessor();
		}

		if (components.empty())
		{
			break;
		}
		parentProperty = accessor.getProperty();

		//If a sub object
		currentBase = accessor.getValue();
	}
	if (cache)
	{
		(*cache)[propertyId] = accessor.getData();
	}
	return accessor;
}

//------------------------------------------------------------------------------
PropertyAccessor ClassDefinition::Impl::bindProperty(
	std::vector< std::remove_const< IPropertyPath::ConstPtr >::type > & o_Components,
	const Variant& base,
	IBasePropertyPtr& parentProperty,
	std::shared_ptr<ObjectReference> & o_ParentReference,
	const IClassDefinition * & o_ParentDefinition ) const
{
	TF_ASSERT(o_Components.empty() == false);

	auto component = o_Components.back();
	o_Components.pop_back();
	switch (component->getType() )
	{

	case IPropertyPath::TYPE_PROPERTY:
	case IPropertyPath::TYPE_COLLECTION:
	{
		ObjectHandle handle;
		if (!base.tryCast(handle))
		{
			// Fail: could not find property
			return PropertyAccessor();
		}

		handle = reflectedRoot(handle, *defManager_);
		o_ParentDefinition = defManager_->getDefinition(handle);
		auto currentDef = o_ParentDefinition;
		if (!currentDef)
		{
			// Fail: not a reflected type
			return PropertyAccessor();
		}

		auto cache = ReflectionPrivate::s_Cache
			? &(ReflectionPrivate::s_Cache->propCache_[currentDef][handle.getRecursiveHash()])
			: nullptr;

		auto propertyId = cache ? component->getRecursiveHash() : 0;
		if (cache)
		{
			auto findIt = cache->find(propertyId);
			if (findIt != cache->end())
			{
				o_ParentReference = findIt->second->reference_;
				return std::move( PropertyAccessor(findIt->second) );
			}
		}

		auto property = currentDef->findProperty(component->getPath().c_str(), component->getPath().length());
		if (property == nullptr)
		{
			// Fail: could not find property
			return PropertyAccessor();
		}

		/* BEGIN TEMP */
		o_ParentReference =
			std::dynamic_pointer_cast<ObjectReference>(handle.storage());
		TF_ASSERT(o_ParentReference);
		/* END TEMP */

		PropertyAccessor accessor( component->getPath().c_str(), o_ParentReference );
		accessor.setBaseProperty(property);

		if (cache)
		{
			(*cache)[propertyId] = accessor.getData();
		}
		return accessor;
	}
	case IPropertyPath::TYPE_COLLECTION_ITEM:
	{
		Collection collection;

		if (!base.tryCast(collection))
		{
			ObjectHandle handle;
			bool castSuccessful = base.tryCast(handle);
			Collection* collectionPointer = handle.getBase<Collection>();

			if (!castSuccessful || collectionPointer == nullptr)
			{
				// Fail: could not find property
				return PropertyAccessor();
			}
			collection = *collectionPointer;
		}

		auto cache = ReflectionPrivate::s_Cache
			? &(ReflectionPrivate::s_Cache->propCache_[o_ParentDefinition][collection])
			: nullptr;

		auto propertyId = cache ? component->getRecursiveHash() : 0;
		if (cache)
		{
			auto findIt = cache->find(propertyId);
			if (findIt != cache->end())
			{
				o_ParentReference = findIt->second->reference_;
				return std::move(PropertyAccessor(findIt->second));
			}
		}


		// determine key type (heterogeneous keys are not supported yet)
		const auto begin = collection.begin();
		const auto end = collection.end();

		if (begin == end)
		{
			// can't index empty collection
			return PropertyAccessor();
		}

		
		auto decoratedPath = component->generateDecoratedPath();
		auto pDecoratedPath = decoratedPath.c_str();

		Variant key = Collection::parseKey(begin.key().type(), pDecoratedPath);
		auto it = collection.find(key);
		// TODO Cache these against the parent reference
		auto property = std::make_shared<CollectionElementHolder>(collection, it, collection.valueType(), decoratedPath,
			parentProperty, *defManager_);

		auto parent = component->getParent();
		TF_ASSERT(parent);
		PropertyAccessor accessor(
			(parent ? parent->getPath() + decoratedPath : decoratedPath).c_str(), 
			o_ParentReference );

		accessor.setBaseProperty(property);

		if (cache)
		{
			(*cache)[propertyId] = accessor.getData();
		}
		return accessor;
	}
	default:
		TF_ASSERT(false, "Unhandled type");
		break;
	}
	return PropertyAccessor();
}

//------------------------------------------------------------------------------
void ClassDefinition::Impl::bindProperty(
	const ClassDefinition & definition,
	const char* path, const Variant& base,
	PropertyAccessor& o_PropertyAccessor) const
{
	if (!*path)
	{
		return;
	}

	auto hashCode = base.getHashCode();
	auto cache = ReflectionPrivate::s_Cache
		? &( ReflectionPrivate::s_Cache->propCache_[ &definition ][ base ] )
		: nullptr;

	auto propertyId = cache ? ReflectionPrivate::computePropertyId(path) : 0;
	if (cache)
	{
		auto findIt = cache->find(propertyId);
		if (findIt != cache->end())
		{
			o_PropertyAccessor = std::move(PropertyAccessor(findIt->second));
			return;
		}
	}
	IBasePropertyPtr property;
	bool continueLooking;
	std::string propertyName;

	std::tie(property, continueLooking, propertyName) = ReflectionUtilities::parseProperty(path, base, o_PropertyAccessor.getProperty(), &definition, *defManager_);

	if (property == nullptr)
	{
		// Fail: could not find property
		o_PropertyAccessor.setBaseProperty(nullptr);
		return;
	}

	o_PropertyAccessor.setBaseProperty(property);
	o_PropertyAccessor.setPath(o_PropertyAccessor.getPath() + propertyName);

	if (!continueLooking)
	{
		// Success
		if (cache)
		{
			(*cache)[ propertyId ] = o_PropertyAccessor.getData();
		}
		return;
	}

	auto newPath = path + propertyName.length();
	if (*newPath == DOT_OPERATOR)
	{
		++newPath;
		if (*newPath == DOT_OPERATOR || *newPath == Collection::getIndexOpen() || *newPath == Collection::getIndexClose())
		{
			// Fail: unexpected character
			o_PropertyAccessor.setBaseProperty(nullptr);
			return;
		}

		Variant newBase = o_PropertyAccessor.getValue();
		ObjectHandle newObject;
		if (!newBase.tryCast(newObject))
		{
			// Fail: unexpected type
			o_PropertyAccessor.setBaseProperty(nullptr);
			return;
		}

		/* BEGIN TEMP */
		std::shared_ptr<ObjectReference> reference = std::dynamic_pointer_cast<ObjectReference>(newObject.storage());
		TF_ASSERT(reference);
		/* END TEMP */

		newObject = reflectedRoot(newObject, *defManager_);
		auto subDefinition = defManager_->getDefinition(newObject);
		if (!subDefinition)
		{
			// Fail: not a reflected type
			o_PropertyAccessor.setBaseProperty(nullptr);
			return;
		}

		o_PropertyAccessor = subDefinition->bindProperty(newPath, newObject);
		if (cache && o_PropertyAccessor.isValid())
		{
			(*cache)[ propertyId ] = o_PropertyAccessor.getData();
		}
		return;
	}
	else if (*newPath == Collection::getIndexOpen())
	{
		Variant newBase = o_PropertyAccessor.getValue();
		// recurse
		bindProperty(definition, newPath, newBase, o_PropertyAccessor);
		if (cache && o_PropertyAccessor.isValid())
		{
			(*cache)[propertyId] = o_PropertyAccessor.getData();
		}
		return;
	}

	// Fail: unexpected character
	o_PropertyAccessor.setBaseProperty(nullptr);
}

std::shared_ptr<ObjectReference> ClassDefinition::getChildReference(
	const std::shared_ptr<ObjectReference>& parent, const char* path, const Variant& value) const
{
	TF_ASSERT(parent);
	TF_ASSERT(path && *path);
	auto objectManager = impl_->defManager_->getObjectManager();
	TF_ASSERT(objectManager);

	auto childReference = objectManager->getObject(parent->id(), path);
	objectManager->updateObjectStorage(childReference, value);
	return childReference;
}

//==============================================================================
IBasePropertyPtr ClassDefinition::findProperty(const char* name, size_t length) const
{
	// Some definitions allow you to lookup by name directly
	auto & details = getDetails();
	if (details.canDirectLookupProperty())
	{
		std::string propName(name, length);

		auto target = &details;
		IBasePropertyPtr prop = details.directLookupProperty(propName.c_str());
		if (prop != nullptr)
		{
			return prop;
		}
		const auto & parentNames = getParentNames();

		for (const auto & parentName : parentNames)
		{
			auto parentDef =
				impl_->defManager_->getDefinition(parentName.c_str());
			if (parentDef == nullptr)
			{
				continue;
			}
			prop = parentDef->findProperty(propName.c_str());
			if (prop != nullptr)
			{
				return prop;
			}
		}
		return nullptr;
	}

	// Otherwise, perform a search
	auto nameHash = HashUtilities::compute(name, length);
	auto properties = allProperties();
	for (auto it = properties.begin(); it != properties.end(); ++it)
	{
		if (it->getNameHash() == nameHash)
		{
			return *it;
		}
	}
	return nullptr;
}


//------------------------------------------------------------------------------
IBasePropertyPtr ClassDefinition::findProperty(IPropertyPath::ConstPtr & path) const
{
	return impl_->findProperty(path);
}


//------------------------------------------------------------------------------
bool ClassDefinition::isGeneric() const
{
	return getDetails().isGeneric();
}

//------------------------------------------------------------------------------
bool ClassDefinition::canBeCastTo(const IClassDefinition& definition) const
{
	// Assuming definitions are shared we only need to check the pointer
	if (this == &definition)
	{
		return true;
	}

	auto && metaBasesHolder = impl_->getMetaBasesHolder();
	if (metaBasesHolder == nullptr)
	{
		return false;
	}
	const auto & parents = metaBasesHolder->getParents();
	for (const auto & parent : parents)
	{
		auto baseDef =
			getDefinitionManager()->getDefinition(parent.c_str());
		if (baseDef->canBeCastTo(definition))
		{
			return true;
		}
	}
	return false;
}

//------------------------------------------------------------------------------
void* ClassDefinition::castTo(const IClassDefinition& definition, void* object) const
{
	// Assuming definitions are shared we only need to check the pointer
	if (this == &definition)
	{
		return object;
	}

	auto metaBasesHolder = findFirstMetaData< MetaBasesHolderObj >(
		*this, *getDefinitionManager());
	if (metaBasesHolder == nullptr)
	{
		return nullptr;
	}
	const auto & parents = metaBasesHolder->getParents();
	const auto & casters = metaBasesHolder->getCasters();
	size_t i = 0;
	for (const auto & parent : parents)
	{
		auto baseDef =
			getDefinitionManager()->getDefinition(parent.c_str());
		if (baseDef == nullptr)
		{
			continue;
		}
		if (baseDef->canBeCastTo(definition))
		{
			object = casters[i](object);
			return baseDef->castTo(definition, object);
		}
		++i;
	}
	return nullptr;
}

//------------------------------------------------------------------------------
ObjectHandleStoragePtr ClassDefinition::createObjectStorage() const
{
	return impl_->details_->createObjectStorage(*this);
}

//------------------------------------------------------------------------------
ManagedObjectPtr ClassDefinition::createManaged(RefObjectId id) const
{
	return impl_->details_->createManaged(*this, id);
}

//------------------------------------------------------------------------------
ObjectHandle ClassDefinition::createShared(RefObjectId id) const
{
	impl_->sharedObjects_.push_back(createManaged(id));
	return impl_->sharedObjects_.back()->getHandle();
}

//------------------------------------------------------------------------------
const char* ClassDefinition::getName() const
{
	return impl_->details_->getName();
}

//------------------------------------------------------------------------------
const MetaData & ClassDefinition::getMetaData() const
{
	return impl_->details_->getMetaData();
}

//------------------------------------------------------------------------------
void ClassDefinition::setDefinitionManager(IDefinitionManager* defManager)
{
	impl_->defManager_ = defManager;
}

//------------------------------------------------------------------------------
IDefinitionManager* ClassDefinition::getDefinitionManager() const
{
	return impl_->defManager_;
}

} // end namespace wgt
