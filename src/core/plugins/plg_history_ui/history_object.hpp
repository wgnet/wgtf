#ifndef HISTORY_OBJECT_HPP
#define HISTORY_OBJECT_HPP

#include "core_reflection/object_handle.hpp"
#include "core_data_model/selection_handler.hpp"
#include "core_command_system/i_command_manager.hpp"
#include "history_model.hpp"
#include "core_object/object_handle_provider.hpp"

#include <memory>

namespace wgt
{
class IDefinitionManager;

/**
 *	Wrapper for accessing the current position in the undo/redo list from QML.
 */
class HistoryObject : public ObjectHandleProvider<HistoryObject>
{
public:
	// ClassDefinition::create() requires a default constructor
	HistoryObject();
	void init(ICommandManager& commandSystem, IDefinitionManager& defManager);
	void fini();

	const AbstractListModel* getHistory() const;
	const ISelectionHandler* selectionHandlerSource() const;
	ObjectHandle createMacro() const;

	void setClearButtonVisible(const bool& isVisible);
	bool isClearButtonVisible() const;

	void setMakeMacroButtonVisible(const bool& isVisible);
	bool isMakeMacroButtonVisible() const;

	ObjectHandle extractItemDetail(const Variant& item);
	void clearHistory();

	int commandIndex_;
	void setCommandIndex(const int newIndex);
	void provideSignalFunc(Signal<void(Variant&)>** signal) const
	{
		auto signalPtr = const_cast<Signal<void(Variant&)>*>(&commandIndexChanged_);
		*signal = signalPtr;
	}

private:
	void bindCommandHistoryCallbacks();
	void unbindCommandHistoryCallbacks();
	void onCommandHistoryPostReset(const Collection& history);
	void clearSelection();
	Signal<void(Variant&)> commandIndexChanged_;

	void onPostCommandIndexChanged(int index);

	ICommandManager* commandSystem_;
	IDefinitionManager* defManager_;
	// TODO: NGT-849
	// Eventually, we need to remove this
	SelectionHandler selectionHandler_;
	HistoryModel historyModel_;
	Connection postHistoryItemsRemoved_;
	ConnectionHolder historyCallbacks_;

	bool clearButtonVisible;
	bool makeMacroButtonVisible;
};
} // end namespace wgt
#endif // HISTORY_OBJECT_HPP
