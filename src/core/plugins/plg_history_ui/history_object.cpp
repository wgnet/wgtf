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


namespace wgt
{
struct CurrentIndexNotifier: public IValueChangeNotifier
{
	CurrentIndexNotifier( ICommandManager& commandManager )
		: commandManager_( commandManager )
	{
	}

	~CurrentIndexNotifier()
	{
		disconnect();
	}

	void connect()
	{
		using namespace std::placeholders;
		callbacks_ += commandManager_.signalPreCommandIndexChanged.connect( std::bind(
			&CurrentIndexNotifier::onPreCommandIndexChanged, this, _1 ) );
		callbacks_ += commandManager_.signalPostCommandIndexChanged.connect( std::bind(
			&CurrentIndexNotifier::onPostCommandIndexChanged, this, _1 ) );
	}

	void disconnect()
	{
		callbacks_.clear();
	}

	void onPreCommandIndexChanged( int index )
	{
		signalPreDataChanged();
	}

	void onPostCommandIndexChanged( int index )
	{
		signalPostDataChanged();
	}

	Variant variantValue() const override
	{
		return commandManager_.commandIndex();
	}

	bool variantValue( const Variant& data ) override
	{
		int value;

		if (!data.tryCast<int>( value ))
		{
			return false;
		}

		if (value != commandManager_.commandIndex())
		{
			commandManager_.moveCommandIndex( value );
		}

		return true;
	}

