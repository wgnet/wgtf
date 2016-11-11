#include "history_object.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/generic/generic_object.hpp"
#include "core_data_model/variant_list.hpp"
#include "core_data_model/i_value_change_notifier.hpp"
#include "core_data_model/i_item_role.hpp"
#include "core_logging/logging.hpp"
#include "display_object.hpp"

#include <cassert>
#include <future>
namespace wgt
{
struct CurrentIndexNotifier : public IValueChangeNotifier
{
	CurrentIndexNotifier(ICommandManager& commandManager) : commandManager_(commandManager)
	{
	}

	~CurrentIndexNotifier()
	{
		disconnect();
	}

	void connect()
	{
		using namespace std::placeholders;
		callbacks_ += commandManager_.signalPreCommandIndexChanged.connect(
		std::bind(&CurrentIndexNotifier::onPreCommandIndexChanged, this, _1));
		callbacks_ += commandManager_.signalPostCommandIndexChanged.connect(
		std::bind(&CurrentIndexNotifier::onPostCommandIndexChanged, this, _1));
	}

	void disconnect()
	{
		callbacks_.clear();
	}

	void onPreCommandIndexChanged(int index)
	{
		signalPreDataChanged();
	}

	void onPostCommandIndexChanged(int index)
	{
		signalPostDataChanged();
	}

	Variant variantValue() const override
	{
		return commandManager_.commandIndex();
	}

	bool variantValue(const Variant& data) override
	{
		int value;

		if (!data.tryCast<int>(value))
		{
			return false;
		}

		if (value != commandManager_.commandIndex())
		{
			commandManager_.moveCommandIndex(value);
		}

		return true;
	}

	ICommandManager& commandManager_;
	ConnectionHolder callbacks_;
};

//==============================================================================
HistoryObject::HistoryObject() : commandSystem_(nullptr), clearButtonVisible(true), makeMacroButtonVisible(true)
{
}

//==============================================================================
void HistoryObject::init(ICommandManager& commandSystem, IDefinitionManager& defManager)
{
	commandSystem_ = &commandSystem;
	defManager_ = &defManager;

	currentIndexNotifier_.reset(new CurrentIndexNotifier(*commandSystem_));
	historyItems_.setSource(commandSystem_->getHistory());

	bindCommandHistoryCallbacks();
}

//==============================================================================
void HistoryObject::bindCommandHistoryCallbacks()
{
	using namespace std::placeholders;
	historyCallbacks_ += commandSystem_->signalHistoryPostReset.connect(
		std::bind(&HistoryObject::onCommandHistoryPostReset, this, _1));

	CurrentIndexNotifier* cin =
		static_cast<CurrentIndexNotifier*>(currentIndexNotifier_.get());

	cin->connect();
}

//==============================================================================
void HistoryObject::unbindCommandHistoryCallbacks()
{
	CurrentIndexNotifier* cin = static_cast<CurrentIndexNotifier*>(currentIndexNotifier_.get());
	cin->disconnect();

	historyCallbacks_.clear();
}

//==============================================================================
void HistoryObject::fini()
{
	assert(commandSystem_ != nullptr);
	unbindCommandHistoryCallbacks();
}

//==============================================================================
const AbstractListModel* HistoryObject::getHistory() const
{
	return &historyItems_;
}

//==============================================================================
const IValueChangeNotifier* HistoryObject::currentIndexSource() const
{
	return currentIndexNotifier_.get();
}

//==============================================================================
const ISelectionHandler* HistoryObject::selectionHandlerSource() const
{
	return &selectionHandler_;
}

//==============================================================================
ObjectHandle HistoryObject::createMacro() const
{
	assert(commandSystem_ != nullptr);
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
			assert(false);
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
	assert(commandSystem_ != nullptr);
	clearSelection();
	auto removeAll = [](const CommandInstancePtr& instance) {return true; };
	commandSystem_->removeCommands(removeAll);
}

//==============================================================================
void HistoryObject::onCommandHistoryPostReset(const Collection& history)
{
	clearSelection();
	historyItems_.setSource(history);
}

void HistoryObject::clearSelection()
{
	selectionHandler_.setSelectedRows(std::vector<int>());
	selectionHandler_.setSelectedItems(std::vector<IItem*>());
}
} // end namespace wgt
