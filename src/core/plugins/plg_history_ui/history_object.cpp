#include "history_object.hpp"

#include "core_common/assert.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/property_accessor.hpp"
#include "core_reflection/generic/generic_object.hpp"
#include "core_data_model/i_value_change_notifier.hpp"
#include "core_data_model/i_item_role.hpp"
#include "core_logging/logging.hpp"
#include "display_object.hpp"

#include <cassert>
#include <future>
namespace wgt
{
HistoryObject::HistoryObject() : commandSystem_(nullptr), clearButtonVisible(true), makeMacroButtonVisible(true)
{
}

//==============================================================================
void HistoryObject::init(ICommandManager& commandSystem, IDefinitionManager& defManager)
{
	commandSystem_ = &commandSystem;
	defManager_ = &defManager;

    TF_ASSERT(handle() != nullptr);

	historyModel_.setSource(commandSystem_->getHistory());
	bindCommandHistoryCallbacks();
}

//==============================================================================
void HistoryObject::bindCommandHistoryCallbacks()
{
	using namespace std::placeholders;
	historyCallbacks_ += commandSystem_->signalHistoryPostReset.connect(
		std::bind(&HistoryObject::onCommandHistoryPostReset, this, _1));
	historyCallbacks_ += commandSystem_->signalPostCommandIndexChanged.connect(
	std::bind(&HistoryObject::onPostCommandIndexChanged, this, _1));
}

//==============================================================================
void HistoryObject::onPostCommandIndexChanged(int index)
{
	commandIndex_ = index;
	Variant indexVariant = commandIndex_;

    auto definition = defManager_->getDefinition<HistoryObject>();
    definition->bindProperty("commandIndex", handle()).setValue(indexVariant);
}

//==============================================================================
void HistoryObject::unbindCommandHistoryCallbacks()
{
	historyCallbacks_.clear();
}

//==============================================================================
void HistoryObject::fini()
{
	TF_ASSERT(commandSystem_ != nullptr);
	unbindCommandHistoryCallbacks();
}

//==============================================================================
const AbstractListModel* HistoryObject::getHistory() const
{
	return &historyModel_;
}

//==============================================================================
const ISelectionHandler* HistoryObject::selectionHandlerSource() const
{
	return &selectionHandler_;
}

//==============================================================================
ObjectHandle HistoryObject::createMacro() const
{
	TF_ASSERT(commandSystem_ != nullptr);
	const Collection& history = commandSystem_->getHistory();
	int size = static_cast<int>(history.size());
	std::vector<Variant> commandList;
	const auto& selectionSet = selectionHandler_.getSelectedRows();

	if (selectionSet.empty())
	{
		return nullptr;
	}

	for (auto index : selectionSet)
	{
		if (index >= size)
		{
			TF_ASSERT(false);
			return nullptr;
		}
		commandList.push_back(history[index]);
	}

	commandSystem_->createMacro(Collection(commandList));
	return nullptr;
}

void HistoryObject::setClearButtonVisible(const bool& isVisible)
{
	clearButtonVisible = isVisible;
}

bool HistoryObject::isClearButtonVisible() const
{
	return clearButtonVisible;
}

void HistoryObject::setMakeMacroButtonVisible(const bool& isVisible)
{
	makeMacroButtonVisible = isVisible;
}

bool HistoryObject::isMakeMacroButtonVisible() const
{
	return makeMacroButtonVisible;
}

ObjectHandle HistoryObject::extractItemDetail(const Variant& item)
{
	CommandInstancePtr commandInstance;

	if (!item.tryCast(commandInstance))
	{
		return ObjectHandle();
	}

	return commandInstance->getCommandDescription();
}

void HistoryObject::clearHistory()
{
	TF_ASSERT(commandSystem_ != nullptr);
	clearSelection();
	auto removeAll = [](const CommandInstancePtr& instance) { return true; };
	commandSystem_->removeCommands(removeAll);
}

//==============================================================================
void HistoryObject::setCommandIndex(const int newIndex)
{
	commandIndex_ = newIndex;
	if (commandIndex_ != commandSystem_->commandIndex())
	{
		commandSystem_->moveCommandIndex(commandIndex_);
	}
}

//==============================================================================
void HistoryObject::onCommandHistoryPostReset(const Collection& history)
{
	clearSelection();
	historyModel_.setSource(history);
}

void HistoryObject::clearSelection()
{
	selectionHandler_.setSelectedRows(std::vector<int>());
	selectionHandler_.setSelectedItems(std::vector<IItem*>());
}
} // end namespace wgt
