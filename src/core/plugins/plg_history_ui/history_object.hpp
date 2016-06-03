#ifndef HISTORY_OBJECT_HPP
#define HISTORY_OBJECT_HPP


#include "core_data_model/i_list_model.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_data_model/selection_handler.hpp"
#include "core_data_model/variant_list.hpp"
#include "core_command_system/i_command_manager.hpp"

#include <memory>

namespace wgt
{
class IDefinitionManager;
class IValueChangeNotifier;

/**
 *	Wrapper for accessing the current position in the undo/redo list from QML.
 */
class HistoryObject
{
public:
	// ClassDefinition::create() requires a default constructor
	HistoryObject();
	void init( ICommandManager& commandSystem, IDefinitionManager& defManager );
	void fini();

	const IListModel * getHistory() const;
	const IValueChangeNotifier * currentIndexSource() const;
	const ISelectionHandler * selectionHandlerSource() const;
	ObjectHandle createMacro() const;

    void setClearButtonVisible(const bool& isVisible);
    bool isClearButtonVisible() const;

    void setMakeMacroButtonVisible(const bool& isVisible);
    bool isMakeMacroButtonVisible() const;

private:
	void pushHistoryItems( const VariantList& history );
	void bindCommandHistoryCallbacks();
	void unbindCommandHistoryCallbacks();

	void onPostCommandHistoryInserted( const VariantList & history, size_t index, size_t count );
	void onPostCommandHistoryRemoved( const VariantList & history, size_t index, size_t count );
	void onCommandHistoryPreReset( const VariantList & history );
	void onCommandHistoryPostReset( const VariantList & history );

	void onPostHistoryItemsRemoved( size_t index, size_t count );

	ICommandManager* commandSystem_;
	IDefinitionManager* defManager_;
	// TODO: NGT-849
	// Eventually, we need to remove this
	SelectionHandler selectionHandler_;
	VariantList historyItems_;
	Connection postHistoryItemsRemoved_;
	ConnectionHolder historyCallbacks_;
	std::unique_ptr<IValueChangeNotifier> currentIndexNotifier_;

    bool clearButtonVisible;
    bool makeMacroButtonVisible;
};
} // end namespace wgt
#endif // HISTORY_OBJECT_HPP
