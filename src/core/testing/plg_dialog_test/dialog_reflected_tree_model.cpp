#include "dialog_reflected_tree_model.hpp"
#include "core_data_model/reflection_proto/reflected_tree_model.hpp"
#include "core_reflection/ref_object_id.hpp"
#include "core_reflection/generic/generic_object.hpp"
#include "core_reflection/interfaces/i_reflection_controller.hpp"
#include "core_reflection/metadata/meta_utilities.hpp"
#include "core_reflection/metadata/meta_impl.hpp"
#include "core_reflection/base_property_with_metadata.hpp"

namespace wgt
{
DialogReflectedTreeModel::DialogReflectedTreeModel()
    : model_(nullptr), dataEdited_(false), environmentID_(IEnvManager::INVALID_ID), commandIndex_(-1),
      modifyDataDirectly_(true)
{
}

DialogReflectedTreeModel::~DialogReflectedTreeModel()
{
	if (context_ && environmentID_ != IEnvManager::INVALID_ID)
	{
		auto environmentManager = context_->queryInterface<IEnvManager>();
		if (environmentManager)
		{
			environmentManager->removeEnv(environmentID_);
		}
	}
}

void DialogReflectedTreeModel::initialise(IComponentContext* context, const IClassDefinition* definition)
{
	context_ = context;
	assert(context_);

	definition_ = definition;
	assert(definition_);
}

void DialogReflectedTreeModel::setObject(ObjectHandle data, ObjectHandle copy, bool modifyDirectly)
{
	modifyDataDirectly_ = modifyDirectly;
	dataEdited_ = false;
	dialogData_ = copy;
	originalData_ = data;

	auto definitionManager = context_->queryInterface<IDefinitionManager>();
	assert(definitionManager);
	auto reflectionController = context_->queryInterface<IReflectionController>();
	assert(reflectionController);
	copyReflectedData(*definitionManager, *reflectionController, originalData_, dialogData_, false);

	AbstractTreeModel::DataCallback onDataChanged = [this](const AbstractTreeModel::ItemIndex&, int, ItemRole::Id,
	                                                       const Variant&) {
		if (!dataEdited_)
		{
			dataEdited_ = true;
			definition_->bindProperty("dataEdited", this).setValue(true);
		}
	};

	model_.reset(new proto::ReflectedTreeModel(*context_, modifyDirectly ? originalData_ : dialogData_));
	model_->connectPostItemDataChanged(onDataChanged);
}

void DialogReflectedTreeModel::onClose(IDialog::Result result)
{
	setResult(result);

	if (environmentID_ != IEnvManager::INVALID_ID)
	{
		auto environmentManager = context_->queryInterface<IEnvManager>();
		assert(environmentManager);
		environmentManager->deSelectEnv(environmentID_);
	}

	if (result == CANCEL && modifyDataDirectly_)
	{
		auto commandManager = context_->queryInterface<ICommandManager>();
		assert(commandManager);
		commandManager->moveCommandIndex(commandIndex_);
		commandIndex_ = -1;
	}
	else if (result == SAVE && !modifyDataDirectly_)
	{
		auto definitionManager = context_->queryInterface<IDefinitionManager>();
		assert(definitionManager);
		auto reflectionController = context_->queryInterface<IReflectionController>();
		assert(reflectionController);
		copyReflectedData(*definitionManager, *reflectionController, dialogData_, originalData_, true);
	}

	if (environmentID_ != IEnvManager::INVALID_ID)
	{
		auto environmentManager = context_->queryInterface<IEnvManager>();
		assert(environmentManager);
		environmentManager->removeEnv(environmentID_);
		environmentID_ = IEnvManager::INVALID_ID;
	}
}

const DialogReflectedTreeModel* DialogReflectedTreeModel::getSource() const
{
	return this;
}

const AbstractTreeModel* DialogReflectedTreeModel::getModel() const
{
	return model_.get();
}

void DialogReflectedTreeModel::copyReflectedData(IDefinitionManager& definitionManager,
                                                 IReflectionController& reflectionController, const ObjectHandle& src,
                                                 const ObjectHandle& dst, bool notify)
{
	if (!src.isValid() || !dst.isValid() || src.type() != dst.type())
	{
		NGT_ERROR_MSG("src and dst must be the same type and valid");
		return;
	}

	auto definitionSrc = src.getDefinition(definitionManager);
	auto definitionDst = dst.getDefinition(definitionManager);

	for (auto property : definitionDst->allProperties())
	{
		const char* name = property->getName();

		PropertyAccessor accessorSrc = definitionSrc->bindProperty(name, src);
		PropertyAccessor accessorDst = definitionDst->bindProperty(name, dst);
		if (!accessorSrc.isValid() || !accessorDst.isValid())
		{
			NGT_ERROR_MSG("src and dst must both include property %s", name);
			continue;
		}

		auto valueSrc = accessorSrc.getValue();
		auto valueDst = accessorDst.getValue();
		if (valueSrc.type() != valueDst.type())
		{
			NGT_ERROR_MSG("src and dst property %s must be the same type", name);
			continue;
		}

		if (valueSrc.canCast<ObjectHandle>())
		{
			auto handleSrc = valueSrc.cast<ObjectHandle>();
			auto handleDst = valueDst.cast<ObjectHandle>();
			copyReflectedData(definitionManager, reflectionController, handleSrc, handleDst, notify);
		}
		else if (valueSrc != valueDst)
		{
			if (notify)
			{
				reflectionController.setValue(accessorDst, valueSrc);
			}
			else
			{
				accessorDst.setValueWithoutNotification(valueSrc);
			}
		}
	}
}

void DialogReflectedTreeModel::onShow()
{
	if (modifyDataDirectly_)
	{
		auto commandManager = context_->queryInterface<ICommandManager>();
		assert(commandManager);
		commandIndex_ = commandManager->commandIndex();
	}
	else
	{
		auto environmentManager = context_->queryInterface<IEnvManager>();
		assert(environmentManager);
		environmentID_ = environmentManager->addEnv(RefObjectId::generate().toString().c_str());
		environmentManager->selectEnv(environmentID_);
	}
}

void DialogReflectedTreeModel::onFocusIn()
{
	if (environmentID_ != IEnvManager::INVALID_ID)
	{
		auto environmentManager = context_->queryInterface<IEnvManager>();
		assert(environmentManager);
		environmentManager->selectEnv(environmentID_);
	}
}

void DialogReflectedTreeModel::onFocusOut(bool isChildStealingFocus)
{
	if (environmentID_ != IEnvManager::INVALID_ID && !isChildStealingFocus)
	{
		auto environmentManager = context_->queryInterface<IEnvManager>();
		assert(environmentManager);
		environmentManager->deSelectEnv(environmentID_);
	}
}

} // end namespace wgt
