#include "reflection_controller.hpp"
#include "core_dependency_system/depends.hpp"
#include "core_common/assert.hpp"
#include "core_command_system/i_command_manager.hpp"
#include "commands/custom_command.hpp"
#include "commands/set_reflectedproperty_command.hpp"
#include "commands/invoke_reflected_method_command.hpp"
#include "commands/reflected_collection_insert_command.hpp"
#include "commands/reflected_collection_erase_command.hpp"
#include "core_reflection/property_accessor.hpp"
#include "core_reflection/i_object_manager.hpp"
#include "core_reflection/reflected_method_parameters.hpp"
#include "core_reflection/metadata/meta_utilities.hpp"
#include "core_reflection/metadata/meta_impl.hpp"
#include "core_reflection/base_property_with_metadata.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/reflected_method.hpp"
#include "core/interfaces/editor/i_editor.hpp"

#include <map>

namespace wgt
{
class ReflectionController::Impl : public ICommandEventListener
{
public:
	Impl(ICommandManager& commandManager) : commandManager_(commandManager), editor_(nullptr)
	{
		commandManager_.registerCommandStatusListener(this);
	}

	~Impl()
	{
		commandManager_.deregisterCommandStatusListener(this);
	}

	Variant getValue(const PropertyAccessor& pa)
	{
		// This createKey is added as a work-around for objects that were
		// not registered with the IObjectManager properly on creation.
		//
		// Objects need a key to be shared with other plugins.
		// E.g. getValue() will be sharing your object with plg_command_manager.
		//
		// Better to register your object with IObjectManager::registerObject()
		// or IObjectManager::registerUnmanagedObject() when you create the
		// ObjectHandle.
		//
		// @see IObjectManager
		Key key;
		if (!createKey(pa, key))
		{
			return pa.getValue();
		}

		// TODO: assert access is only on the main thread
		auto range = commands_.equal_range(key);
		for (auto it = range.first; it != range.second; ++it)
		{
			auto instance = it->second;
			commandManager_.waitForInstance(instance);
		}
		commands_.erase(range.first, range.second);

		return pa.getValue();
	}

	void setValue(const PropertyAccessor& pa, const Variant& data)
	{
		if (editor_)
		{
			editor_->updateEditState(pa.getObject());
		}

		if (findFirstMetaData<MetaDirectInvokeObj>(pa, *pa.getDefinitionManager()) != nullptr)
		{
			pa.setValue(data);
			return;
		}

		// Check if custom set function processes this case.
		if (setFunc_ && setFunc_(pa, data))
		{
			return;
		}

		Key key;
		if (!createKey(pa, key))
		{
			pa.setValue(data);
			return;
		}

		// Access is only on the main thread
		TF_ASSERT(std::this_thread::get_id() == commandManager_.ownerThreadId());

        auto commandArgs = ManagedObject<ReflectedPropertyCommandArgument>::make_iunique_fn(
            [&key, &data](ReflectedPropertyCommandArgument& commandArgs)
        {
            commandArgs.setContextId(key.first);
            commandArgs.setPath(key.second.c_str());
            commandArgs.setValue(data);
        });

        const auto commandId = getClassIdentifier<SetReflectedPropertyCommand>();
        auto command = commandManager_.queueCommand(commandId, std::move(commandArgs));

		// Queuing may cause it to execute straight away
		// Based on the thread affinity of SetReflectedPropertyCommand
		if (!command->isComplete())
		{
			commands_.emplace(std::pair<Key, CommandInstancePtr>(key, command));
		}
	}

	Variant invoke(const PropertyAccessor& pa, const ReflectedMethodParameters& parameters)
	{
		if (findFirstMetaData<MetaDirectInvokeObj>(pa, *pa.getDefinitionManager()) != nullptr)
		{
			return pa.invoke(parameters);
		}

		Key key;
		if (!createKey(pa, key))
		{
			return pa.invoke(parameters);
		}

		auto classId = getClassIdentifier<InvokeReflectedMethodCommand>();
		auto args = ManagedObject<ReflectedMethodCommandParameters>::make_unique();
		(*args)->setId(key.first);
		(*args)->setPath(key.second.c_str());
		(*args)->setParameters(parameters);
		const auto itr = commands_.emplace(std::pair<Key, CommandInstancePtr>(
			key, commandManager_.queueCommand(classId, ManagedObjectPtr(std::move(args)))));

		commandManager_.waitForInstance(itr->second);
		Variant returnValueObject = itr->second.get()->getReturnValue();
		commands_.erase(itr);
		return returnValueObject;
	}

	void insert(const PropertyAccessor& pa, const Variant& insertKey, const Variant& value)
	{
		if (editor_)
		{
			editor_->updateEditState(pa.getObject());
		}

		// Check if custom insert function processes this case.
		if (insertFunc_ && insertFunc_(pa, insertKey, value))
		{
			return;
		}

		Key key;
		if (!createKey(pa, key))
		{
			pa.insert(insertKey, value);
			return;
		}

		auto classId = getClassIdentifier<ReflectedCollectionInsertCommand>();
		auto args = ManagedObject<ReflectedCollectionInsertCommandParameters>::make_unique();
		(*args)->id_ = key.first;
		(*args)->path_ = key.second;
		(*args)->key_ = insertKey;
		(*args)->value_ = value;
		const auto itr = commands_.emplace(std::pair<Key, CommandInstancePtr>(
			key, commandManager_.queueCommand(classId, ManagedObjectPtr(std::move(args)))));

		commandManager_.waitForInstance(itr->second);
		commands_.erase(itr);
	}

