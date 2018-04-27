#include "property_accessor.hpp"

#include "interfaces/i_base_property.hpp"
#include "reflected_object.hpp"
#include "reflected_method.hpp"
#include "reflected_method_parameters.hpp"
#include "object_handle.hpp"
#include "property_accessor_listener.hpp"
#include "i_definition_manager.hpp"
#include "utilities/reflection_utilities.hpp"
#include "metadata/meta_base.hpp"
#include "core_reflection/base_property_with_metadata.hpp"
#include "core_reflection/i_object_manager.hpp"
#include "core_reflection/object/object_reference.hpp"
#include <unordered_map>
#include "core_variant/variant.hpp"
#include "metadata/meta_utilities.hpp"
#include "metadata/meta_impl.hpp"
#include "core_logging/logging.hpp"
#include "core_common/assert.hpp"
#include "private/property_accessor_data.hpp"

namespace wgt
{


//==============================================================================
PropertyAccessor::PropertyAccessor()
{
}


//------------------------------------------------------------------------------
PropertyAccessor::PropertyAccessor(PropertyAccessor&& other)
    : data_( std::move( other.data_ ) )
{
}


//------------------------------------------------------------------------------
PropertyAccessor::PropertyAccessor(const PropertyAccessor& other)
    : data_( std::make_shared<PropertyAccessorPrivate::Data>( *other.data_ ) )
{
}


//------------------------------------------------------------------------------
PropertyAccessor::PropertyAccessor(
	const char* path, const std::shared_ptr<ObjectReference>& reference)
	: data_( std::make_shared< PropertyAccessorPrivate::Data >( path, reference ) )
{
}


//------------------------------------------------------------------------------
PropertyAccessor::~PropertyAccessor()
{
}


//------------------------------------------------------------------------------
PropertyAccessor& PropertyAccessor::operator=(const PropertyAccessor& other)
{
	if (this == &other)
	{
		return *this;
	}
	if (other.data_ == nullptr)
	{
		return *this;
	}

	data_ = std::make_shared< PropertyAccessorPrivate::Data >( *other.data_ );
	return *this;
}

//==============================================================================
PropertyAccessor& PropertyAccessor::operator=(PropertyAccessor&& other)
{
	data_ = std::move( other.data_ );
	return *this;
}

//==============================================================================
bool PropertyAccessor::isValid() const
{
	return data_ != nullptr && data_->object_.isValid() && getProperty() != nullptr;
}

//==============================================================================
const TypeId& PropertyAccessor::getType() const
{
	if (!isValid())
	{
		static auto s_voidType = TypeId::getType<void>();
		return s_voidType;
	}
	return getProperty()->getType();
}

//==============================================================================
bool PropertyAccessor::canSetValue() const
{
	if (!this->isValid())
	{
		return false;
	}

	if (data_->get< IDefinitionManager >() == nullptr)
	{
		return false;
	}

	return !this->getProperty()->readOnly(getObject());
}

//==============================================================================
void PropertyAccessor::notify() const
{
	// Intentionally copy cause set of listeners can change between preSet and postSet
	auto listeners = data_->get< IDefinitionManager >()->getPropertyAccessorListeners();
	auto itBegin = listeners.cbegin();
	auto itEnd = listeners.cend();
	Variant value = getValue();
	for (auto it = itBegin; it != itEnd; ++it)
	{
		auto listener = *it;
		TF_ASSERT(listener != nullptr);
		listener->preSetValue(*this, value);
	}

	for (auto it = itBegin; it != itEnd; ++it)
	{
		auto listener = *it;
		TF_ASSERT(listener != nullptr);
		listener->postSetValue(*this, value);
	}
}

//==============================================================================
bool PropertyAccessor::setValue(const Variant& value) const
{
	if (!this->canSetValue())
	{
		return false;
	}

	// Intentionally copy cause set of listeners can change between preSet and postSet
	auto listeners = data_->get< IDefinitionManager >()->getPropertyAccessorListeners();
	auto itBegin = listeners.cbegin();
	auto itEnd = listeners.cend();
	for (auto it = itBegin; it != itEnd; ++it)
	{
		auto listener = *it;
		TF_ASSERT(listener != nullptr);
		listener->preSetValue(*this, value);
	}
	bool ret = getProperty()->set(data_->object_, value, *data_->get< IDefinitionManager >());

	// Set the parent object to support properties returned by value
	propagateSetToAncestors();

	for (auto it = itBegin; it != itEnd; ++it)
	{
		auto listener = *it;
		TF_ASSERT(listener != nullptr);
		listener->postSetValue(*this, value);
	}
	return ret;
}

//==============================================================================
bool PropertyAccessor::setValueWithoutNotification(const Variant& value) const
{
	if (!this->canSetValue())
	{
		return false;
	}

	if (!getProperty()->set(data_->object_, value, *data_->get< IDefinitionManager >()))
	{
		return false;
	}

	propagateSetToAncestors();
	return true;
}

//==============================================================================
bool PropertyAccessor::canInvoke() const
{
	if (!this->isValid())
	{
		return false;
	}

	if (data_->get< IDefinitionManager >()== nullptr)
	{
		return false;
	}

	return this->getProperty()->isMethod();
}

//==============================================================================
Variant PropertyAccessor::invoke(const ReflectedMethodParameters& parameters) const
{
	Variant result;

	if (!this->canInvoke())
	{
		return result;
	}

	auto listeners = data_->get< IDefinitionManager >()->getPropertyAccessorListeners();

	for (auto it = listeners.cbegin(); it != listeners.cend(); ++it)
	{
		auto listener = *it;
		TF_ASSERT(listener != nullptr);
		listener->preInvoke(*this, parameters, false);
	}

	result = getProperty()->invoke(
		data_->object_, *data_->get< IDefinitionManager >(), parameters);

	for (auto it = listeners.cbegin(); it != listeners.cend(); ++it)
	{
		auto listener = *it;
		TF_ASSERT(listener != nullptr);
		listener->postInvoke(*this, result, false);
	}

	return result;
}

//==============================================================================
void PropertyAccessor::invokeUndoRedo(const ReflectedMethodParameters& parameters, Variant result, bool undo) const
{
	if (!this->canInvoke())
	{
		return;
	}

	auto method = dynamic_cast<ReflectedMethod*>(getProperty().get());
	if (method == nullptr)
	{
		// Check if a method with meta data
		auto methodWithMetaData = dynamic_cast<BasePropertyWithMetaData*>(getProperty().get());
		if (methodWithMetaData != nullptr && methodWithMetaData->baseProperty())
		{
			method = dynamic_cast<ReflectedMethod*>(methodWithMetaData->baseProperty().get());
		}
	}

	if (method == nullptr)
	{
		NGT_WARNING_MSG("Could not invoke undo/redo for property");
		return;
	}

	auto listeners = data_->get<IDefinitionManager >()->getPropertyAccessorListeners();
	for (auto it = listeners.cbegin(); it != listeners.cend(); ++it)
	{
		auto listener = *it;
		// What does this assertion mean? Isn't this to be expected sometimes?
		// I encountered this assertion during shutdown. @m_martin
		TF_ASSERT(listener != nullptr);
		listener->preInvoke(*this, parameters, undo);
	}

	method = undo ? method->getUndoMethod() : method->getRedoMethod();
	TF_ASSERT(method != nullptr);
	ReflectedMethodParameters paramsUndoRedo;
	//TODO: push back each parameter one by one
	/*for (int i = 0; i < parameters.size(); ++i)
	{
		paramsUndoRedo.push_back(parameters[i]);
	}*/
	paramsUndoRedo.push_back(parameters);
	paramsUndoRedo.push_back(result);
	method->invoke(
		data_->object_, *data_->get< IDefinitionManager >(), paramsUndoRedo);

	for (auto it = listeners.cbegin(); it != listeners.cend(); ++it)
	{
		auto listener = *it;
		TF_ASSERT(listener != nullptr);
		listener->postInvoke(*this, result, undo);
	}
}

//==============================================================================
bool PropertyAccessor::canInsert() const
{
	Collection collection;
	auto thisValue = getValue();
	if (!thisValue.tryCast(collection))
	{
		return false;
	}

	return collection.canResize();
}

//==============================================================================
bool PropertyAccessor::insert(const Variant& key, const Variant& value) const
{
	Collection collection;
	auto thisValue = getValue();
	if (!thisValue.tryCast(collection))
	{
		return false;
	}

	if (!collection.canResize())
	{
		return false;
	}

	auto listeners = data_->get< IDefinitionManager >()->getPropertyAccessorListeners();
	auto itBegin = listeners.cbegin();
	auto itEnd = listeners.cend();

	auto preInsert = collection.connectPreInsert([&](Collection::Iterator pos, size_t count) {
		auto index = std::distance(collection.begin(), pos);
		for (auto it = itBegin; it != itEnd; ++it)
		{
			auto listener = *it;
			TF_ASSERT(listener != nullptr);
			listener->preInsert(*this, index, count);
		}
	});
	auto postInserted = collection.connectPostInserted([&](Collection::Iterator pos, size_t count) {
		auto index = std::distance(collection.begin(), pos);
		for (auto it = itBegin; it != itEnd; ++it)
		{
			auto listener = *it;
			TF_ASSERT(listener != nullptr);
			listener->postInserted(*this, index, count);
		}
	});

	auto it = collection.insertValue(key, value);

	preInsert.disconnect();
	postInserted.disconnect();

	setValueWithoutNotification(collection);

	return it != collection.end();
}

//==============================================================================
bool PropertyAccessor::canErase() const
{
	Collection collection;
	auto thisValue = getValue();
	if (!thisValue.tryCast(collection))
	{
		return false;
	}

	return collection.canResize();
}

//==============================================================================
bool PropertyAccessor::erase(const Variant& key) const
{
	Collection collection;
	auto thisValue = getValue();
	if (!thisValue.tryCast(collection))
	{
		return false;
	}

	if (!collection.canResize())
	{
		return false;
	}

	auto listeners = data_->get< IDefinitionManager >()->getPropertyAccessorListeners();
	auto itBegin = listeners.cbegin();
	auto itEnd = listeners.cend();

	auto preErase = collection.connectPreErase([&](Collection::Iterator pos, size_t count) {
		auto index = std::distance(collection.begin(), pos);
		for (auto it = itBegin; it != itEnd; ++it)
		{
			auto listener = *it;
			TF_ASSERT(listener != nullptr);
			listener->preErase(*this, index, count);
		}
	});
	auto postErased = collection.connectPostErased([&](Collection::Iterator pos, size_t count) {
		auto index = std::distance(collection.begin(), pos);
		
		auto objectManager = data_->get< IDefinitionManager >()->getObjectManager();
		auto rootObject = getRootObject();

		for (size_t i = 0; i < count; ++i)
		{
			auto fullPath = getFullPath() + std::string("[") + std::to_string(index + i) + "]";
			auto rootReference = std::dynamic_pointer_cast<ObjectReference>(rootObject.storage());
			TF_ASSERT(rootReference);
			auto childReference = objectManager->getObject( data_->rootReference_->id(), fullPath);
			objectManager->clearStorageRecursively(childReference);
		}

		for (auto it = itBegin; it != itEnd; ++it)
		{
			auto listener = *it;
			TF_ASSERT(listener != nullptr);
			listener->postErased(*this, index, count);
		}
	});

	auto count = collection.eraseKey(key);

	preErase.disconnect();
	postErased.disconnect();

	setValueWithoutNotification(collection);

	return count > 0;
}

//==============================================================================
const char* PropertyAccessor::getName() const
{
	if (!isValid())
	{
		return nullptr;
	}
	return getProperty()->getName();
}

//==============================================================================
const IClassDefinition* PropertyAccessor::getStructDefinition() const
{
	if (!isValid())
	{
		return nullptr;
	}
	auto defManager = data_->get< IDefinitionManager >();
	if (defManager == nullptr)
	{
		return nullptr;
	}
	Collection collection;
	bool isCollection = getValue().tryCast(collection);
	if (isCollection)
	{
		return defManager->getDefinition(collection.valueType().getName());
	}
	return defManager->getDefinition(getProperty()->getType().getName());
}

bool PropertyAccessor::canGetValue() const
{
	if (!this->isValid())
	{
		return false;
	}

	if (data_->get< IDefinitionManager >() == nullptr)
	{
		return false;
	}

	return this->getProperty()->isValue();
}

//==============================================================================
Variant PropertyAccessor::getValue() const
{
	auto defManager = data_->get< IDefinitionManager >();
	if (!this->isValid() || defManager == nullptr)
	{
		return Variant();
	}

	TF_ASSERT(this->canGetValue());

	// TODO: early out if a child reference already exists and the existing storage is by value for this property

	Variant variant = getProperty()->get(data_->object_, *defManager);

	if (getProperty()->isCollection())
	{
		return variant;
	}

	static TypeId objectType = TypeId::getType<ObjectHandle>();
	auto typeId = variant.type()->typeId();
	if (typeId == objectType)
	{
		// property is a link
		return variant;
	}

	auto objectManager = data_->get< IObjectManager >();
	auto tempObject = ObjectHandle(objectManager->createObjectStorage(variant));
	auto definition = defManager->getObjectDefinition(tempObject);
	if (definition != nullptr)
	{
		auto rootObject = getRootObject();
		auto fullPath = getFullPath();

		auto rootReference = std::dynamic_pointer_cast<ObjectReference>(rootObject.storage());
		TF_ASSERT(rootReference);
		auto childReference = objectManager->getObject(data_->rootReference_->id(), fullPath);

		if(getProperty()->isByReference())
		{
			// Always update the object storage when data is returned by reference
			// Example: Edit a material and toggle the BLEND_TERRAIN flag. When reloaded the material adds new parameters
			// The storage of the parameters must be update as the references to the elements has changed
			objectManager->updateObjectStorage(childReference, variant);
		}
		else
		{
			auto childReferenceDefinition = defManager->getObjectDefinition(ObjectReference::asHandle(childReference));
			const char* currentType = childReferenceDefinition ? childReferenceDefinition->getName() : "";
			const char* newType = getProperty()->getType().getName();
			if (childReference->storage() == nullptr || strcmp(currentType, newType) != 0)
			{
				// For properties that return by value don't update object storage if it already exists
				// as this would invalidate sub references to the existing storage
				// however we must update if the object is set to the new type
				objectManager->updateObjectStorage(childReference, variant);
			}
		}

		return ObjectReference::asHandle(childReference);
	}

	return variant;
}

//==============================================================================
IBasePropertyPtr PropertyAccessor::getProperty() const
{
	return data_->property_;
}

//==============================================================================
const ObjectHandle& PropertyAccessor::getObject() const
{
	return data_->object_;
}

//==============================================================================
const char * PropertyAccessor::getPath() const
{
	return data_->path_.c_str();
}

//==============================================================================
void PropertyAccessor::setObjectReference(const std::shared_ptr<ObjectReference>& reference)
{
	data_->reference_ = reference;
	data_->object_ = ObjectReference::asHandle(data_->reference_);
}

//==============================================================================
void PropertyAccessor::setPath(const std::string& path)
{
	data_->path_ = path;
}

//==============================================================================
const MetaData & PropertyAccessor::getMetaData() const
{
	if (getProperty() == nullptr)
	{
		static MetaData s_Empty;
		return s_Empty;
	}
	return getProperty()->getMetaData();
}

//==============================================================================
void PropertyAccessor::setBaseProperty(const IBasePropertyPtr& property)
{
	data_->property_ = property;
}

//==============================================================================
const ObjectHandle & PropertyAccessor::getRootObject() const
{
	if (data_->rootObject_.isValid())
	{
		return data_->rootObject_;
	}

	data_->rootReference_ = ObjectReference::rootReference(data_->reference_);
	data_->rootObject_ = ObjectReference::asHandle(data_->rootReference_);
	return data_->rootObject_;
}

//==============================================================================
const char* PropertyAccessor::getFullPath() const
{
	if (!data_->fullPath_.empty())
	{
		return data_->fullPath_.c_str();
	}

	data_->reference_->getFullPath(data_->fullPath_);

	if (!data_->fullPath_.empty() && !data_->path_.empty() && data_->path_[0] != '[')
	{
		data_->fullPath_.push_back('.');
	}

	data_->fullPath_ += data_->path_;
	return data_->fullPath_.c_str();
}

//==============================================================================
const IDefinitionManager* PropertyAccessor::getDefinitionManager() const
{
	return data_->get< IDefinitionManager >();
}

void PropertyAccessor::propagateSetToAncestors() const
{
	auto defManager = data_->get< IDefinitionManager >();
	TF_ASSERT(defManager != nullptr);

	std::shared_ptr<ObjectReference> parent = data_->reference_->parentReference();
	std::string path = data_->reference_->path();
	std::shared_ptr<ObjectReference> child = data_->reference_;
	Collection* collection;
	Variant value;
	const IClassDefinition* definition;
	IBasePropertyPtr property;

	while (parent && !path.empty())
	{
		ObjectHandle parentHandle(ObjectReference::asHandle(parent));
		collection = child->get<Collection>();
		collection == nullptr ? value = ObjectReference::asHandle(child) : value = *collection;
		definition = defManager->getDefinition(parentHandle);
		property = std::get<0>(ReflectionUtilities::parseProperty(path.c_str(), parentHandle, nullptr, definition, *defManager));
		TF_ASSERT(property.get() != nullptr);

		if (!property->isByReference())
		{
			property->set(parentHandle, value, *defManager);
		}

		auto callback = findFirstMetaData<MetaCallbackObj>(*property, *defManager);
		if (callback != nullptr)
		{
			callback->invoke(parentHandle);
		}

		child = parent;
		path = child->path();
		parent = child->parentReference();
	}
}

//------------------------------------------------------------------------------
PropertyAccessor::PropertyAccessor( std::shared_ptr< PropertyAccessorPrivate::Data > & data)
	: data_( data )
{
}


//------------------------------------------------------------------------------
std::shared_ptr< PropertyAccessorPrivate::Data > & PropertyAccessor::getData()
{
	return data_;
}


} // end namespace wgt
