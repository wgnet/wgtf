#include "core_generic_plugin/generic_plugin.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_generic_plugin/interfaces/i_plugin_context_manager.hpp"
#include "qt_copy_paste_manager.hpp"
#include "qt_framework_adapter.hpp"
#include "qt_application_adapter.hpp"
#include "core_variant/variant.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_qt_common/shared_controls.hpp"
#include "core_qt_common/qt_new_handler.hpp"
#include "core_qt_common/i_qt_framework.hpp"
#include "private/ui_view_creator.hpp"

#include <vector>
#include <QApplication>

namespace wgt
{
class MayaAdapterPlugin
	: public PluginMain
{
public:
	MayaAdapterPlugin( IComponentContext & contextManager )
	{
		contextManager.registerInterface(new UIViewCreator(contextManager));
	}

	bool PostLoad( IComponentContext & contextManager ) override
	{
		qtCopyPasteManager_ = new QtCopyPasteManager();
		types_.push_back(
			contextManager.registerInterface(qtCopyPasteManager_));

		IPluginContextManager* pPluginContextManager = contextManager.queryInterface<IPluginContextManager>();

		if (pPluginContextManager && pPluginContextManager->getExecutablePath())
			QCoreApplication::addLibraryPath(pPluginContextManager->getExecutablePath());
		
		qtApplication_ = new QtApplicationAdapter();
		qtFramework_ = new QtFrameworkAdapter(contextManager);

		types_.push_back(
			contextManager.registerInterface( qtApplication_ ) );
		types_.push_back(
			contextManager.registerInterface( qtFramework_ ) );
		return true;
	}

	void Initialise( IComponentContext & contextManager ) override
	{
		Variant::setMetaTypeManager(contextManager.queryInterface< IMetaTypeManager >());

		auto definitionManager = contextManager.queryInterface<IDefinitionManager>();
		auto commandsystem = contextManager.queryInterface<ICommandManager>();
		qtCopyPasteManager_->init(definitionManager, commandsystem);

		qtFramework_->initialise( contextManager );

		SharedControls::init();

		qtApplication_->initialise( qtFramework_ );
	}

	bool Finalise( IComponentContext & contextManager ) override
	{
		qtCopyPasteManager_->fini();
		qtApplication_->finalise();
		qtFramework_->finalise();
		return true;
	}

	void Unload( IComponentContext & contextManager ) override
	{
		for ( auto type: types_ )
		{
			contextManager.deregisterInterface( type );
		}

		qtFramework_ = nullptr;
		qtApplication_ = nullptr;
		qtCopyPasteManager_ = nullptr;
	}

private:
	QtFramework * qtFramework_;
    QtApplication * qtApplication_;
	QtCopyPasteManager * qtCopyPasteManager_;
	std::vector< IInterface * > types_;
};

PLG_CALLBACK_FUNC( MayaAdapterPlugin )
} // end namespace wgt
