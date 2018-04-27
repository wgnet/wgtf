#include "core_generic_plugin/generic_plugin.hpp"
#include "core_serialization/serializer/serialization_manager.hpp"

namespace wgt
{
//==============================================================================
class SerializationSystemHolder
{
public:
	//==========================================================================
	SerializationSystemHolder() : serializationManager_(new SerializationManager)
	{
	}

	//==========================================================================
	~SerializationSystemHolder()
	{
		serializationManager_.reset();
	}

	SerializationManager* getSerializationManager()
	{
		return serializationManager_.get();
	}

private:
	std::unique_ptr<SerializationManager> serializationManager_;
};

/**
* A plugin which registers an ISerializationManager interface which allows reading and writing Variant objects to an
IDataStream
*
* @ingroup plugins
* @note Requires Plugins:
*       - @ref coreplugins

*/
class SerializationPlugin : public PluginMain
{
private:
	std::unique_ptr<SerializationSystemHolder> serializationSystemHolder_;
	InterfacePtrs types_;

public:
	//==========================================================================
	SerializationPlugin(IComponentContext& contextManager) : serializationSystemHolder_(new SerializationSystemHolder)
	{
		types_.push_back(
		contextManager.registerInterface(serializationSystemHolder_->getSerializationManager(), false));
	}

	//==========================================================================
	void Unload(IComponentContext& contextManager) override
	{
		for (auto type : types_)
		{
			contextManager.deregisterInterface(type.get());
		}
	}
};

PLG_CALLBACK_FUNC(SerializationPlugin)
} // end namespace wgt
