#include "ui_test_panel_context.hpp"
#include "core_data_model/i_tree_model.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"

#include "metadata/ui_test_panel_context.mpp"


namespace wgt
{
namespace
{

void callMethod( Variant & object,
	IDefinitionManager & definitionManager,
	const char * name )
{
	ObjectHandle handle = object.cast< ObjectHandle >();
	if (!handle.isValid())
	{
		NGT_ERROR_MSG( "Failed to call method\n" );
		return;
	}

	auto definition = handle.getDefinition( definitionManager );
	auto property = definition->findProperty( name );
	if (property == nullptr)
	{
		NGT_ERROR_MSG( "Failed to call method\n" );
		return;
	}

	ReflectedMethodParameters parameters;
	property->invoke( handle, definitionManager, parameters );
}

} // namespace


PanelContext::PanelContext()
{
}


PanelContext::~PanelContext()
{
}


const std::string & PanelContext::panelName() const
{
	return panelName_;
}


ITreeModel * PanelContext::treeModel() const
{
	return treeModel_.get();
}


void PanelContext::updateValues()
{
	assert( pContext_ != nullptr );
	auto pDefinitionManager = pContext_->queryInterface< IDefinitionManager >();
	if (pDefinitionManager == nullptr)
	{
		NGT_ERROR_MSG( "Failed to find IDefinitionManager\n" );
		return;
	}
	auto & definitionManager = (*pDefinitionManager);

	const char* methodName = "updateValues";

	auto moduleDefinition = pythonObject_.getDefinition( definitionManager );

	auto property = moduleDefinition->findProperty( "oldStyleObject" );
	auto oldStylePythonObject_ = property->get( pythonObject_, definitionManager );

	property = moduleDefinition->findProperty( "newStyleObject" );
	auto newStylePythonObject_ = property->get( pythonObject_, definitionManager );
	
	callMethod( oldStylePythonObject_, definitionManager, methodName );
	callMethod( newStylePythonObject_, definitionManager, methodName );
}
} // end namespace wgt
