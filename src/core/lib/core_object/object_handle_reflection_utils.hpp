#ifndef OBJECT_HANDLE_STORAGE_REFLECTED_CAST_HPP
#define OBJECT_HANDLE_STORAGE_REFLECTED_CAST_HPP

#include "core_object/i_object_handle_storage.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include <cassert>
#include <type_traits>
#include <typeinfo>

namespace wgt
{
class RefObjectId;
class IParticleEffectTreeModel;
class IClassDefinition;
class IDefinitionManager;

class ObjectHandleStorageReflected : public IObjectHandleStorage
{
public:
	ObjectHandleStorageReflected(const ObjectHandleStoragePtr& storage,
								 const TypeId& typeId,
								 const IDefinitionManager& definitionManager)
		: storage_(storage), typeId_(typeId), definitionManager_(definitionManager)
	{
	}

	virtual bool isValid() const override
	{
		return storage_ != nullptr;
	}

	virtual void* data() const override
	{
		if (storage_ == nullptr)
		{
			return nullptr;
		}

		return cast(storage_->data(), storage_->type(), typeId_, definitionManager_);
	}

	virtual TypeId type() const override
	{
		return typeId_;
	}

	virtual bool getId(RefObjectId& id) const override
	{
		if (storage_ == nullptr)
		{
			return false;
		}

		return storage_->getId(id);
	}

	virtual ObjectHandleStoragePtr inner() const override
	{
		return storage_;
	}

	virtual void setParent(const std::shared_ptr<IObjectHandleStorage>& storage, const std::string& path) override
	{
		if (storage_ == nullptr)
		{
			return;
		}

		storage_->setParent(storage, path);
	}

	virtual std::shared_ptr<IObjectHandleStorage> parent() const override
	{
		if (storage_ == nullptr)
		{
			return nullptr;
		}

		return storage_->parent();
	}

	std::string path() const
	{
		if (storage_ == nullptr)
		{
			return "";
		}

		return storage_->path();
	}

	static void* cast(void* source, const TypeId& typeIdSource, const TypeId& typeIdDest, const IDefinitionManager& definitionManager)
	{
		char* pRaw = static_cast<char*>(source);
		if (pRaw == nullptr)
		{
			return pRaw;
		}

		if (typeIdSource == typeIdDest)
		{
			return pRaw;
		}

		auto srcDefinition = definitionManager.getDefinition(typeIdSource.getName());
		if (srcDefinition != nullptr)
		{
			auto dstDefinition = definitionManager.getDefinition(typeIdDest.getName());
			if (dstDefinition != nullptr && srcDefinition->canBeCastTo(*dstDefinition))
			{
				auto result = srcDefinition->castTo(*dstDefinition, pRaw);
				assert(result != nullptr);
				return result;
			}
			else
			{
				return nullptr;
			}
		}

		return nullptr;
	}

	static ObjectHandle cast(const ObjectHandle& other, const TypeId& typeIdDest, const IDefinitionManager& definitionManager)
	{
		ObjectHandleStoragePtr storage =
		std::make_shared<ObjectHandleStorageReflected>(other.storage(), typeIdDest, definitionManager);
		return ObjectHandle(storage);
	}

	static ObjectHandle root(const ObjectHandle& source, const IDefinitionManager& definitionManager)
	{
		if (!source.isValid())
		{
			return source;
		}

		auto root = source.storage();
		auto reflectedRoot = definitionManager.getObjectDefinition(root) != nullptr ? root : nullptr;
		for (;;)
		{
			auto inner = root->inner();
			if (inner == nullptr)
			{
				break;
			}
			root = inner;
			reflectedRoot = definitionManager.getObjectDefinition(root) != nullptr ? root : nullptr;
		}
		return ObjectHandle(reflectedRoot);
	}

private:
	ObjectHandleStoragePtr storage_;
	TypeId typeId_;
	const IDefinitionManager& definitionManager_;
};

inline void* reflectedCast(void* source, const TypeId& typeIdSource, const TypeId& typeIdDest, const IDefinitionManager& definitionManager)
{
	return ObjectHandleStorageReflected::cast(source, typeIdSource, typeIdDest, definitionManager);
}

inline ObjectHandle reflectedCast(const ObjectHandle& other, const TypeId& typeIdDest, const IDefinitionManager& definitionManager)
{
	return ObjectHandleStorageReflected::cast(other, typeIdDest, definitionManager);
}

inline ObjectHandle reflectedRoot(const ObjectHandle& source, const IDefinitionManager& definitionManager)
{
	return ObjectHandleStorageReflected::root(source, definitionManager);
}

template <typename T>
ObjectHandleT<T> reflectedCast(const ObjectHandle& other, const IDefinitionManager& definitionManager)
{
	return reinterpretCast<T>(reflectedCast(other, TypeId::getType<T>(), definitionManager));
}

template <typename T>
T* reflectedCast(void* source, const TypeId& typeIdSource, const IDefinitionManager& definitionManager)
{
	return reinterpret_cast<T*>(reflectedCast(source, typeIdSource, TypeId::getType<T>(), definitionManager));
}

} // end namespace wgt
#endif // OBJECT_HANDLE_STORAGE_HPP
