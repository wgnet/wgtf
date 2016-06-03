#include "command_manager.hpp"
#include "command_instance.hpp"
#include "compound_command.hpp"
#include "undo_redo_command.hpp"
#include "i_command_event_listener.hpp"
#include "core_generic_plugin/interfaces/i_application.hpp"
//TODO: remove this pragma
#pragma warning (push)
#pragma warning (disable : 4996 )
#include "core_data_model/generic_list.hpp"
#pragma warning( pop )
#include "core_data_model/value_change_notifier.hpp"
#include "core_data_model/variant_list.hpp"
#include "core_variant/variant.hpp"
#include "wg_types/hashed_string_ref.hpp"
#include "core_reflection/property_accessor.hpp"
#include "core_reflection/utilities/reflection_utilities.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_serialization/serializer/i_serializer.hpp"
#include "core_logging/logging.hpp"
#include "batch_command.hpp"
#include <atomic>
#include <deque>
#include <map>
#include <mutex>
#include "core_common/wg_condition_variable.hpp"
#include "core_common/thread_local_value.hpp"
#include "i_env_system.hpp"
#include "core_serialization/i_file_system.hpp"
#include "core_serialization/serializer/xml_serializer.hpp"
#include "wg_types/binary_block.hpp"
#include "reflection_undo_redo_data.hpp"
#include <memory>

// TODO: Remove to platform string header
#if defined( _WIN32 )
#define snprintf sprintf_s
#endif


namespace wgt
{
namespace
{
	static const char * s_historyVersion = "_ui_main_ver_1_0_14";
	typedef XMLSerializer HistorySerializer;
	const int NO_SELECTION = -1;
	static const char* s_macro_file = "macro";

	struct CommandFrame
	{
		CommandFrame( const CommandInstancePtr & instance )
		{
			stackQueue_.push_back( instance );
			commandStack_.push_back( instance );
		}

		std::deque< CommandInstancePtr > stackQueue_;
		std::deque< CommandInstancePtr > commandStack_;
		std::deque< CommandInstancePtr > commandQueue_;
	};

	class HistoryEnvCom : public IEnvComponent
	{
		DEFINE_EC_GUID
	public:
		HistoryEnvCom()
			: index_( NO_SELECTION )
			, previousSelectedIndex_( NO_SELECTION )
			, commandFrames_()
			, currentFrame_( nullptr )
		{
			commandFrames_.push_back( new CommandFrame( nullptr ) );
			THREAD_LOCAL_SET( currentFrame_, commandFrames_.back() );
		}

		virtual ~HistoryEnvCom()
		{
			assert( commandFrames_.size() == 1 );
			delete commandFrames_.back();
			commandFrames_.clear();
		}

		int																index_;
		int																previousSelectedIndex_;
		std::deque< CommandInstancePtr >	pendingHistory_;
		VariantList												history_;
		std::vector< CommandFrame * >			commandFrames_;
		THREAD_LOCAL( CommandFrame *)			currentFrame_;
	};

	DECLARE_EC_GUID( HistoryEnvCom, 0x4ee1ae34u, 0xd5294b5fu, 0x82ebf5d4u, 0xb3c6380eu );

class CommandManagerImpl : public IEnvEventListener
{
public:

	CommandManagerImpl( CommandManager* pCommandManager )
		: historyState_( &nullHistoryState_ )
		, currentIndex_( NO_SELECTION )
		, ownerThreadId_( std::this_thread::get_id() )
		, workerThreadId_()
		, workerMutex_()
		, workerWakeUp_()
		, ownerWakeUp_( false )
		, commands_()
		, macros_()
		, eventListenerCollection_()
		, globalEventListener_()
		, exiting_( false )
		, enableWorker_( true )
		, pCommandManager_( pCommandManager )
		, workerThread_()
		, batchCommand_( pCommandManager )
		, undoRedoCommand_( pCommandManager )
		, application_( nullptr )
		, envManager_( nullptr )
        , historySerializationEnabled(true)
	{
	}

	~CommandManagerImpl()
	{
	}

	void init( IApplication & application, IEnvManager & envManager );
	void fini();
	void update();
	void registerCommand( Command * command );
	void deregisterCommand( const char * commandName );
	Command * findCommand(const char * commandName ) const;
	CommandInstancePtr queueCommand(
		const char * commandName, const ObjectHandle & arguments );
	void queueCommand( const CommandInstancePtr & instance );
	void waitForInstance( const CommandInstancePtr & instance );
	void registerCommandStatusListener( ICommandEventListener * listener );
	void deregisterCommandStatusListener( ICommandEventListener * listener );
	void fireCommandStatusChanged( const CommandInstance & command ) const;
	void fireProgressMade( const CommandInstance & command ) const;
    void fireCommandExecuted(const CommandInstance & command, CommandOperation operation) const;
	void updateSelected( const int & value );

	void undo();
	void redo();
	bool canUndo() const;
	bool canRedo() const;
    void removeCommands(const ICommandManager::TRemoveFunctor & functor);
	VariantList & getHistory();
	IListModel & getMacros();

	void beginBatchCommand();
	void endBatchCommand();
	void abortBatchCommand();
	void notifyBeginMultiCommand();
	void notifyCompleteMultiCommand();
	void notifyCancelMultiCommand();
	void notifyHandleCommandQueued( const char * commandId );
	void notifyNonBlockingProcessExecution( const char * commandId );

	void pushFrame( const CommandInstancePtr & instance );
	void popFrame();
	bool createCompoundCommand( const VariantList & commandInstanceList, const char * id );
	bool deleteCompoundCommand( const char * id );
	void saveMacroList();
	void serializeMacroList( ISerializer & serializer );
	void loadMacroList();
	void deserializeMacroList( ISerializer & serializer );
	void addToHistory( const CommandInstancePtr & instance );
	void processCommands();
	void flush();
    void SetHistorySerializationEnabled(bool isEnabled);
	bool SaveCommandHistory( ISerializer & serializer, const HistoryEnvCom* ec ) const;
	bool LoadCommandHistory( ISerializer & serializer, HistoryEnvCom* ec ) const;
	void threadFunc();

	// IEnvEventListener
	virtual void onAddEnv( IEnvState* state ) override;
	virtual void onRemoveEnv( IEnvState* state ) override;
	virtual void onSelectEnv( IEnvState* state ) override;
    virtual void onSaveEnvState( IEnvState* state ) override;
    virtual void onLoadEnvState( IEnvState* state ) override;

