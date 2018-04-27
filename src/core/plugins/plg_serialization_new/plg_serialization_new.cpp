#include "../../lib/core_generic_plugin/generic_plugin.hpp"
#include "../../lib/core_serialization_new/serializer_new.hpp"
#include "../../lib/core_serialization_new/serializationhandlermanager.hpp"
#include <memory>

#include "../../lib/core_serialization/resizing_memory_stream.hpp"

namespace wgt
{
/**
* A plugin that creates and registers an ISerializer interface. Allows de/serialization of primitive, reflected,
and custom types.
* @ingroup plugins
* @note Requires Plugins:
*	-ref coreplugins
*/
class SerializationNewPlugin : public PluginMain
{
public:
	SerializationNewPlugin(IComponentContext& componentContext)
	{
	}

	bool PostLoad(IComponentContext& componentContext) override
	{
		IDefinitionManager* definitionManager = componentContext.queryInterface<IDefinitionManager>();
		if (definitionManager == nullptr)
		{
			return false;
		}
		handlerManager_ = std::make_unique<SerializationHandlerManager>(*definitionManager);
		interfaces_.push_back(componentContext.registerInterface(handlerManager_.get(), false));
		serializer_ = std::make_unique<SerializerNew>(handlerManager_.get());
		interfaces_.push_back(componentContext.registerInterface(serializer_.get(), false));

		return true;
	}

	void Initialise(IComponentContext& componentContext) override
	{
	}

	bool Finalise(IComponentContext& componentContext) override
	{
		return true;
	}

	void Unload(IComponentContext& componentContext) override
	{
		for (auto inter : interfaces_)
		{
			componentContext.deregisterInterface(inter);
		}
		serializer_.release();
		handlerManager_.release();
	}

private:
	std::unique_ptr<SerializationHandlerManager> handlerManager_;
	std::unique_ptr<SerializerNew> serializer_;
	std::vector<IInterface*> interfaces_;
};

PLG_CALLBACK_FUNC(SerializationNewPlugin)
} // end namespace wgt