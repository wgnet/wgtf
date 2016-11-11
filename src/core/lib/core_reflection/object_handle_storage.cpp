#include "object_handle_storage.hpp"
#include "object_handle.hpp"

namespace wgt
{
ObjectHandleStorageReflectedCast::ObjectHandleStorageReflectedCast(const std::shared_ptr<IObjectHandleStorage>& storage,
                                                                   const TypeId& typeId,
                                                                   const IDefinitionManager& definitionManager)
    : storage_(storage), typeId_(typeId), definitionManager_(definitionManager)
{
}

void* ObjectHandleStorageReflectedCast::data() const
{
	if (storage_ == nullptr)
	{
		return nullptr;
	}

	return reflectedCast(storage_->data(), storage_->type(), typeId_, definitionManager_);
}

TypeId ObjectHandleStorageReflectedCast::type() const
{
	return typeId_;
}

std::shared_ptr<IObjectHandleStorage> ObjectHandleStorageReflectedCast::inner() const
{
	return storage_;
}

bool ObjectHandleStorageReflectedCast::getId(RefObjectId& id) const
{
	if (storage_ == nullptr)
	{
		return false;
	}

	return storage_->getId(id);
}
} // end namespace wgt