	HistoryEnvCom nullHistoryState_;
	HistoryEnvCom* historyState_;
	ValueChangeNotifier< int > currentIndex_;
	ConnectionHolder indexConnections_;
	ConnectionHolder historyConnections_;
	Connection updateConnection_;

	std::thread::id ownerThreadId_;
	std::thread::id workerThreadId_;

private:
	void bindIndexCallbacks();
	void unbindIndexCallbacks();
	void bindHistoryCallbacks();
	void unbindHistoryCallbacks();

	void switchEnvContext(HistoryEnvCom* ec);

	typedef std::unordered_map< HashedStringRef, Command * > CommandCollection;
	typedef std::list< ICommandEventListener * > EventListenerCollection;

	/*
	Guard data shared with worker thread.
	Try to cover as less code as possible by mutex, but not less than required.
	Especially avoid calling user callbacks while holding it.
	*/
	std::mutex								workerMutex_;

	/*
	Assumed predicate: worker has something to do (at least one of these):
	- commandQueue_ is not empty
	- exiting_ == true
	*/
	wg_condition_variable					workerWakeUp_;
	std::atomic<bool>						ownerWakeUp_;

	CommandCollection						commands_;

	GenericListT< ObjectHandleT< CompoundCommand > > macros_;
	EventListenerCollection					eventListenerCollection_;
	std::unique_ptr< ICommandEventListener > globalEventListener_;

	bool									exiting_;
	bool									enableWorker_;
	CommandManager*							pCommandManager_;
	std::thread								workerThread_;
	BatchCommand							batchCommand_;
	UndoRedoCommand							undoRedoCommand_;

	IApplication *							application_;
	IEnvManager *								envManager_;
    bool historySerializationEnabled;

	void multiCommandStatusChanged( ICommandEventListener::MultiCommandStatus status );
	void onPreDataChanged();
	void onPostDataChanged();
	void onPostItemsInserted( size_t index, size_t count );
	void onPostItemsRemoved( size_t index, size_t count );

