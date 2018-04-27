//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  plg_file_system.cpp
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "core_generic_plugin/generic_plugin.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"

#include "core_serialization/file_system.hpp"

namespace wgt
{
/**
* A plugin used to enumerate and manipulate the underlying file system
*
* @ingroup plugins
* @ingroup coreplugins
*/
class FileSystemPlugin : public PluginMain
{
public:
	bool PostLoad(IComponentContext& contextManager) override
	{
		contextManager.registerInterface(new FileSystem);
		return true;
	}

	void Initialise(IComponentContext& contextManager) override
	{
	}

	bool Finalise(IComponentContext& contextManager) override
	{
		return true;
	}

	void Unload(IComponentContext& contextManager) override
	{
		for (auto type : types_)
		{
			contextManager.deregisterInterface(type.get());
		}
	}

private:
	InterfacePtrs types_;
};

PLG_CALLBACK_FUNC(FileSystemPlugin)
} // end namespace wgt
