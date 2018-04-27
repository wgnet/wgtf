#include "reflected_dialog_model.hpp"

#include "core_common/assert.hpp"
#include "core_data_model/reflection_proto/property_tree_model.hpp"
#include "core_reflection/ref_object_id.hpp"
#include "core_reflection/generic/generic_object.hpp"
#include "core_reflection/metadata/meta_utilities.hpp"
#include "core_reflection/metadata/meta_impl.hpp"
#include "core_reflection/base_property_with_metadata.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/utilities/reflection_utilities.hpp"

namespace wgt
{
ReflectedDialogModel::ReflectedDialogModel()
{
}

ReflectedDialogModel::~ReflectedDialogModel()
{
}

void ReflectedDialogModel::initialise(ManagedObjectPtr dialogData, ObjectHandle originalData, const char* resource, const char* title)
{
	if(resource != nullptr)
	{
		setURL(resource);
	}

	if (title != nullptr)
	{
		setTitle(title);
	}

	originalData_ = originalData;
	TF_ASSERT(originalData_.isValid());

	if(!modifyDataDirectly())
	{
		TF_ASSERT(dialogData);
		dialogDataObj_ = std::move(dialogData);
		dialogData_ = dialogDataObj_->getHandle();
		TF_ASSERT(dialogData_.isValid());

		auto definitionManager = get<IDefinitionManager>();
		TF_ASSERT(definitionManager);
		ReflectionUtilities::copyProperties(*definitionManager, originalData_, dialogData_, false);
	}

	AbstractTreeModel::DataCallback onDataChanged = [this](const AbstractTreeModel::ItemIndex&, int, ItemRole::Id,
	                                                       const Variant&) {
		if (!dataEdited_)
		{
			dataEdited_ = true;
			dataEditSignal_(Variant(dataEdited_));
		}
	};

	setModel(std::make_shared<proto::PropertyTreeModel>(modifyDataDirectly() ? originalData_ : dialogData_));
	model_->connectPostItemDataChanged(onDataChanged);

	dataEdited_ = false;
	dataEditSignal_(Variant(dataEdited_));
}

void ReflectedDialogModel::setModel(std::shared_ptr<AbstractTreeModel> model)
{
	model_ = model;
}

void ReflectedDialogModel::onClose(IDialog::Result result)
{
	setResult(result);

	if (result == CANCEL)
	{
		auto commandManager = get<ICommandManager>();
		TF_ASSERT(commandManager);
		commandManager->moveCommandIndex(commandIndex_);
		commandIndex_ = -1;
	}
	else if (result == SAVE && !modifyDataDirectly())
	{
		auto definitionManager = get<IDefinitionManager>();
		TF_ASSERT(definitionManager);
		ReflectionUtilities::copyProperties(*definitionManager, dialogData_, originalData_, true);
	}
}

const AbstractTreeModel* ReflectedDialogModel::getModel() const
{
	return model_.get();
}

void ReflectedDialogModel::onShow()
{
	auto commandManager = get<ICommandManager>();
	TF_ASSERT(commandManager);
	commandIndex_ = commandManager->commandIndex();
}

bool ReflectedDialogModel::dataEdited() const
{
	return dataEdited_;
}

bool ReflectedDialogModel::modifyDataDirectly() const
{
	return true;
}

void ReflectedDialogModel::getDataEditSignal(Signal<void(Variant&)>** result) const
{
	*result = const_cast<Signal<void(Variant&)>*>(&dataEditSignal_);
}

} // end namespace wgt
