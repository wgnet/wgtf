#include "dialog_test_model.hpp"
#include "metadata/dialog_reflected_data.mpp"
#include "metadata/dialog_reflected_tree_model.mpp"
#include "core_reflection/reflection_macros.hpp"
#include "core_reflection/function_property.hpp"
#include "core_reflection/metadata/meta_types.hpp"
#include "core_reflection/utilities/reflection_function_utilities.hpp"
#include "core_reflection/property_accessor.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_ui_framework/i_dialog.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_data_model/reflection_proto/reflected_tree_model.hpp"

namespace wgt
{
BEGIN_EXPOSE(DialogTestModel, MetaNone())
EXPOSE("basicDialogResult", basicDialogResult_)
EXPOSE("getReflectedModel", getReflectedModel, MetaNoSerialization())
EXPOSE("getSource", getSource, MetaNoSerialization())
EXPOSE_METHOD("callBasicDialog", callBasicDialog, MetaDirectInvoke())
EXPOSE_METHOD("callReflectedDialog", callReflectedDialog, MetaDirectInvoke())
END_EXPOSE()

DialogTestModel::DialogTestModel()
    : context_(nullptr)
    , definition_(nullptr)
{
}

DialogTestModel::~DialogTestModel()
{
}

void DialogTestModel::initialise(IComponentContext& context, const IClassDefinition* definition)
{
	context_ = &context;
	definition_ = definition;

	IDefinitionManager& definitionManager = *context.queryInterface<IDefinitionManager>();
	reflectedData_ = definitionManager.create<DialogReflectedData>();
	reflectedData_->initialise(context);
	reflectedModel_.reset(new proto::ReflectedTreeModel(*context_, reflectedData_));
}

void DialogTestModel::finalise()
{
	reflectedDialog_.reset();
}

void DialogTestModel::callBasicDialog(bool modal)
{
	assert(context_ != nullptr);
	auto uiFramework = context_->queryInterface<IUIFramework>();
	assert(uiFramework != nullptr);

	IDialog::ClosedCallback callback = [this](IDialog& dialog)
	{
		assert(definition_ != nullptr);
		PropertyAccessor accessor = definition_->bindProperty("basicDialogResult", this);
		accessor.setValue(dialog.result());
	};

	const IDialog::Mode mode = modal ? IDialog::Mode::MODAL : IDialog::Mode::MODELESS;
	uiFramework->showDialog("plg_dialog_test/basic_dialog.qml", mode, callback);
}

void DialogTestModel::callReflectedDialog(bool modal, bool modifyDataDirectly)
{
	const auto mode = modal ? IDialog::Mode::MODAL : IDialog::Mode::MODELESS;

	auto uiFramework = context_->queryInterface<IUIFramework>();
	assert(uiFramework != nullptr);

	IDefinitionManager& definitionManager = *context_->queryInterface<IDefinitionManager>();
	auto model = definitionManager.create<DialogReflectedTreeModel>();
	model->initialise(context_, model.getDefinition(definitionManager));
	model->setObject(reflectedData_, modifyDataDirectly);

	reflectedDialog_ = uiFramework->createDialog("plg_dialog_test/reflected_dialog.qml", model);
	reflectedDialog_->show(mode);
}

const DialogTestModel* DialogTestModel::getSource() const
{
	return this;
}

const AbstractTreeModel* DialogTestModel::getReflectedModel() const
{
	return reflectedModel_.get();
}

} // end namespace wgt
