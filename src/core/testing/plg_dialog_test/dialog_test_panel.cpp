#include "dialog_test_panel.hpp"
#include "core_reflection/reflection_macros.hpp"
#include "core_reflection/function_property.hpp"
#include "core_reflection/metadata/meta_types.hpp"
#include "core_reflection/utilities/reflection_function_utilities.hpp"
#include "core_reflection/property_accessor.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_ui_framework/i_dialog.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_data_model/reflection_proto/reflected_tree_model.hpp"
#include "core_data_model/dialog/reflected_dialog_model.hpp"
#include "custom_dialog_model.hpp"

namespace wgt
{
BEGIN_EXPOSE(DialogTestPanel, MetaNone())
EXPOSE("basicDialogResult", basicDialogResult_)
EXPOSE("customDialogResult", customDialogResult_)
EXPOSE("getReflectedModel", getReflectedModel, MetaNoSerialization())
EXPOSE_METHOD("callBasicDialog", callBasicDialog, MetaDirectInvoke())
EXPOSE_METHOD("callCustomDialog", callCustomDialog, MetaDirectInvoke())
EXPOSE_METHOD("callReflectedDialogModifyDirect", callReflectedDialogModifyDirect, MetaDirectInvoke())
EXPOSE_METHOD("callReflectedDialogModifyCopy", callReflectedDialogModifyCopy, MetaDirectInvoke())
END_EXPOSE()

DialogTestPanel::~DialogTestPanel()
{
}

void DialogTestPanel::initialise()
{
    assert(handle() != nullptr);
    definition_ = get<IDefinitionManager>()->getDefinition(handle());
    assert(definition_);

	reflectedData_ = ManagedObject<DialogReflectedData>::make();
	reflectedModel_.reset(new proto::ReflectedTreeModel(reflectedData_.getHandleT()));
}

void DialogTestPanel::callBasicDialog(bool modal)
{
	auto uiFramework = get<IUIFramework>();
	assert(uiFramework != nullptr);

	auto onClose = [this](IDialog& dialog) {
		PropertyAccessor accessor = definition_->bindProperty("basicDialogResult", handle());
		accessor.setValue(dialog.result());
	};

	if (modal)
	{
		auto dialog = uiFramework->showDialog("plg_dialog_test/basic_dialog.qml", IDialog::Mode::MODAL);
		onClose(*dialog);
	}
	else
	{
		uiFramework->showDialog("plg_dialog_test/basic_dialog.qml", IDialog::Mode::MODELESS, onClose);
	}
}

void DialogTestPanel::callCustomDialog(bool modal)
{
	auto uiFramework = get<IUIFramework>();
	assert(uiFramework != nullptr);

	auto onClose = [this](IDialog& dialog) {
		const auto model = static_cast<CustomDialogModel*>(dialog.model().get());
		PropertyAccessor accessor = definition_->bindProperty("customDialogResult", handle());
		accessor.setValue(model->getChosenOption());
	};

	if (modal)
	{
		auto dialog = uiFramework->showDialog<CustomDialogModel>(IDialog::Mode::MODAL);
		onClose(*dialog);
	}
	else
	{
		uiFramework->showDialog<CustomDialogModel>(IDialog::Mode::MODELESS, onClose);
	}
}

void DialogTestPanel::callReflectedDialogModifyDirect(bool modal)
{
	typedef CustomDialogReflectedModel<true> CustomModel;

	auto uiFramework = get<IUIFramework>();
	assert(uiFramework != nullptr);

	auto onClose = [this](IDialog& dialog) {
		const auto model = static_cast<CustomModel*>(dialog.model().get());
		if (model->dataSaved())
		{
			NGT_MSG("Reflected Dialog (direct) data was saved!");
		}
	};

	if (modal)
	{
		auto dialog = uiFramework->showDialog<CustomModel>(reflectedData_.getHandleT(), IDialog::Mode::MODAL);
		onClose(*dialog);
	}
	else
	{
		uiFramework->showDialog<CustomModel>(reflectedData_.getHandleT(), IDialog::Mode::MODELESS, onClose);
	}
}

void DialogTestPanel::callReflectedDialogModifyCopy(bool modal)
{
	typedef CustomDialogReflectedModel<false> CustomModel;

	auto uiFramework = get<IUIFramework>();
	assert(uiFramework != nullptr);

	auto onClose = [this](IDialog& dialog) {
		const auto model = static_cast<CustomModel*>(dialog.model().get());
		if (model->dataSaved())
		{
			NGT_MSG("Reflected Dialog (copy) data was saved!");
		}
	};

	if (modal)
	{
		auto dialog = uiFramework->showDialog<CustomModel>(reflectedData_.getHandleT(), IDialog::Mode::MODAL);
		onClose(*dialog);
	}
	else
	{
		uiFramework->showDialog<CustomModel>(reflectedData_.getHandleT(), IDialog::Mode::MODELESS, onClose);
	}
}

const AbstractTreeModel* DialogTestPanel::getReflectedModel() const
{
	return reflectedModel_.get();
}

} // end namespace wgt
