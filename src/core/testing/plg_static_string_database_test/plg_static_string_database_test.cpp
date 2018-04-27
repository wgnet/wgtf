#include "core_generic_plugin/generic_plugin.hpp"

#include "static_string_database.hpp"

namespace wgt
{

/**
* A plugin which tests StaticStringDatabase iterface
*
* @ingroup plugins
* @note Requires Plugins:
*       - FileSystemPlugin
*       - ReflectionPlugin
*       - CommandSystemPlugin
*       - MainTestPlugin TODO: check what test is capable of, implement some tests.
*/
class StaticStringDatabaseTestPlugin : public PluginMain
{
public:
	//==========================================================================
	StaticStringDatabaseTestPlugin(IComponentContext& contextManager)
	{
	}

	//==========================================================================
	bool PostLoad(IComponentContext& contextManager)
	{
		contextManager.registerInterface(static_cast<IStaticStringDatabase*>(new StaticStringDatabase()));
		return true;
	}

	//==========================================================================
	void Initialise(IComponentContext& contextManager)
	{
		auto staticStringDatabase = contextManager.queryInterface<IStaticStringDatabase>();
		if (staticStringDatabase != nullptr)
		{
			staticStringDatabase->insert("Alice");
			staticStringDatabase->insert("Bob");
			staticStringDatabase->insert("Eve");
		}
	}
};

PLG_CALLBACK_FUNC(StaticStringDatabaseTestPlugin)
} // end namespace wgt
