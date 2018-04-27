#include "managed_object_base.hpp"

#include "core_common/assert.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/i_object_manager.hpp"
#include "core_reflection/metadata/meta_base.hpp"
#include "core_reflection/metadata/meta_impl.hpp"
#include "core_reflection/object/object_reference.hpp"
#include "core_reflection/private/internal_context_query.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"

namespace wgt
{

//------------------------------------------------------------------------------
ManagedObjectBase::ManagedObjectBase(
	const ObjectHandleStoragePtr& storage, const RefObjectId& id)
{
	TF_ASSERT(storage);

	auto & definitionManager = ReflectionShared::getDefinitionManager();
	auto definition = definitionManager.getObjectDefinition(storage);
	TF_ASSERT(definition && "ManagedObject requires a registered definition");

	if (definition)
	{
		RefObjectId objectId = id;
		if (id == RefObjectId::zero())
		{
			auto metaUniqueId = findFirstMetaData<MetaUniqueIdObj>(*definition, definitionManager);
			if (metaUniqueId != nullptr)
			{
				objectId = std::string(metaUniqueId->getId());
			}
		}

		auto objectManager = 
			ReflectionShared::getComponentContext().queryInterface< IObjectManager >();
		TF_ASSERT(objectManager);
		std::tie(storage_, reference_) = objectManager->registerObject(storage, objectId);
	}
}


//------------------------------------------------------------------------------
ManagedObjectBase::ManagedObjectBase(const std::nullptr_t& )
{
}

//------------------------------------------------------------------------------
ManagedObjectBase::ManagedObjectBase(ManagedObjectBase&& rhs)
	: storage_(std::move(rhs.storage_))
	, reference_(std::move(rhs.reference_))
{
}

ManagedObjectBase::~ManagedObjectBase()
{
}


//------------------------------------------------------------------------------
ManagedObjectBase & ManagedObjectBase::operator=(ManagedObjectBase && rhs)
{
	if (this != &rhs)
	{
		storage_ = std::move(rhs.storage_);
		reference_ = std::move(rhs.reference_);
	}
	return *this;
}


//------------------------------------------------------------------------------
ManagedObjectBase & ManagedObjectBase::operator=(const std::nullptr_t&)
{
	storage_.reset();
	reference_.reset();
	return *this;
}

//------------------------------------------------------------------------------
bool ManagedObjectBase::operator==(const ManagedObjectBase & obj) const
{
	return getStoragePointer() == obj.getStoragePointer();
}

//------------------------------------------------------------------------------
bool ManagedObjectBase::operator==(const std::nullptr_t&) const
{
	return getStoragePointer() == nullptr;
}


//------------------------------------------------------------------------------
bool ManagedObjectBase::operator!=(const std::nullptr_t&) const
{
	return getStoragePointer() != nullptr;
}


//------------------------------------------------------------------------------
bool ManagedObjectBase::set(std::shared_ptr<RootObjectReference>&& reference, std::shared_ptr<ObjectStorage>&& storage)
{
	if (reference->type() != type())
	{
		return false;
	}

	reference_ = std::move(reference);
	storage_ = std::move(storage);
	return true;
}


//------------------------------------------------------------------------------
void * ManagedObjectBase::getStoragePointer() const
{
	if (!storage_)
	{
		return nullptr;
	}
	return storage_->storage()->data();
}


//------------------------------------------------------------------------------
ObjectHandle ManagedObjectBase::getHandle() const
{
	return reference_ ? ObjectReference::asHandle(std::static_pointer_cast<ObjectReference>(reference_)) : nullptr;
}


//------------------------------------------------------------------------------
Variant ManagedObjectBase::getVariant() const
{
	return getHandle();
}


//------------------------------------------------------------------------------
const IDefinitionManager * ManagedObjectBase::getDefinitionManager() const
{
	return
		ReflectionShared::getComponentContext().queryInterface< IDefinitionManager >();
}

} // end namespace wgt