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
 * FileSystemPlugin
 *
 * A plugin used to enumerate and manipulate the underlying file system
 */
class FileSystemPlugin
	: public PluginMain
{
public:
	FileSystemPlugin( IComponentContext & contextManager )
	{

	}

	bool PostLoad( IComponentContext & contextManager ) override
	{
		types_.push_back( contextManager.registerInterface( &fileSystem_, false ) );
		return true;
	}

	void Initialise(IComponentContext & contextManager) override
	{
	}

	bool Finalise( IComponentContext & contextManager ) override
	{
		return true;
	}

	void Unload( IComponentContext & contextManager ) override
	{
		for (auto type: types_)
		{
			contextManager.deregisterInterface( type );
		}
	}

private:
	FileSystem fileSystem_;
	std::vector<IInterface*> types_;
};

PLG_CALLBACK_FUNC( FileSystemPlugin )
} // end namespace wgt
