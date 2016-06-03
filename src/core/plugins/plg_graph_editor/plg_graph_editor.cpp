
#include "src/type_registration.h"
#include "src/graph_editor.h"

#include <core_logging/logging.hpp>

#include <core_generic_plugin/interfaces/i_component_context.hpp>
#include <core_generic_plugin/generic_plugin.hpp>

#include <core_variant/variant.hpp>
#include <core_variant/interfaces/i_meta_type_manager.hpp>

#include <core_reflection/i_definition_manager.hpp>

#include <core_ui_framework/i_ui_framework.hpp>
#include <core_ui_framework/i_ui_application.hpp>
#include <core_ui_framework/i_view.hpp>
#include "core_ui_framework/interfaces/i_view_creator.hpp"

#include "core_dependency_system/depends.hpp"

namespace wgt
{
class GraphEditorPlugin
	: public PluginMain
	, public Depends< IViewCreator >
{
public:
	GraphEditorPlugin(IComponentContext& context)
		: Depends( context )
	{
	}

	bool PostLoad(IComponentContext& context) override
	{
		return true;
	}

	void Initialise(IComponentContext& context) override
	{
		IDefinitionManager* defMng = context.queryInterface<IDefinitionManager>();

		assert(defMng != nullptr);

		Variant::setMetaTypeManager(context.queryInterface<IMetaTypeManager>());

        RegisterGraphEditorTypes(*defMng);
		editor = defMng->create<GraphEditor>(false);

		auto viewCreator = get< IViewCreator >();
		if (viewCreator)
		{
			viewCreator->createView("plg_graph_editor/GraphEditorView.qml", std::move(editor),
				view_);
		}
	}

	bool Finalise(IComponentContext& context) override
	{
		const auto uiApplication = context.queryInterface< IUIApplication >();
		if (uiApplication == nullptr)
		{
			return false;
		}
		if (view_ != nullptr)
		{
			uiApplication->removeView( *view_ );
			view_.reset();
		}
		editor = ObjectHandleT<GraphEditor>();
		return true;
	}

	void Unload(IComponentContext& context) override
	{
	}

private:
	std::unique_ptr<IView> view_;
	ObjectHandleT<GraphEditor> editor;
};

PLG_CALLBACK_FUNC(GraphEditorPlugin)
} // end namespace wgt