	void addBatchCommandToCompoundCommand(
		const ObjectHandleT<CompoundCommand> & compoundCommand,
		const CommandInstancePtr & instance );
};

//==============================================================================
void CommandManagerImpl::init( IApplication & application, IEnvManager & envManager )
{
	application_ = &application;
	updateConnection_ = application_->signalUpdate.connect( std::bind( &CommandManagerImpl::update, this ) );

	CommandManagerEventListener *
		listener = new CommandManagerEventListener();
	listener->setCommandSystemProvider( pCommandManager_ );
	globalEventListener_.reset( listener );

	registerCommand( &batchCommand_ );
	registerCommand( &undoRedoCommand_ );

	bindIndexCallbacks();
	bindHistoryCallbacks();

	if (enableWorker_)
	{
		workerThread_ = std::thread( &CommandManagerImpl::threadFunc, this );
		workerThreadId_ = workerThread_.get_id();
	}
	else
	{
		workerThreadId_ = ownerThreadId_;
	}

	envManager_ = &envManager;
	envManager_->registerListener( this );

	//loadMacroList();
}

//==============================================================================
void CommandManagerImpl::fini()
{
	//saveMacroList();

	abortBatchCommand();

	assert( envManager_ != nullptr );
	envManager_->deregisterListener( this );

	updateConnection_.disconnect();

	{
		// mutex lock is needed here to ensure new exiting_ value
		// is visible in other thread (due to memory barrier introduced by lock/unlock)
		std::unique_lock<std::mutex> lock( workerMutex_ );
		exiting_ = true;
		workerWakeUp_.notify_all();
	}

	if (enableWorker_)
	{
		workerThread_.join();
	}

	unbindIndexCallbacks();
	unbindHistoryCallbacks();
}

//==============================================================================
void CommandManagerImpl::update()
{
	// Optimisation to early out before calling processCommands which will attempt to acquire a mutex
	if (!ownerWakeUp_)
	{
		return;
	}

	processCommands();

	ownerWakeUp_ = false;
}

//==============================================================================
void CommandManagerImpl::registerCommand( Command * command )
{
	auto findIt = commands_.find( command->getId() );
	if (findIt != commands_.end())
	{
		assert( false );
		return;
	}
	commands_.insert(
		std::make_pair( command->getId(), command ) );
	command->registerCommandStatusListener( globalEventListener_.get() );
	command->setCommandSystemProvider( pCommandManager_ );
}

//==============================================================================
void CommandManagerImpl::deregisterCommand( const char * commandId )
{
	auto findIt = commands_.find( commandId );
	if (findIt != commands_.end())
	{
		commands_.erase( findIt );
	}
}


//==============================================================================
Command * CommandManagerImpl::findCommand(const char * commandId ) const
{
	auto findIt = commands_.find( commandId );
	if (findIt != commands_.end())
	{
		return findIt->second;
	}
	return nullptr;
}


//==============================================================================
CommandInstancePtr CommandManagerImpl::queueCommand(
	const char * commandId, const ObjectHandle & arguments )
{
	Command * command = findCommand( commandId );
	if(command == nullptr)
	{
		NGT_ERROR_MSG( "Command %s not found. \n", commandId );
		return nullptr;
	}

	auto instance =
		pCommandManager_->getDefManager().create< CommandInstance >();
	instance->setCommandSystemProvider( pCommandManager_ );
	instance->setCommandId( command ->getId() );
	instance->setArguments( arguments );
	instance->setDefinitionManager( pCommandManager_->getDefManager() );
	instance->setStatus( Queued );
	queueCommand( instance );
	return instance;
}


//==============================================================================
void CommandManagerImpl::queueCommand( const CommandInstancePtr & instance )
{
	std::thread::id currentThreadId = std::this_thread::get_id();
	assert( ( currentThreadId == workerThreadId_ || currentThreadId == ownerThreadId_ )
		&& "queueCommand can only be called in command thread and owner thread. \n" );

	{
		std::unique_lock<std::mutex> lock( workerMutex_ );

		// Push the command onto the queue of the relevant command frame, determined by the current thread
		auto commandFrame = THREAD_LOCAL_GET( historyState_->currentFrame_ );
		commandFrame->commandQueue_.push_back( instance );

		// If the command is a batch command we need to push/pop to the current command frames stack queue.
		// The stack queue is used to record pending batch command groups that have not been processed yet.
		if (strcmp( instance->getCommandId(), typeid( BatchCommand ).name() ) == 0)
		{
			auto stage = instance->getArguments().getBase<BatchCommandStage>();
			assert( stage != nullptr );
			if (*stage == BatchCommandStage::Begin)
			{
				commandFrame->stackQueue_.push_back( instance );
			}
			else
			{
				if (!commandFrame->stackQueue_.empty())
				{
					commandFrame->stackQueue_.pop_back();
				}
				else
				{
					NGT_WARNING_MSG( "Command frame queue is already empty\n" );
				}
			}
		}
	}

	// Try to execute the queued commands instantly.
	// This will either execute the command or notify the appropriate thread to start processing
	processCommands();
}


//==============================================================================
void CommandManagerImpl::waitForInstance( const CommandInstancePtr & instance )
{
	// TODO: Introduce a timeout option - primarily for the unit tests

	// The stack queue (pending Batch Command groups) will tell us the actual commmand to wait for.
	// For example in the following scenario
	// 0: BeginBatchCommand
	// 1:  BeginBatchCommand
	// 2:   SetProperty-A
	// 3:  EndBatchCommand
	// 4:  WaitForCommand-2
	// 5: EndBatchCommand
	// command 4 actually needs to wait for command 1, as the result of command 2 should not be visible to
	// command 4 until the BatchCommand it belongs to has completed.

	std::deque< CommandInstancePtr > stackQueue;
	std::deque< CommandInstancePtr > commandQueue;
	CommandInstancePtr batchFrame = nullptr;
	{
		std::unique_lock<std::mutex> lock( workerMutex_ );
		auto commandFrame = THREAD_LOCAL_GET( historyState_->currentFrame_ );
		stackQueue = commandFrame->stackQueue_;
		commandQueue = commandFrame->commandQueue_;
	}

	auto first = commandQueue.begin();
	auto last = std::find( commandQueue.begin(), commandQueue.end(), instance );
	for (auto batch = stackQueue.rbegin(); batch != stackQueue.rend(); ++batch)
	{
		auto it = std::find( first, last, *batch );
		if (it != last)
		{
			first = it;
			break;
		}
	}

	auto it = last;
	auto waitFor = instance;
	while (waitFor != nullptr)
	{
		while (waitFor->status_ != Complete)
		{
			processCommands();

			// TODO: This is piggy backing on now defunct command status messages.
			// This needs to be revised.
			fireProgressMade( *waitFor );
		}

		CommandInstancePtr parent = nullptr;
		while (it != first)
		{
			--it;
			if (std::find( (*it)->children_.begin(), (*it)->children_.end(), waitFor ) != (*it)->children_.end())
			{
				parent = *it;
				break;
			}
		}
		waitFor = parent;
	}
}


//==============================================================================
void CommandManagerImpl::registerCommandStatusListener(
	ICommandEventListener * listener )
{
	eventListenerCollection_.push_back( listener );
}


//==============================================================================
void CommandManagerImpl::deregisterCommandStatusListener(
	ICommandEventListener * listener )
{
	auto it = std::find( eventListenerCollection_.begin(), eventListenerCollection_.end(), listener );
	if (it != eventListenerCollection_.end())
	{
		eventListenerCollection_.erase( it );
	}
}


//==============================================================================
void CommandManagerImpl::fireCommandStatusChanged( const CommandInstance & command ) const
{
	EventListenerCollection::const_iterator it =
		eventListenerCollection_.begin();
	EventListenerCollection::const_iterator itEnd =
		eventListenerCollection_.end();
	for( ; it != itEnd; ++it )
	{
		(*it)->statusChanged( command );
	}
}


//==============================================================================
void CommandManagerImpl::fireProgressMade( const CommandInstance & command ) const
{
	EventListenerCollection::const_iterator it =
		eventListenerCollection_.begin();
	EventListenerCollection::const_iterator itEnd =
		eventListenerCollection_.end();
	for( ; it != itEnd; ++it )
	{
		(*it)->progressMade( command );
	}
}


void CommandManagerImpl::fireCommandExecuted(const CommandInstance & command, CommandOperation operation) const
{
    EventListenerCollection::const_iterator it =
        eventListenerCollection_.begin();
    EventListenerCollection::const_iterator itEnd =
        eventListenerCollection_.end();
    for (; it != itEnd; ++it)
    {
        (*it)->commandExecuted(command, operation);
    }
}

//==============================================================================
void CommandManagerImpl::updateSelected( const int & value )
{
	unbindIndexCallbacks();
	pCommandManager_->signalPreCommandIndexChanged( currentIndex_.value() );

    historyState_->index_ = value;
    historyState_->previousSelectedIndex_ = value;
	currentIndex_.value( value );

	pCommandManager_->signalPostCommandIndexChanged( currentIndex_.value() );
	bindIndexCallbacks();
}


//==============================================================================
bool CommandManagerImpl::canUndo() const
{
	if ((historyState_->history_.empty() || (historyState_->previousSelectedIndex_ < 0)))
	{
		return false;
	}
	return true;
}

//==============================================================================
bool CommandManagerImpl::canRedo() const
{
	if (!historyState_->history_.empty() &&
		(historyState_->previousSelectedIndex_ != (( int ) historyState_->history_.size() - 1)))
	{
		return true;
	}
	return false;
}
    
void CommandManagerImpl::removeCommands(const ICommandManager::TRemoveFunctor & functor)
{
    flush();
    std::unique_lock<std::mutex> lock( workerMutex_ );
    unbindHistoryCallbacks();
	unbindIndexCallbacks();


    int currentIndexValue = currentIndex_.value();
    int commandIndex = 0;

	int prevSelecetedIndexValue = historyState_->previousSelectedIndex_;
	int prevSelecetedIndex = 0;

	pCommandManager_->signalPreCommandIndexChanged( currentIndexValue );
	pCommandManager_->signalHistoryPreReset( historyState_->history_ );

    VariantList::Iterator iter = historyState_->history_.begin();
    while(iter != historyState_->history_.end())
    {
        if (functor((*iter).value<CommandInstancePtr>()))
        {
            iter = historyState_->history_.erase(iter);
            if (commandIndex < currentIndexValue)
            {
                currentIndexValue = std::max(currentIndexValue - 1, -1);
            }

			if (prevSelecetedIndex < prevSelecetedIndexValue)
			{
				prevSelecetedIndexValue = std::max( prevSelecetedIndexValue - 1, -1 );
			}
        }
        else
        {
            ++iter;
            ++commandIndex;
			++prevSelecetedIndex;
        }
    }


	historyState_->previousSelectedIndex_ = prevSelecetedIndexValue;
	currentIndex_.variantValue( currentIndexValue );
	historyState_->index_ = currentIndexValue;

	pCommandManager_->signalHistoryPostReset( historyState_->history_ );
	pCommandManager_->signalPostCommandIndexChanged( currentIndexValue );

    bindIndexCallbacks();
    bindHistoryCallbacks();
}

//==============================================================================
void CommandManagerImpl::undo()
{

	flush();
	assert(currentIndex_.value() >= 0);
	assert(canUndo());
	currentIndex_.value( currentIndex_.value() - 1 );
}


//==============================================================================
void CommandManagerImpl::redo()
{
	flush();
	assert(currentIndex_.value() < (int)historyState_->history_.size());
	assert(canRedo());
	currentIndex_.value(currentIndex_.value() + 1);
}

//==============================================================================
VariantList & CommandManagerImpl::getHistory()
{
	flush();
	return historyState_->history_;
}

//==============================================================================
IListModel & CommandManagerImpl::getMacros()
{
	return macros_;
}

//==============================================================================
void CommandManagerImpl::beginBatchCommand()
{
	notifyBeginMultiCommand();
	this->queueCommand( getClassIdentifier<BatchCommand>(), BatchCommandStage::Begin );
}

//==============================================================================
void CommandManagerImpl::endBatchCommand()
{
	this->queueCommand( getClassIdentifier<BatchCommand>(), BatchCommandStage::End );
}

//==============================================================================
void CommandManagerImpl::abortBatchCommand()
{
	this->queueCommand( getClassIdentifier<BatchCommand>(), BatchCommandStage::Abort );
}

//==============================================================================
void CommandManagerImpl::notifyBeginMultiCommand()
{
	multiCommandStatusChanged( ICommandEventListener::MultiCommandStatus_Begin );
}

//==============================================================================
void CommandManagerImpl::notifyCompleteMultiCommand()
{
	multiCommandStatusChanged( ICommandEventListener::MultiCommandStatus_Complete );
}

//==============================================================================
void CommandManagerImpl::notifyCancelMultiCommand()
{
	multiCommandStatusChanged( ICommandEventListener::MultiCommandStatus_Cancel );
}

//==============================================================================
void CommandManagerImpl::notifyHandleCommandQueued( const char * commandId )
{
	for ( auto & it : eventListenerCollection_ )
	{
		it->handleCommandQueued( commandId );
	}
}

//==============================================================================
void CommandManagerImpl::notifyNonBlockingProcessExecution( const char * commandId )
{
	for ( auto & it : eventListenerCollection_ )
	{
		it->onNonBlockingProcessExecution( commandId );
	}
}

//==============================================================================
namespace
{
	bool isBatchCommand(const ObjectHandleT<CommandInstance>& cmd)
	{
		return strcmp( cmd->getCommandId(), typeid( BatchCommand ).name() ) == 0;
	}
}

//==============================================================================
void CommandManagerImpl::pushFrame( const CommandInstancePtr & instance )
{
	assert( instance != nullptr );
	assert( !historyState_->commandFrames_.empty() );
	auto currentFrame = historyState_->commandFrames_.back();
	assert ( !currentFrame->commandStack_.empty() );
	auto parentInstance = currentFrame->commandStack_.back();

	instance->setStatus( Running );

	historyState_->commandFrames_.push_back( new CommandFrame( instance ) );

	if (isBatchCommand( instance ))
	{
		auto stage = instance->getArguments().getBase<BatchCommandStage>();
		assert( stage != nullptr );
		if (*stage != BatchCommandStage::Begin)
		{
			return;
		}

		// The command stack represents BatchCommand groups within a frame.
		// On BeginBatchCommand we need to add a group to the stack
		currentFrame->commandStack_.push_back( instance );
	}

	if (parentInstance != nullptr)
	{
		parentInstance->children_.push_back( instance );
	}
}

//==============================================================================
void CommandManagerImpl::popFrame()
{
	assert( !historyState_->commandFrames_.empty() );
	auto currentFrame = historyState_->commandFrames_.back();
	assert ( !currentFrame->commandStack_.empty() );
	auto instance = currentFrame->commandStack_.back();
	assert ( instance != nullptr );
	currentFrame->commandStack_.pop_back();
	assert( currentFrame->commandStack_.empty() && currentFrame->commandQueue_.empty() );
	delete historyState_->commandFrames_.back();
	historyState_->commandFrames_.pop_back();
	assert( !historyState_->commandFrames_.empty() );
	currentFrame = historyState_->commandFrames_.back();
	auto parentInstance = currentFrame->commandStack_.back();

	if (isBatchCommand( instance ))
	{
		auto stage = instance->getArguments().getBase<BatchCommandStage>();
		assert( stage != nullptr );
		// Set the arguments to nullptr since there is no need to serialize BatchCommand arguments
		instance->setArguments( nullptr );

		if (*stage == BatchCommandStage::Begin)
		{
			return;
		}

		// If we are ending or aborting a BatchCommand, we need to remove its group from the stack
		if ( parentInstance != nullptr )
		{
			currentFrame->commandStack_.pop_back();
			auto errorCode = instance->errorCode_;
			instance = parentInstance;
			instance->errorCode_ = errorCode;
			parentInstance = currentFrame->commandStack_.back();
		}
		else
		{
			NGT_WARNING_MSG( "Remmoving from an empty batch command group\n" );
		}
	}

    if (instance->getErrorCode() != CommandErrorCode::COMMAND_NO_ERROR)
	{
		instance->consolidateUndoRedoData( nullptr );
		instance->undo();
		if (instance->isMultiCommand())
		{
			notifyCancelMultiCommand();
		}
	}
	else
	{
		instance->consolidateUndoRedoData( parentInstance.get() );
		if (parentInstance == nullptr)
		{
			// If we are popping a root command frame, finalise the undo/redo stream and add the command to history
			addToHistory( instance );
			if (instance->isMultiCommand())
			{
				notifyCompleteMultiCommand();
			}
		}
	}

	instance->setStatus( Complete );
}

//==============================================================================
void CommandManagerImpl::addToHistory( const CommandInstancePtr & instance )
{
	if (instance.get()->getCommand()->canUndo( instance.get()->getArguments() ))
	{
		historyState_->pendingHistory_.push_back( instance );
	}
}

//==============================================================================
bool CommandManagerImpl::SaveCommandHistory( ISerializer & serializer, const HistoryEnvCom* ec ) const
{
    if (!historySerializationEnabled)
        return true;

	NGT_WARNING_MSG( "History serialization functionality temporarily disabled" );
	// save objects
	/*size_t count = ec->history_.size();
	serializer.serialize( count );
	for (size_t i = 0; i < count; i++)
	{
        const CommandInstancePtr& cmdIns = ec->history_[i].value<CommandInstancePtr>();
		serializer.serialize( cmdIns );
        auto undoData = cmdIns->reflectionUndoRedoData_.getUndoData();
        auto redoData = cmdIns->reflectionUndoRedoData_.getRedoData();
        serializer.serialize( undoData );
        serializer.serialize( redoData );
	}
	// save history index
	serializer.serialize( ec->index_ );*/
	return true;
}

void CommandManagerImpl::SetHistorySerializationEnabled(bool isEnabled)
{
    historySerializationEnabled = isEnabled;
}

//==============================================================================
bool CommandManagerImpl::LoadCommandHistory( ISerializer & serializer, HistoryEnvCom* ec ) const
{
    if (!historySerializationEnabled)
        return false;

	NGT_WARNING_MSG( "History serialization functionality temporarily disabled" );
	// read history data
	/*size_t count = 0;
	serializer.deserialize( count );
	for(size_t i = 0; i < count; i++)
	{
		Variant variant;
		serializer.deserialize( variant );
		CommandInstancePtr ins;
		bool isOk = variant.tryCast( ins );
		assert( isOk );
		assert( ins != nullptr );
        std::shared_ptr<BinaryBlock> undoData;
        std::shared_ptr<BinaryBlock> redoData;
        serializer.deserialize( undoData );
        serializer.deserialize( redoData );
        ins->reflectionUndoRedoData_.setUndoData( undoData );
        ins->reflectionUndoRedoData_.setRedoData( redoData );
		ins->setCommandSystemProvider( pCommandManager_ );
		ins->setDefinitionManager( pCommandManager_->getDefManager() );
		ec->history_.emplace_back( std::move( variant ) );
	}
	int index = NO_SELECTION;
	serializer.deserialize( ec->index_ );
	ec->previousSelectedIndex_ = ec->index_;*/
	return true;
}

//==============================================================================
void CommandManagerImpl::multiCommandStatusChanged( ICommandEventListener::MultiCommandStatus status )
{
	for ( auto & it : eventListenerCollection_ )
	{
		it->multiCommandStatusChanged( status );
	}
}

//==============================================================================
void CommandManagerImpl::onPreDataChanged()
{
	historyState_->previousSelectedIndex_ = currentIndex_.value();
	pCommandManager_->signalPreCommandIndexChanged( historyState_->previousSelectedIndex_ );
}

//==============================================================================
void CommandManagerImpl::onPostDataChanged()
{
	static const char* id = typeid( UndoRedoCommand ).name();
	auto instance = queueCommand( id, currentIndex_.value() );
	waitForInstance( instance );

	historyState_->index_ = currentIndex_.value();
	pCommandManager_->signalPostCommandIndexChanged( historyState_->index_ );
}

//==============================================================================
void CommandManagerImpl::onPostItemsInserted( size_t index, size_t count )
{
	pCommandManager_->signalHistoryPostInserted( historyState_->history_, index, count);
}

//==============================================================================
void CommandManagerImpl::onPostItemsRemoved( size_t index, size_t count )
{
	// update currentIndex when history_ items removed
	if(historyState_->history_.empty())
	{
		updateSelected( NO_SELECTION );
	}
	else
	{
		const int & size = static_cast<int>(historyState_->history_.size());
		if (currentIndex_.value() > size)
		{
			// goes here means history was corrupt.
			assert( false );
		}
	}
	pCommandManager_->signalHistoryPostRemoved(historyState_->history_, index, count);
}


//==============================================================================
void CommandManagerImpl::processCommands()
{
    {
        std::thread::id currentThreadId = std::this_thread::get_id();

        std::unique_lock<std::mutex> lock( workerMutex_ );

        for (;;)
        {
            auto commandFrame = historyState_->commandFrames_.back();		
            if (commandFrame->commandQueue_.empty())
            {
                break;
            }

            auto job = commandFrame->commandQueue_.front();
            auto threadAffinity = job->getCommand()->threadAffinity();
            if (threadAffinity == CommandThreadAffinity::UI_THREAD &&
                currentThreadId != ownerThreadId_)
            {
                // The next command in the queue needs to be run on the UI thread.
                // Notify the owner thread
                ownerWakeUp_ = true;
                break;
            }
            else if (threadAffinity == CommandThreadAffinity::COMMAND_THREAD &&
                currentThreadId != workerThreadId_)
            {
                // The next command in the queue needs to be run on the command thread.
                // Notify the worker thread
                workerWakeUp_.notify_all();
                break;
            }

            commandFrame->commandQueue_.pop_front();

            if (strcmp( job->getCommandId(), typeid( UndoRedoCommand ).name() ) == 0)
            {
                //execute undo/redo
                lock.unlock(); // release lock while running commands
                job->setStatus( Running );
                job->execute();
                job->setStatus( Complete );
                lock.lock();
            }
            else
            {
                // Push a command frame for this job
                auto previousFrame = THREAD_LOCAL_GET( historyState_->currentFrame_ );
                pushFrame( job );
                auto currentFrame = historyState_->commandFrames_.back();
                THREAD_LOCAL_SET( historyState_->currentFrame_, currentFrame );

                lock.unlock(); // release lock while running commands
                job->execute();
                lock.lock();

                // Spin and process commands until all sub commands for this frame have been executed
                while (!currentFrame->commandQueue_.empty() || historyState_->commandFrames_.back() != currentFrame )
                {
                    lock.unlock();
                    processCommands();
                    lock.lock();
                }

                // Pop the command frame
                THREAD_LOCAL_SET( historyState_->currentFrame_, previousFrame );
                popFrame();
            }
        }

        if (currentThreadId != ownerThreadId_)
        {
            if (!historyState_->pendingHistory_.empty())
            {
                ownerWakeUp_ = true;
            }
            return;
        }

        if (historyState_->pendingHistory_.empty())
        {
            return;
        }

        if (static_cast<int>(historyState_->history_.size()) > currentIndex_.value() + 1)
        {
            // erase all history after the current index as we have pending
            // history that will make this invalid
            historyState_->history_.resize( currentIndex_.value() + 1 );
        }

        while (!historyState_->pendingHistory_.empty())
        {
            historyState_->history_.push_back( historyState_->pendingHistory_.front() );
            historyState_->pendingHistory_.pop_front();
        }
    }
	updateSelected( static_cast< int >( historyState_->history_.size() - 1 ) );
}


//==============================================================================
void CommandManagerImpl::flush()
{
	assert( std::this_thread::get_id() == ownerThreadId_ );

	std::unique_lock<std::mutex> lock( workerMutex_ );

	while (historyState_->commandFrames_.size() > 1 ||
		!historyState_->commandFrames_.front()->commandQueue_.empty() ||
		!historyState_->pendingHistory_.empty())
	{
		lock.unlock();
		processCommands();
		lock.lock();
	}
}

//==============================================================================
/*static */void CommandManagerImpl::threadFunc()
{
	std::unique_lock<std::mutex> lock( workerMutex_ );

	while (exiting_ == false)
	{
		workerWakeUp_.wait( lock, [this]
		{
			auto & commandFrame = *historyState_->commandFrames_.back();
			return
				!commandFrame.commandQueue_.empty() ||
				exiting_;
		});

		// execute commands
		lock.unlock();
		processCommands();
		lock.lock();
	}
}

void CommandManagerImpl::onAddEnv( IEnvState* state )
{
	ENV_STATE_ADD( HistoryEnvCom, ec );
}

void CommandManagerImpl::onRemoveEnv( IEnvState* state )
{
    ENV_STATE_REMOVE( HistoryEnvCom, ec );
    if (ec == historyState_)
    {
        switchEnvContext( &nullHistoryState_ );
    }
}

void CommandManagerImpl::onSelectEnv( IEnvState* state )
{
	ENV_STATE_QUERY( HistoryEnvCom, ec );
	if (ec != historyState_)
	{
		switchEnvContext(ec);
	}
}

void CommandManagerImpl::onLoadEnvState( IEnvState* state )
{
    ENV_STATE_QUERY( HistoryEnvCom, ec );
    std::string file = state->description();
    file += s_historyVersion;

    const IFileSystem* fileSystem = pCommandManager_->getFileSystem();
    assert( fileSystem );

    if (fileSystem->exists( file.c_str() ))
    {
        IDefinitionManager& defManager = pCommandManager_->getDefManager();

        IFileSystem::IStreamPtr fileStream = fileSystem->readFile( file.c_str(), std::ios::in | std::ios::binary );
        HistorySerializer serializer( *fileStream, defManager );
        std::string version;
        serializer.deserialize( version );
        if( version == s_historyVersion)
        {
            LoadCommandHistory( serializer, ec );
        }
    }
}
void CommandManagerImpl::onSaveEnvState( IEnvState* state )
{
    ENV_STATE_QUERY( HistoryEnvCom, ec );
    IDefinitionManager& defManager = pCommandManager_->getDefManager();
    ResizingMemoryStream stream;
    HistorySerializer serializer( stream, defManager );

    serializer.serialize( s_historyVersion );

    SaveCommandHistory( serializer, ec );

    std::string file = state->description();
    file += s_historyVersion;
    IFileSystem* fileSystem = pCommandManager_->getFileSystem();
    assert( fileSystem );
    fileSystem->writeFile( file.c_str(), stream.buffer().c_str(), stream.buffer().size(), std::ios::out | std::ios::binary );
}

void CommandManagerImpl::switchEnvContext(HistoryEnvCom* ec)
{
    flush();
	unbindHistoryCallbacks();
	unbindIndexCallbacks();
	pCommandManager_->signalPreCommandIndexChanged( currentIndex_.value() );
	currentIndex_.value( NO_SELECTION );
	pCommandManager_->signalHistoryPreReset( historyState_->history_ );
	{
		std::unique_lock<std::mutex> lock( workerMutex_ );
		historyState_ = ec;
	}
	pCommandManager_->signalHistoryPostReset( historyState_->history_ );
	currentIndex_.value( historyState_->index_ );
	pCommandManager_->signalPostCommandIndexChanged( currentIndex_.value() );
	bindIndexCallbacks();
	bindHistoryCallbacks();
}

void CommandManagerImpl::bindIndexCallbacks()
{
	using namespace std::placeholders;
	indexConnections_ += currentIndex_.signalPreDataChanged.connect( std::bind( &CommandManagerImpl::onPreDataChanged, this ) );
	indexConnections_ += currentIndex_.signalPostDataChanged.connect( std::bind( &CommandManagerImpl::onPostDataChanged, this ) );
}

void CommandManagerImpl::unbindIndexCallbacks()
{
	indexConnections_.clear();
}

void CommandManagerImpl::bindHistoryCallbacks()
{
	using namespace std::placeholders;
	historyConnections_ += historyState_->history_.signalPostItemsInserted.connect( std::bind( &CommandManagerImpl::onPostItemsInserted, this, _1, _2 ) );
	historyConnections_ += historyState_->history_.signalPostItemsRemoved.connect( std::bind( &CommandManagerImpl::onPostItemsRemoved, this, _1, _2 ) );
}

void CommandManagerImpl::unbindHistoryCallbacks()
{
	historyConnections_.clear();
}

}

//==============================================================================
CommandManager::CommandManager( IDefinitionManager & defManager )
	: pImpl_( new CommandManagerImpl( this ) )
	, defManager_( defManager )
	, fileSystem_( nullptr )
{
}


//==============================================================================
CommandManager::~CommandManager()
{
}

//==============================================================================
void CommandManager::init( IApplication & application, IEnvManager & envManager,
													IFileSystem * fileSystem, IReflectionController * controller )
{
	fileSystem_ = fileSystem;
	controller_ = controller;
	pImpl_->init( application, envManager );
}


//==============================================================================
void CommandManager::fini()
{
	pImpl_->fini();
}


//==============================================================================
void CommandManager::registerCommand( Command * command )
{
	pImpl_->registerCommand( command );
}


//==============================================================================
void CommandManager::deregisterCommand( const char * commandId )
{
	pImpl_->deregisterCommand( commandId );
}

//==============================================================================
Command * CommandManager::findCommand(
	const char * commandId ) const
{
	return pImpl_->findCommand( commandId );
}


//==============================================================================
CommandInstancePtr CommandManager::queueCommand(
	const char * commandId, const ObjectHandle & arguments )
{
	return pImpl_->queueCommand( commandId, arguments );
}


//==============================================================================
void CommandManager::waitForInstance( const CommandInstancePtr & instance )
{
	pImpl_->waitForInstance( instance );
}


//==============================================================================
void CommandManager::registerCommandStatusListener(
	ICommandEventListener * listener )
{
	return pImpl_->registerCommandStatusListener( listener );
}


//==============================================================================
void CommandManager::deregisterCommandStatusListener(
	ICommandEventListener * listener )
{
	return pImpl_->deregisterCommandStatusListener( listener );
}


//==============================================================================
void CommandManager::fireCommandStatusChanged( const CommandInstance & command ) const
{
	return pImpl_->fireCommandStatusChanged( command );
}


//==============================================================================
void CommandManager::fireProgressMade( const CommandInstance & command ) const
{
	return pImpl_->fireProgressMade( command );
}


void CommandManager::fireCommandExecuted(const CommandInstance & command, CommandOperation operation) const
{
    return pImpl_->fireCommandExecuted(command, operation);
}

//==============================================================================
bool CommandManager::canUndo() const
{
	return pImpl_->canUndo();
}


//==============================================================================
bool CommandManager::canRedo() const
{
	return pImpl_->canRedo();
}

void CommandManager::removeCommands(const TRemoveFunctor & functor)
{
    pImpl_->removeCommands(functor);
}


//==============================================================================
void CommandManager::undo()
{
	pImpl_->undo();
}


//==============================================================================
void CommandManager::redo()
{
	pImpl_->redo();
}


//==============================================================================
const VariantList & CommandManager::getHistory() const
{
	return pImpl_->getHistory();
}

//==============================================================================
const IListModel & CommandManager::getMacros() const
{
	return pImpl_->getMacros();
}

const int CommandManager::commandIndex() const
{
	return pImpl_->currentIndex_.value();
}

void CommandManager::moveCommandIndex( int newIndex )
{
	pImpl_->currentIndex_.value( newIndex );
}

//==============================================================================
IDefinitionManager & CommandManager::getDefManager() const
{
	return defManager_;
}

//==============================================================================
IFileSystem * CommandManager::getFileSystem() const
{
	return fileSystem_;
}

//==============================================================================
IReflectionController * CommandManager::getReflectionController() const
{
	return controller_;
}

//==============================================================================
bool CommandManager::SaveHistory( ISerializer & serializer )
{
	return pImpl_->SaveCommandHistory( serializer, pImpl_->historyState_ );
}

//==============================================================================
bool CommandManager::LoadHistory( ISerializer & serializer )
{
	return pImpl_->LoadCommandHistory( serializer, pImpl_->historyState_ );
}


ISelectionContext& CommandManager::selectionContext()
{
	return selectionContext_;
}


std::thread::id CommandManager::ownerThreadId() /* override */
{
	return pImpl_->ownerThreadId_;
}

//==============================================================================
void CommandManager::beginBatchCommand()
{
	pImpl_->beginBatchCommand();
}

//==============================================================================
void CommandManager::endBatchCommand()
{
	pImpl_->endBatchCommand();
}

void CommandManager::abortBatchCommand()
{
	pImpl_->abortBatchCommand();
}

//==============================================================================
void CommandManager::notifyBeginMultiCommand()
{
	pImpl_->notifyBeginMultiCommand();
}

//==============================================================================
void CommandManager::notifyCompleteMultiCommand()
{
	pImpl_->notifyCompleteMultiCommand();
}

//==============================================================================
void CommandManager::notifyCancelMultiCommand()
{
	pImpl_->notifyCancelMultiCommand();
}

//==============================================================================
void CommandManager::notifyHandleCommandQueued( const char * commandId )
{
	pImpl_->notifyHandleCommandQueued( commandId );
}

//==============================================================================
void CommandManager::notifyNonBlockingProcessExecution( const char * commandId )
{
	pImpl_->notifyNonBlockingProcessExecution( commandId );
}

void CommandManager::SetHistorySerializationEnabled(bool isEnabled)
{
    pImpl_->SetHistorySerializationEnabled(isEnabled);
}

//==============================================================================
bool CommandManagerImpl::createCompoundCommand(
	const VariantList & commandInstanceList, const char * id )
{
	// create compound command
	std::vector< size_t > commandIndices;
	size_t count = commandInstanceList.size();
	{
		std::unique_lock<std::mutex> lock( workerMutex_ );
		for(size_t i = 0; i < count; i++)
		{
			const Variant & variant = commandInstanceList[i];
			auto && findIt =
				std::find( historyState_->history_.begin(), historyState_->history_.end(), variant );
			if (findIt == historyState_->history_.end())
			{
				continue;
			}
			commandIndices.push_back( findIt - historyState_->history_.begin() );
		}
	}

	if (commandIndices.empty())
	{
		NGT_ERROR_MSG( "Failed to create macros: no command history. \n" );
		return false;
	}
	auto macro = pCommandManager_->getDefManager().create<CompoundCommand>( false );
	macro->setId( id );
	pCommandManager_->registerCommand( macro.get() );
	std::sort( commandIndices.begin(), commandIndices.end() );
	auto indexIt =
		commandIndices.begin();
	auto indexItEnd =
		commandIndices.end();
	for( ; indexIt != indexItEnd; ++indexIt )
	{
		const CommandInstancePtr & instance = historyState_->history_[*indexIt].value<CommandInstancePtr>();
		if (instance->isMultiCommand())
		{
			addBatchCommandToCompoundCommand( macro, instance );
		}
		else
		{
			macro->addCommand( instance->getCommandId(), instance->getArguments() );
		}
	}
	macro->initDisplayData( pCommandManager_->getDefManager(), pCommandManager_->getReflectionController() );
	macros_.emplace_back( std::move( macro ) );
	return true;
}

//==============================================================================
void CommandManagerImpl::addBatchCommandToCompoundCommand(
	const ObjectHandleT<CompoundCommand>& compoundCommand,
	const CommandInstancePtr & instance )
{
	assert( !instance->children_.empty() );
	for (auto & child : instance->children_)
	{
		if (child->isMultiCommand())
		{
			addBatchCommandToCompoundCommand( compoundCommand, child );
		}
		else
		{
			if (strcmp(child->getCommandId(), typeid(BatchCommand).name()) != 0)
			{
				compoundCommand->addCommand( child->getCommandId(), child->getArguments() );
			}
		}
	}
}

//==============================================================================
bool CommandManager::createMacro( const VariantList & commandInstanceList, const char * id )
{
	static int index = 1;
	const char* defaultName = "Macro";

	std::string macroName("");
	if (id != nullptr)
	{
		macroName = id;
	}
	if (findCommand( macroName.c_str() ) != nullptr )
	{
		NGT_ERROR_MSG( "Failed to create macros: macro name %s already exists. \n", macroName.c_str() );
		return false;
	}
	if (macroName.empty())
	{
		char buffer[260];
		do
		{
			snprintf( buffer, 260, "%d", index);
			macroName = defaultName;
			macroName += buffer;
			index++;
		}
		while (findCommand( macroName.c_str()) != nullptr);
	}
	return pImpl_->createCompoundCommand( commandInstanceList, macroName.c_str() );
}

//==============================================================================
bool CommandManagerImpl::deleteCompoundCommand( const char * id )
{
	bool bSuccess = false;
	CompoundCommand * compoundCommand = static_cast< CompoundCommand * >(findCommand( id ));
	if (compoundCommand != nullptr)
	{
		typedef GenericListT< ObjectHandleT< CompoundCommand > > MacroList;
		for(MacroList::Iterator iter = macros_.begin(); iter != macros_.end(); ++iter)
		{
			const  ObjectHandleT< CompoundCommand > & obj = *iter;
			bool isOk = ( strcmp( id, obj->getId() ) == 0);
			if(isOk)
			{
				// deregisterCommand( id ); evgenys: commands stack might hold a ref to this command, so remove macro only
				macros_.erase( iter );
				bSuccess = true;
				break;
			}
		}
	}
	return bSuccess;
}

void CommandManagerImpl::saveMacroList()
{
	IDefinitionManager& defManager = pCommandManager_->getDefManager();
	ResizingMemoryStream stream;
	XMLSerializer serializer( stream, defManager );
	serializer.serialize( s_historyVersion );
	serializeMacroList( serializer );
	std::string file = "macro";
	file += s_historyVersion;
	IFileSystem* fileSystem = pCommandManager_->getFileSystem();
	assert( fileSystem );
	fileSystem->writeFile( file.c_str(), stream.buffer().c_str(), stream.buffer().size(), std::ios::out | std::ios::binary );
}

void CommandManagerImpl::serializeMacroList( ISerializer & serializer )
{
	serializer.serialize( macros_.size() );
	for (auto& m : macros_)
	{
		m->serialize( serializer );
	}
}

void CommandManagerImpl::loadMacroList()
{
	std::string file = s_macro_file;
	file += s_historyVersion;

	const IFileSystem* fileSystem = pCommandManager_->getFileSystem();
	assert( fileSystem );

	if (fileSystem->exists( file.c_str() ))
	{
		IDefinitionManager& defManager = pCommandManager_->getDefManager();

		IFileSystem::IStreamPtr fileStream = fileSystem->readFile( file.c_str(), std::ios::in | std::ios::binary );
		XMLSerializer serializer( *fileStream, defManager );
		std::string version;
		serializer.deserialize( version );
		if (version == s_historyVersion)
		{
			deserializeMacroList( serializer );
		}
	}
}

void CommandManagerImpl::deserializeMacroList( ISerializer & serializer )
{
	macros_.clear();
	size_t size = 0;
	serializer.deserialize( size );
	std::string id;
	for (size_t i = 0; i < size; ++i)
	{
		auto macro = pCommandManager_->getDefManager().create<CompoundCommand>( false );
		serializer.deserialize( id );
		macro->setId( id.c_str() );
		pCommandManager_->registerCommand( macro.get() );
		macro->initDisplayData( pCommandManager_->getDefManager(), pCommandManager_->getReflectionController() );

		macro->deserialize( serializer );
		macros_.emplace_back( std::move( macro ) );
	}
}

//==============================================================================
bool CommandManager::deleteMacroByName( const char * id )
{
	return pImpl_->deleteCompoundCommand( id );
}

//==============================================================================
void CommandManager::addToHistory( const CommandInstancePtr & instance )
{
	pImpl_->addToHistory( instance );
}

//==============================================================================
bool CommandManager::undoRedo( const int & desiredIndex )
{
	assert( pImpl_ != nullptr );
	auto & history = this->getHistory();
	const int & size = static_cast<int>(history.size());
	if (size == 0)
	{
		//assert( false );
		return false;
	}
	if ((pImpl_->historyState_->previousSelectedIndex_ == desiredIndex) || (desiredIndex >= size))
	{
		//assert( false );
		return false;
	}
	while (pImpl_->historyState_->previousSelectedIndex_ != pImpl_->currentIndex_.value())
	{
		if (pImpl_->historyState_->previousSelectedIndex_ > pImpl_->currentIndex_.value())
		{
			int i = pImpl_->historyState_->previousSelectedIndex_;
			CommandInstancePtr job = history[i].value<CommandInstancePtr>();
			job->undo();
			pImpl_->historyState_->previousSelectedIndex_--;
		}
		else
		{
			int i = pImpl_->historyState_->previousSelectedIndex_;
			CommandInstancePtr job = history[i+1].value<CommandInstancePtr>();
			job->redo();
			pImpl_->historyState_->previousSelectedIndex_++;
		}
	}
	return true;
}
} // end namespace wgt