	void erase(const PropertyAccessor& pa, const Variant& eraseKey)
	{
		if (editor_)
		{
			editor_->updateEditState(pa.getObject());
		}

		// Check if custom erase function processes this case.
		if (eraseFunc_ && eraseFunc_(pa, eraseKey))
		{
			return;
		}

		Key key;
		if (!createKey(pa, key))
		{
			pa.erase(eraseKey);
			return;
		}

		auto classId = getClassIdentifier<ReflectedCollectionEraseCommand>();
		auto args = ManagedObject<ReflectedCollectionEraseCommandParameters>::make_unique();
		(*args)->id_ = key.first;
		(*args)->path_ = key.second;
		(*args)->key_ = eraseKey;
		const auto itr = commands_.emplace(std::pair<Key, CommandInstancePtr>(
			key, commandManager_.queueCommand(classId, ManagedObjectPtr(std::move(args)))));

		commandManager_.waitForInstance(itr->second);
		commands_.erase(itr);
	}

	void customCommand(ExecuteFunc executeFunc, UndoFunc undoFunc, const std::string& description)
	{
		auto classId = getClassIdentifier<CustomCommand>();
		auto args = ManagedObject<CustomCommandParameters>::make_unique();
		(*args)->description_ = description;
		(*args)->execute_ = executeFunc;
		(*args)->undo_ = undoFunc;
		const auto itr = commands_.emplace(std::pair<Key, CommandInstancePtr>(
			std::make_pair(RefObjectId::generate(), description), commandManager_.queueCommand(classId, ManagedObjectPtr(std::move(args)))));

		commandManager_.waitForInstance(itr->second);
		commands_.erase(itr);
	}


	virtual void statusChanged(const CommandInstance& commandInstance) const override
	{
		if (!commandInstance.isComplete())
		{
			return;
		}
		if (strcmp(commandInstance.getCommandId(), getClassIdentifier<SetReflectedPropertyCommand>()) != 0)
		{
			return;
		}

		// Unfortunately don't have key for map lookup
		for (auto itr = commands_.cbegin(); itr != commands_.cend(); ++itr)
		{
			if (&commandInstance == itr->second.get())
			{
				commands_.erase(itr);
				break;
			}
		}
	}

private:
	typedef std::pair<RefObjectId, std::string> Key;
	bool createKey(const PropertyAccessor& pa, Key& o_Key)
	{
		const auto obj = pa.getRootObject();
		if (obj == nullptr)
		{
			return false;
		}

		TF_ASSERT(obj.id() != RefObjectId::zero());
        o_Key.first = obj.id();
		o_Key.second = pa.getFullPath();
		return true;
	}

	ICommandManager& commandManager_;

	// commands_ must be mutable to satisfy ICommandEventListener
	// Use a multimap in case multiple commands for the same key get queued
	mutable std::multimap<Key, CommandInstancePtr> commands_;
	IEditor* editor_;
	CustomSetValueFunc setFunc_;
	CustomEraseFunc eraseFunc_;
	CustomInsertFunc insertFunc_;

	friend ReflectionController;
};

ReflectionController::ReflectionController()
{
}

ReflectionController::~ReflectionController()
{
}

void ReflectionController::init(ICommandManager& commandManager)
{
	impl_.reset(new Impl(commandManager));
}

void ReflectionController::fini()
{
	impl_.reset();
}

Variant ReflectionController::getValue(const PropertyAccessor& pa)
{
	TF_ASSERT(impl_ != nullptr);
	return impl_->getValue(pa);
}

void ReflectionController::setValue(const PropertyAccessor& pa, const Variant& data)
{
	TF_ASSERT(impl_ != nullptr);
	impl_->setValue(pa, data);
}

Variant ReflectionController::invoke(const PropertyAccessor& pa, const ReflectedMethodParameters& parameters)
{
	TF_ASSERT(impl_ != nullptr);
	return impl_->invoke(pa, parameters);
}

void ReflectionController::insert(const PropertyAccessor& pa, const Variant& key, const Variant& value)
{
	TF_ASSERT(impl_ != nullptr);
	impl_->insert(pa, key, value);
}

void ReflectionController::erase(const PropertyAccessor& pa, const Variant& key)
{
	TF_ASSERT(impl_ != nullptr);
	impl_->erase(pa, key);
}

void ReflectionController::customCommand(ExecuteFunc executeFunc, UndoFunc undoFunc, const std::string& description)
{
	TF_ASSERT(impl_ != nullptr);
	impl_->customCommand(executeFunc, undoFunc, description);
}

void ReflectionController::setEditor(IEditor* editor)
{
	impl_->editor_ = editor;
}

void ReflectionController::setCustomSetValue(CustomSetValueFunc setFunc)
{
	impl_->setFunc_ = setFunc;
}

void ReflectionController::setCustomErase(CustomEraseFunc eraseFunc)
{
	impl_->eraseFunc_ = eraseFunc;
}

void ReflectionController::setCustomInsert(CustomInsertFunc insertFunc)
{
	impl_->insertFunc_ = insertFunc;
}

} // end namespace wgt