	ICommandManager& commandManager_;
	ConnectionHolder callbacks_;
};

//==============================================================================
HistoryObject::HistoryObject()
	: commandSystem_( nullptr )
    , clearButtonVisible(true)
    , makeMacroButtonVisible(true)
{
}

//==============================================================================
void HistoryObject::init( ICommandManager& commandSystem, IDefinitionManager& defManager )
{
	commandSystem_ = &commandSystem;
	defManager_ = &defManager;

	currentIndexNotifier_.reset( new CurrentIndexNotifier( *commandSystem_ ) );
	pushHistoryItems( commandSystem_->getHistory() );

	using namespace std::placeholders;
	postHistoryItemsRemoved_ = historyItems_.signalPostItemsRemoved.connect( std::bind( &HistoryObject::onPostHistoryItemsRemoved, this, _1, _2 ) );

	bindCommandHistoryCallbacks();
}

//==============================================================================
void HistoryObject::bindCommandHistoryCallbacks()
{
	using namespace std::placeholders;
	historyCallbacks_ += commandSystem_->signalHistoryPostInserted.connect( std::bind(
		&HistoryObject::onPostCommandHistoryInserted, this, _1, _2, _3 ) );
	historyCallbacks_ += commandSystem_->signalHistoryPostRemoved.connect( std::bind(
		&HistoryObject::onPostCommandHistoryRemoved, this, _1, _2, _3 ) );
	historyCallbacks_ += commandSystem_->signalHistoryPreReset.connect( std::bind(
		&HistoryObject::onCommandHistoryPreReset, this, _1 ) );
	historyCallbacks_ += commandSystem_->signalHistoryPostReset.connect( std::bind(
		&HistoryObject::onCommandHistoryPostReset, this, _1 ) );

	CurrentIndexNotifier* cin =
		static_cast<CurrentIndexNotifier*>( currentIndexNotifier_.get() );
	cin->connect();
}

//==============================================================================
void HistoryObject::unbindCommandHistoryCallbacks()
{
	CurrentIndexNotifier* cin =
		static_cast<CurrentIndexNotifier*>( currentIndexNotifier_.get() );
	cin->disconnect();

	historyCallbacks_.clear();
}

//==============================================================================
void HistoryObject::pushHistoryItems( const VariantList& history )
{
	size_t oldSize = historyItems_.size();
	size_t newSize = history.size();
	size_t overwriteSize = std::min( oldSize, newSize );
	size_t insertSize = newSize > oldSize ? newSize - oldSize : 0;

	auto getHistoryItem = [this]( const Variant& variantHistoryItem ) -> ObjectHandleT<DisplayObject>
	{
		auto displayObject = defManager_->create<DisplayObject>( false );
		auto instance = variantHistoryItem.value<CommandInstancePtr>();
		displayObject->init( *defManager_, instance );
		return displayObject;
	};

	for (size_t i = 0; i < overwriteSize; ++i)
	{
		Variant newValue = getHistoryItem( history[i] );
		IItem* item = historyItems_.item( i );
		ItemRole::Id roleId = ValueRole::roleId_;

		historyItems_.signalPreItemDataChanged( item, 0, roleId, newValue );
		item->setData( 0, roleId, newValue );
		historyItems_.signalPostItemDataChanged( item, 0, roleId, newValue );
	}

	if (insertSize > 0)
	{
		std::vector<Variant> newValues;

		for (size_t i = overwriteSize; i < overwriteSize + insertSize; ++i)
		{
			newValues.push_back(getHistoryItem(history[i]));
		}

		historyItems_.insert(historyItems_.end(), newValues.begin(), newValues.end());
	}

	historyItems_.resize( newSize );
}

//==============================================================================
void HistoryObject::fini()
{
	assert( commandSystem_ != nullptr );

	unbindCommandHistoryCallbacks();

	postHistoryItemsRemoved_.disconnect();
}

//==============================================================================
const IListModel * HistoryObject::getHistory() const
{
	return &historyItems_;
}

//==============================================================================
const IValueChangeNotifier * HistoryObject::currentIndexSource() const
{
	return currentIndexNotifier_.get();
}

//==============================================================================
const ISelectionHandler * HistoryObject::selectionHandlerSource() const
{
	return &selectionHandler_;
}

//==============================================================================
ObjectHandle HistoryObject::createMacro() const
{
	assert( commandSystem_ != nullptr );
	const VariantList & history = commandSystem_->getHistory();
	int size = static_cast<int>(history.size());
	VariantList commandList;
	const auto & selectionSet = selectionHandler_.getSelectedRows();
	if (selectionSet.empty())
	{
		return nullptr;
	}
	for( auto index : selectionSet)
	{
		if (index >= size)
		{
			assert( false );
			return nullptr;
		}
		commandList.push_back(history[index]);
	}
	commandSystem_->createMacro( commandList );
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

//==============================================================================
void HistoryObject::onPostCommandHistoryInserted( const VariantList & history, size_t index, size_t count )
{
	assert( commandSystem_ != nullptr );
	assert( defManager_ != nullptr );
	size_t historySize = history.size();
	historyItems_.resize( index );
	for (size_t i = 0; i < count; i++)
	{
		assert( index < historySize );
		auto displayObject = defManager_->create<DisplayObject>( false );
		auto instance = history[index++].value<CommandInstancePtr>();
		displayObject->init( *defManager_, instance );
		historyItems_.push_back( displayObject );
	}
	assert( historySize == historyItems_.size() );
}

//==============================================================================
void HistoryObject::onPostCommandHistoryRemoved( const VariantList & history, size_t index, size_t count )
{
	assert( commandSystem_ != nullptr );
	assert( defManager_ != nullptr );

	// detach listener to avoid event loop
	postHistoryItemsRemoved_.disable();
	historyItems_.erase( historyItems_.begin() + index, historyItems_.begin() + index + count );
	postHistoryItemsRemoved_.enable();

	assert( historyItems_.size() == history.size() );
}

//==============================================================================
void HistoryObject::onCommandHistoryPreReset( const VariantList & history )
{
	postHistoryItemsRemoved_.disable();
}

//==============================================================================
void HistoryObject::onCommandHistoryPostReset( const VariantList & history )
{
	pushHistoryItems( history );
	postHistoryItemsRemoved_.enable();

	selectionHandler_.setSelectedRows( std::vector< int >() );
	selectionHandler_.setSelectedItems( std::vector< IItem* >() );
}

//==============================================================================
void HistoryObject::onPostHistoryItemsRemoved( size_t index, size_t count )
{
	// handling user click on clear button
	assert( commandSystem_ != nullptr );

	// detach listener to avoid event loop
	unbindCommandHistoryCallbacks();

	VariantList & history = const_cast<VariantList &>(commandSystem_->getHistory());
	history.erase( history.begin() + index, history.begin() + index + count );

	bindCommandHistoryCallbacks();

	assert( history.size() == historyItems_.size() );
	selectionHandler_.setSelectedRows( std::vector< int >() );
	selectionHandler_.setSelectedItems( std::vector< IItem* >() );
}
} // end namespace wgt
