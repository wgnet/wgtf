#include "reflection_controller.hpp"
#include "core_command_system/i_command_manager.hpp"
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

#include <map>
#include "core_reflection/reflected_method.hpp"

namespace wgt
{
class ReflectionController::Impl
	: public ICommandEventListener
{
public:
	Impl( ICommandManager & commandManager )
		: commandManager_( commandManager )
	{
		commandManager_.registerCommandStatusListener( this );
	}

	~Impl()
	{
		commandManager_.deregisterCommandStatusListener( this );
	}

	Variant getValue( const PropertyAccessor & pa )
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
		if (!createKey( pa, key ))
		{
			return pa.getValue();
		}

		// TODO: assert access is only on the main thread
		auto range = commands_.equal_range( key );
		for (auto it = range.first; it != range.second; ++it)
		{
			auto instance = it->second;
			commandManager_.waitForInstance(instance);
		}
		commands_.erase( range.first, range.second );

		return pa.getValue();
	}

	void setValue( const PropertyAccessor & pa, const Variant & data )
	{
		Key key;
		if (!createKey( pa, key ))
		{
			pa.setValue( data );
			return;
		}

		// Access is only on the main thread
		assert( std::this_thread::get_id() == commandManager_.ownerThreadId() );

		const auto commandId = getClassIdentifier< SetReflectedPropertyCommand >();

		const auto commandArgs = pa.getDefinitionManager()->create< ReflectedPropertyCommandArgument >();
		commandArgs->setContextId( key.first );
		commandArgs->setPath( key.second.c_str() );
		commandArgs->setValue( data );

		auto command = commandManager_.queueCommand( commandId, commandArgs );

		// Queuing may cause it to execute straight away
		// Based on the thread affinity of SetReflectedPropertyCommand
		if (!command->isComplete())
		{
			commands_.emplace( std::pair< Key, CommandInstancePtr >( key, command ) );
		}
	}

	Variant invoke( const PropertyAccessor & pa, const ReflectedMethodParameters & parameters )
	{
		if (findFirstMetaData<MetaDirectInvokeObj>(pa, *pa.getDefinitionManager()))
		{
			return pa.invoke(parameters);
		}

		Key key;
		if (!createKey( pa, key ))
		{
			return pa.invoke( parameters );
		}

		std::unique_ptr<ReflectedMethodCommandParameters> commandParameters( new ReflectedMethodCommandParameters() );
		commandParameters->setId( key.first );
		commandParameters->setPath( key.second.c_str() );
		commandParameters->setParameters( parameters );

		const auto itr = commands_.emplace( std::pair< Key, CommandInstancePtr >( key, commandManager_.queueCommand(
			getClassIdentifier<InvokeReflectedMethodCommand>(), ObjectHandle( std::move( commandParameters ),
			pa.getDefinitionManager()->getDefinition<ReflectedMethodCommandParameters>() ) ) ) );

		commandManager_.waitForInstance( itr->second );
		ObjectHandle returnValueObject = itr->second.get()->getReturnValue();
		commands_.erase( itr );
		Variant* returnValuePointer = returnValueObject.getBase<Variant>();
		assert( returnValuePointer != nullptr );
		return *returnValuePointer;
	}

	void insert( const PropertyAccessor & pa, const Variant & insertKey, const Variant & value )
	{
		Key key;
		if (!createKey( pa, key ))
		{
			pa.insert( insertKey, value );
			return;
		}

		std::unique_ptr<ReflectedCollectionInsertCommandParameters> commandParameters( new ReflectedCollectionInsertCommandParameters() );
		commandParameters->id_ = key.first;
		commandParameters->path_ = key.second;
		commandParameters->key_ = insertKey;
		commandParameters->value_ = value;

		const auto itr = commands_.emplace( std::pair< Key, CommandInstancePtr >( key, commandManager_.queueCommand(
			getClassIdentifier<ReflectedCollectionInsertCommand>(), ObjectHandle( std::move( commandParameters ) ) ) ) );

		commandManager_.waitForInstance( itr->second );
		commands_.erase( itr );
	}

	void erase( const PropertyAccessor & pa, const Variant & eraseKey )
	{
		Key key;
		if (!createKey( pa, key ))
		{
			pa.erase( eraseKey );
			return;
		}

		std::unique_ptr<ReflectedCollectionEraseCommandParameters> commandParameters( new ReflectedCollectionEraseCommandParameters() );
		commandParameters->id_ = key.first;
		commandParameters->path_ = key.second;
		commandParameters->key_ = eraseKey;

		const auto itr = commands_.emplace( std::pair< Key, CommandInstancePtr >( key, commandManager_.queueCommand(
			getClassIdentifier<ReflectedCollectionEraseCommand>(), ObjectHandle( std::move( commandParameters ) ) ) ) );

		commandManager_.waitForInstance( itr->second );
		commands_.erase( itr );
	}

	virtual void statusChanged(
		const CommandInstance & commandInstance ) const override
	{
		if (!commandInstance.isComplete())
		{
			return;
		}
		if (strcmp( commandInstance.getCommandId(),
			getClassIdentifier< SetReflectedPropertyCommand >() ) != 0)
		{
			return;
		}

		// Unfortunately don't have key for map lookup
		for (auto itr = commands_.cbegin(); itr != commands_.cend(); ++itr)
		{
			if (&commandInstance == itr->second.get())
			{
				commands_.erase( itr );
				break;
			}
		}
	}

private:
	typedef std::pair< RefObjectId, std::string > Key;
	bool createKey( const PropertyAccessor & pa, Key & o_Key )
	{
		const auto & obj = pa.getRootObject();
		if (obj == nullptr)
		{
			return false;
		}

		if (!obj.getId( o_Key.first ))
		{
			auto om = pa.getDefinitionManager()->getObjectManager();
			assert( !om->getObject( obj.data() ).isValid() );
			if (!om->getUnmanagedObjectId( obj.data(), o_Key.first ))
			{
				o_Key.first = om->registerUnmanagedObject( obj );
			}
		}

		o_Key.second = pa.getFullPath();
		return true;
	}

	ICommandManager & commandManager_;

	// commands_ must be mutable to satisfy ICommandEventListener
	// Use a multimap in case multiple commands for the same key get queued
	mutable std::multimap< Key, CommandInstancePtr > commands_;
};

ReflectionController::ReflectionController()
{

}

ReflectionController::~ReflectionController()
{

}

void ReflectionController::init( ICommandManager & commandManager )
{
	impl_.reset( new Impl( commandManager ) );
}

void ReflectionController::fini()
{
	impl_.reset();
}

Variant ReflectionController::getValue( const PropertyAccessor & pa )
{
	assert( impl_ != nullptr );
	return impl_->getValue( pa );
}

void ReflectionController::setValue( const PropertyAccessor & pa, const Variant & data )
{
	assert( impl_ != nullptr );
	impl_->setValue( pa, data );
}

Variant ReflectionController::invoke( const PropertyAccessor & pa, const ReflectedMethodParameters & parameters )
{
	assert( impl_ != nullptr );
	return impl_->invoke( pa, parameters );
}

void ReflectionController::insert( const PropertyAccessor & pa, const Variant & key, const Variant & value )
{
	assert( impl_ != nullptr );
	impl_->insert( pa, key, value );
}

void ReflectionController::erase( const PropertyAccessor & pa, const Variant & key )
{
	assert( impl_ != nullptr );
	impl_->erase( pa, key );
}
} // end namespace wgt
