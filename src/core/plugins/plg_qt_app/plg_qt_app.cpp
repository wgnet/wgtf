#include "core_generic_plugin/generic_plugin.hpp"

#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_generic_plugin/interfaces/i_command_line_parser.hpp"
#include "core_generic_plugin/interfaces/i_plugin_context_manager.hpp"

#include "qt_application.hpp"
#include "core_qt_common/shared_controls.hpp"
#include "core_qt_common/i_qt_framework.hpp"
#include "core_variant/variant.hpp"

#include <vector>
#include <QApplication>

namespace wgt
{
class QtPlugin
	: public PluginMain
{
public:
	QtPlugin( IComponentContext & contextManager ){}

	bool PostLoad( IComponentContext & contextManager ) override
	{
		auto clp = contextManager.queryInterface< ICommandLineParser >();
		assert( clp != nullptr );

		qtApplication_ = new QtApplication( clp->argc(), clp->argv() );
		types_.push_back(
			contextManager.registerInterface( qtApplication_ ) );
		return true;
	}

	void Initialise( IComponentContext & contextManager ) override
	{
		Variant::setMetaTypeManager( contextManager.queryInterface< IMetaTypeManager >() );

		IQtFramework* qtFramework =
			contextManager.queryInterface< IQtFramework >();
		assert( qtFramework != nullptr );

		qtApplication_->initialise( qtFramework );
	}

	bool Finalise( IComponentContext & contextManager ) override
	{
		qtApplication_->finalise();

		return true;
	}

	void Unload( IComponentContext & contextManager ) override
	{
		for ( auto type: types_ )
		{
			contextManager.deregisterInterface( type );
		}

		qtApplication_ = nullptr;
	}

private:
	QtApplication * qtApplication_;
	std::vector< IInterface * > types_;
};

PLG_CALLBACK_FUNC( QtPlugin )
} // end namespace wgt
