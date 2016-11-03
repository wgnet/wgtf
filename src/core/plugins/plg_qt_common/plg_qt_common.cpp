#include "core_generic_plugin/generic_plugin.hpp"

#include "qt_copy_paste_manager.hpp"
#include "qt_framework.hpp"
#include "core_variant/variant.hpp"
#include "core_qt_common/qt_new_handler.hpp"
#include "core_command_system/i_command_manager.hpp"
#include "core_reflection/i_definition_manager.hpp"

#include <vector>

#include "core_generic_plugin/interfaces/i_component_context_creator.hpp"
#include "core_dependency_system/i_interface.hpp"
#include "private/ui_view_creator.hpp"

#include <QFile>
#include <QTextStream>
#include <QRegExp>
#if defined(USE_Qt5_WEB_ENGINE)
#include <QtWebEngine/QtWebEngine>
#endif

namespace wgt
{
class QtPluginContextCreator
	: public Implements< IComponentContextCreator >
{
public:
	QtPluginContextCreator(QtFramework * qtFramework)
		:qtFramework_( qtFramework )
	{
	}

	IInterface * createContext(const wchar_t * contextId)
	{
		QFile resourcePathFile( getResourcePathFile( contextId ) );
		if (resourcePathFile.open( QFile::ReadOnly | QFile::Text ))
		{
			QTextStream in( &resourcePathFile );
			while (!in.atEnd())
			{
				qtFramework_->addImportPath( in.readLine() );
			}
			resourcePathFile.close();
		}

		return new InterfaceHolder< QtFramework >( qtFramework_, false );
	}

	const char * getType() const
	{
		return typeid(QtFramework).name();
	}

private:
	QString getResourcePathFile( const wchar_t * contextId )
	{
		QString path = QString::fromWCharArray( contextId );
		path.remove( 0, path.lastIndexOf( '/' ) );
		path.prepend(":");
		path.append( "/resource_paths.txt" );
		return path;
	}
	QtFramework * qtFramework_;
};

/**
* A plugin which creates and registers IUIFramework and IViewCreator interfaces to allow creation of UI Components from Qt resources.
* Mutually exclusive with MayaAdapterPlugin.
*
* @ingroup plugins
* @ingroup coreplugins
* @note Requires Plugins:
*       - @ref coreplugins
*/
class QtPluginCommon
	: public PluginMain
{
public:
	QtPluginCommon( IComponentContext & contextManager )
	{
		contextManager.registerInterface(new UIViewCreator(contextManager));
	}

	bool PostLoad( IComponentContext & contextManager ) override
	{
#if defined(USE_Qt5_WEB_ENGINE)
		QtWebEngine::initialize();
#endif
		qtFramework_.reset(new QtFramework(contextManager));
		types_.push_back(
			contextManager.registerInterface(new QtPluginContextCreator(qtFramework_.get())));

		return true;
	}

	void Initialise( IComponentContext & contextManager ) override
	{
		auto definitionManager = contextManager.queryInterface<IDefinitionManager>();
		auto commandsystem = contextManager.queryInterface<ICommandManager>();

		qtCopyPasteManager_ = new QtCopyPasteManager();
		contextManager.registerInterface(qtCopyPasteManager_);
		qtCopyPasteManager_->init( definitionManager, commandsystem );
		qtFramework_->initialise( contextManager );
	}

	bool Finalise( IComponentContext & contextManager ) override
	{
        qtCopyPasteManager_->fini();
		qtCopyPasteManager_ = nullptr;
		qtFramework_->finalise();
		return true;
	}

	void Unload( IComponentContext & contextManager ) override
	{
		for ( auto type: types_ )
		{
			contextManager.deregisterInterface( type );
		}
        qtFramework_.reset();
	}

private:
	std::unique_ptr< QtFramework > qtFramework_;
    QtCopyPasteManager * qtCopyPasteManager_;
	std::vector< IInterface * > types_;
};

PLG_CALLBACK_FUNC( QtPluginCommon )
} // end namespace wgt
