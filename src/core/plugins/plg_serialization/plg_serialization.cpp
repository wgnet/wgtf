#include "core_generic_plugin/generic_plugin.hpp"
#include "core_serialization/serializer/serialization_manager.hpp"


namespace wgt
{
//==============================================================================
class SerializationSystemHolder
{
public:
	//==========================================================================
	SerializationSystemHolder()
		: serializationManager_( new SerializationManager )
	{
	}


	//==========================================================================
	~SerializationSystemHolder()
	{
		serializationManager_.reset();
	}

	SerializationManager * getSerializationManager()
	{
		return serializationManager_.get();
	}

private:
	std::unique_ptr< SerializationManager > serializationManager_;
};

//==============================================================================
class SerializationPlugin
	: public PluginMain
{
private:
	std::unique_ptr< SerializationSystemHolder >	serializationSystemHolder_;
	std::vector< IInterface * > types_;

public:
	//==========================================================================
	SerializationPlugin( IComponentContext & contextManager )
		: serializationSystemHolder_( new SerializationSystemHolder )
	{ 
		types_.push_back(
			contextManager.registerInterface( serializationSystemHolder_->getSerializationManager(), false ) );
	}

	//==========================================================================
	void Unload( IComponentContext & contextManager ) override
	{
		for( auto type : types_ )
		{
			contextManager.deregisterInterface( type );
		}
	}
};

PLG_CALLBACK_FUNC( SerializationPlugin )
} // end namespace wgt
