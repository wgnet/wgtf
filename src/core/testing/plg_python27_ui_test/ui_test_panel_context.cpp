#include "ui_test_panel_context.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_data_model/reflection/reflected_tree_model_new.hpp"
#include "core_reflection/interfaces/i_reflection_controller.hpp"
#include "core_reflection/interfaces/i_class_definition.hpp"
#include "core_reflection/interfaces/i_base_property.hpp"
#include "core_reflection/reflected_method_parameters.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_logging/logging.hpp"

namespace wgt
{
namespace
{
void callMethod(Variant& object, IDefinitionManager& definitionManager, const char* name)
{
	ObjectHandle handle = object.cast<ObjectHandle>();
	if (!handle.isValid())
	{
		NGT_ERROR_MSG("Failed to call method\n");
		return;
	}

	auto definition = definitionManager.getDefinition(handle);
	auto property = definition->findProperty(name);
	if (property == nullptr)
	{
		NGT_ERROR_MSG("Failed to call method\n");
		return;
	}

	ReflectedMethodParameters parameters;
	property->invoke(handle, definitionManager, parameters);
}

} // namespace

PanelContext::PanelContext() : testScriptDescription_("<---- Click Me!")
{
}

bool PanelContext::initialize(const char* panelName, const ObjectHandle& pythonObject)
{
	panelName_ = panelName;
	pythonObject_ = pythonObject;

	treeModel_.reset(new ReflectedTreeModelNew(pythonObject));
	return true;
}

const std::string& PanelContext::panelName() const
{
	return panelName_;
}

AbstractTreeModel* PanelContext::treeModel() const
{
	return treeModel_.get();
}

const PanelContext* PanelContext::getSource() const
{
	return this;
}

void PanelContext::updateValues()
{
	auto pDefinitionManager = get<IDefinitionManager>();
	if (pDefinitionManager == nullptr)
	{
		NGT_ERROR_MSG("Failed to find IDefinitionManager\n");
		return;
	}
	auto& definitionManager = (*pDefinitionManager);

	const char* methodName = "updateValues";

	auto moduleDefinition = definitionManager.getDefinition(pythonObject_);

	auto property = moduleDefinition->findProperty("oldStyleObject");
	auto oldStylePythonObject_ = property->get(pythonObject_, definitionManager);

	property = moduleDefinition->findProperty("newStyleObject");
	auto newStylePythonObject_ = property->get(pythonObject_, definitionManager);

	callMethod(oldStylePythonObject_, definitionManager, methodName);
	callMethod(newStylePythonObject_, definitionManager, methodName);

	testScriptDescription_ = "Update Values Finished";
}

void PanelContext::undoUpdateValues(Variant, Variant)
{
	/*values automatically undone*/
	testScriptDescription_ = "Update Values Undone";
}

void PanelContext::redoUpdateValues(Variant, Variant)
{
	/*values automatically redone*/
	testScriptDescription_ = "Update Values Redone";
}

} // end namespace wgt
