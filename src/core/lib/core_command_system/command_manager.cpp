#include "command_manager.hpp"
#include "core_dependency_system/depends.hpp"
#include "command_instance.hpp"
#include "compound_command.hpp"
#include "undo_redo_command.hpp"
#include "i_command_event_listener.hpp"
#include "core_generic_plugin/interfaces/i_application.hpp"
// TODO: remove this pragma
#pragma warning(push)
#include "core_variant/variant.hpp"
#include "wg_types/hashed_string_ref.hpp"
#include "core_reflection/property_accessor.hpp"
#include "core_reflection/utilities/reflection_utilities.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/i_object_manager.hpp"
#include "core_logging/logging.hpp"
#include "batch_command.hpp"
#include <atomic>
#include <deque>
#include <map>
#include <mutex>
#include "core_common/assert.hpp"
#include "core_common/wg_condition_variable.hpp"
#include "core_common/thread_local_value.hpp"
#include "wg_types/binary_block.hpp"
#include "reflection_undo_redo_data.hpp"
#include "core_environment_system/i_env_system.hpp"
#include <memory>

// TODO: Remove to platform string header
#if defined(_WIN32)
#define snprintf sprintf_s
#endif

namespace wgt
{

namespace
{

static const char* s_macroVersion = "_macro_ver_0_0_0";
const int NO_SELECTION = -1;
static const char* s_macro_file = "macro";

struct CommandFrame
{
	CommandFrame(const CommandInstancePtr& instance)
	{
		stackQueue_.push_back(instance);
		commandStack_.push_back(instance);
	}

	std::deque<CommandInstancePtr> stackQueue_;
	std::deque<CommandInstancePtr> commandStack_;
	std::deque<CommandInstancePtr> commandQueue_;
};

class HistoryEnvComponentState : public IEnvComponentState
{
public:
	HistoryEnvComponentState(CommandManagerImpl& pCmdMgrImpl)
	    : cmdMgrImpl_(pCmdMgrImpl)
        , index_(NO_SELECTION)
        , previousSelectedIndex_(NO_SELECTION)
        , commandFrames_()
        , currentFrame_(nullptr)
		, abortingBatchCommand_(false)
	{
		commandFrames_.emplace_back(new CommandFrame(nullptr));
		THREAD_LOCAL_SET(currentFrame_, commandFrames_.back().get());
	}

	virtual ~HistoryEnvComponentState()
	{
		TF_ASSERT(commandFrames_.size() == 1);
		commandFrames_.clear();
	}

	void saveState(IDataStream& stream);

	void loadState(IDataStream& stream);

	void resetState();

	CommandManagerImpl& cmdMgrImpl_;
	int index_;
	int previousSelectedIndex_;
	std::deque<CommandInstancePtr> pendingHistory_;
	std::vector<Variant> history_;

    std::vector<std::unique_ptr<CommandFrame>> commandFrames_;
	THREAD_LOCAL(CommandFrame*) currentFrame_;
	bool abortingBatchCommand_;
};

bool isBatchCommand(const CommandInstancePtr& cmd)
{
	return strcmp(cmd->getCommandId(), typeid(BatchCommand).name()) == 0;
}

class CommandManagerImpl : public EnvComponentT<HistoryEnvComponentState>
{
public:
	CommandManagerImpl(CommandManager* pCommandManager, IEnvManager& envManager)
	    : EnvComponentT(envManager), currentIndex_(NO_SELECTION), previousSelectedIndex_(nullptr),
	      ownerThreadId_(std::this_thread::get_id()), workerThreadId_(), workerMutex_(), workerWakeUp_(),
	      ownerWakeUp_(false), commands_(), globalEventListener_(), exiting_(false), enableWorker_(true),
	      pCommandManager_(pCommandManager), workerThread_(), batchCommand_(pCommandManager),
	      undoRedoCommand_(pCommandManager), application_(nullptr)
	{
		CommandManagerEventListener* listener = new CommandManagerEventListener();
		listener->setCommandSystemProvider(pCommandManager_);
		globalEventListener_.reset(listener);

		registerCommand(&batchCommand_);
		registerCommand(&undoRedoCommand_);
		macros_ = Collection(macroList_);
	}

	~CommandManagerImpl()
	{
		clearMacros();
	}

	void clearMacros();
	virtual const char* getEnvComponentId() const override;
	virtual std::unique_ptr<IEnvComponentState> createState() override;
	virtual void onPreEnvironmentChanged(const EnvironmentId& oldId, const EnvironmentId& newId) override;
	virtual void onPostEnvironmentChanged(const EnvironmentId& oldId, const EnvironmentId& newId) override;

	void init(IApplication& application);
	void fini();
	void update();
	void registerCommand(Command* command);
	void deregisterCommand(const char* commandName);
	Command* findCommand(const char* commandName) const;

    CommandInstancePtr makeCommand(const char* commandName) const;
    CommandInstancePtr queueCommand(LockedStateT<HistoryEnvComponentState>& state, const char* commandName, ManagedObjectPtr arguments);
	CommandInstancePtr queueCommand(LockedStateT<HistoryEnvComponentState>& state, const char* commandName, const ObjectHandle& arguments);
    CommandInstancePtr queueCommand(LockedStateT<HistoryEnvComponentState>& state, CommandInstancePtr instance);

	void waitForInstance(LockedStateT<HistoryEnvComponentState>& state, const CommandInstancePtr& instance);
	void updateSelected(LockedStateT<HistoryEnvComponentState>& state, const int& value);

	void undo(LockedStateT<HistoryEnvComponentState>& state);
	void redo(LockedStateT<HistoryEnvComponentState>& state);
	bool canUndo() const;
	bool canRedo() const;
	void removeCommands(LockedStateT<HistoryEnvComponentState>& state, const ICommandManager::TRemoveFunctor& functor);
	Collection& getHistory(LockedStateT<HistoryEnvComponentState>& state);
	Collection& getMacros();
	void moveCommandIndex(LockedStateT<HistoryEnvComponentState>& state, int index);

	void beginBatchCommand(LockedStateT<HistoryEnvComponentState>& state);
	void endBatchCommand(LockedStateT<HistoryEnvComponentState>& state, const char* description);
	void abortBatchCommand(LockedStateT<HistoryEnvComponentState>& state);

	void pushFrame(LockedStateT<HistoryEnvComponentState>& state, const CommandInstancePtr& instance);
	void popFrame(LockedStateT<HistoryEnvComponentState>& state, std::unique_lock<std::mutex>& lock);
	bool createCompoundCommand(const Collection& commandInstanceList, const char* id);
	bool deleteCompoundCommand(LockedStateT<HistoryEnvComponentState>& state, const char* id);
	void addToHistory(LockedStateT<HistoryEnvComponentState>& state, const CommandInstancePtr& instance);
	void processCommands(LockedStateT<HistoryEnvComponentState>& state);
	void flush(LockedStateT<HistoryEnvComponentState>& state);
	void threadFunc();
	bool executingCommandGroup();

	int currentIndex_;
	int* previousSelectedIndex_; // always point to active state's previous selected index
	Connection updateConnection_;

	std::thread::id ownerThreadId_;
	std::thread::id workerThreadId_;

private:
	friend class HistoryEnvComponentState;

	typedef std::unordered_map<HashedStringRef, Command*> CommandCollection;

	/*
	Guard data shared with worker thread.
	Try to cover as less code as possible by mutex, but not less than required.
	Especially avoid calling user callbacks while holding it.
	*/
	std::mutex workerMutex_;

	/*
	Assumed predicate: worker has something to do (at least one of these):
	- commandQueue_ is not empty
	- exiting_ == true
	*/
	wg_condition_variable workerWakeUp_;
	std::atomic<bool> ownerWakeUp_;

	CommandCollection commands_;
	std::vector<CompoundCommand*> macroList_;
	Collection history_;
	Collection macros_;
	std::unique_ptr<ICommandEventListener> globalEventListener_;

	bool exiting_;
	bool enableWorker_;
	CommandManager* pCommandManager_;
	std::thread workerThread_;
	BatchCommand batchCommand_;
	UndoRedoCommand undoRedoCommand_;

	IApplication* application_;
	void addBatchCommandToCompoundCommand(CompoundCommand* compoundCommand,
	                                      const CommandInstancePtr& instance);
};

void CommandManagerImpl::clearMacros()
{
	for (auto macro : macroList_)
	{
		delete macro;
	}

	macroList_.clear();
}

//==============================================================================
const char* CommandManagerImpl::getEnvComponentId() const
{
	static const char* s_componentId = "CommandManqagerComponent";
	return s_componentId;
}

//==============================================================================
std::unique_ptr<IEnvComponentState> CommandManagerImpl::createState()
{
	return std::unique_ptr<IEnvComponentState>(new HistoryEnvComponentState(*this));
}

//==============================================================================
void CommandManagerImpl::onPreEnvironmentChanged(const EnvironmentId& oldId, const EnvironmentId& newId)
{
	auto lockedState = getActiveStateT();
	flush(lockedState);
	pCommandManager_->signalPreCommandIndexChanged(currentIndex_);
	currentIndex_ = NO_SELECTION;
	previousSelectedIndex_ = nullptr;
	pCommandManager_->signalHistoryPreReset(history_);
}

//==============================================================================
void CommandManagerImpl::onPostEnvironmentChanged(const EnvironmentId& oldId, const EnvironmentId& newId)
{
	auto lockedState = getActiveStateT();
	{
		std::unique_lock<std::mutex> lock(workerMutex_);
		history_ = Collection(lockedState->history_);
	}
	pCommandManager_->signalHistoryPostReset(history_);
	currentIndex_ = lockedState->index_;
	previousSelectedIndex_ = &lockedState->previousSelectedIndex_;
	pCommandManager_->signalPostCommandIndexChanged(currentIndex_);
}

//==============================================================================
void CommandManagerImpl::init(IApplication& application)
{
	application_ = &application;
	updateConnection_ = application_->signalUpdate.connect(std::bind(&CommandManagerImpl::update, this));
	initEnvComponent();

	auto lockedState = getActiveStateT();
	previousSelectedIndex_ = &lockedState->previousSelectedIndex_;
	history_ = Collection(lockedState->history_);
	

	if (enableWorker_)
	{
		workerThread_ = std::thread(&CommandManagerImpl::threadFunc, this);
		workerThreadId_ = workerThread_.get_id();
	}
	else
	{
		workerThreadId_ = ownerThreadId_;
	}
}

//==============================================================================
void CommandManagerImpl::fini()
{
	{
		auto lockedState = getActiveStateT();
		abortBatchCommand(lockedState);
	}

	clearMacros();
	updateConnection_.disconnect();

	{
		// mutex lock is needed here to ensure new exiting_ value
		// is visible in other thread (due to memory barrier introduced by lock/unlock)
		std::unique_lock<std::mutex> lock(workerMutex_);
		exiting_ = true;
		workerWakeUp_.notify_all();
	}

	if (enableWorker_)
	{
		workerThread_.join();
	}
	finiEnvComponent();
}

//==============================================================================
void CommandManagerImpl::update()
{
	// Optimisation to early out before calling processCommands which will attempt to acquire a mutex
	if (!ownerWakeUp_)
	{
		return;
	}
	auto lockedState = getActiveStateT();
	processCommands(lockedState);

	ownerWakeUp_ = false;
}

//==============================================================================
void CommandManagerImpl::registerCommand(Command* command)
{
	auto findIt = commands_.find(command->getId());
	if (findIt != commands_.end())
	{
		TF_ASSERT(false);
		return;
	}
	commands_.insert(std::make_pair(command->getId(), command));
	command->registerCommandStatusListener(globalEventListener_.get());
	command->setCommandSystemProvider(pCommandManager_);
}

//==============================================================================
void CommandManagerImpl::deregisterCommand(const char* commandId)
{
	auto findIt = commands_.find(commandId);
	if (findIt != commands_.end())
	{
		commands_.erase(findIt);
	}
}

//==============================================================================
Command* CommandManagerImpl::findCommand(const char* commandId) const
{
	auto findIt = commands_.find(commandId);
	if (findIt != commands_.end())
	{
		return findIt->second;
	}
	return nullptr;
}

//==============================================================================
CommandInstancePtr CommandManagerImpl::makeCommand(const char* commandId) const
{
    Command* command = findCommand(commandId);
    if (command == nullptr)
    {
        NGT_ERROR_MSG("Command %s not found. \n", commandId);
        return nullptr;
    }

    auto instance = std::make_shared<CommandInstance>();
    instance->setCommandSystemProvider(pCommandManager_);
    instance->setCommandId(command->getId());
    instance->setDefinitionManager(pCommandManager_->getDefManager());
    instance->setStatus(Queued);

    return instance;
}

//==============================================================================
CommandInstancePtr CommandManagerImpl::queueCommand(LockedStateT<HistoryEnvComponentState>& state, 
                                                    const char* commandId,
                                                    ManagedObjectPtr arguments)
{
    auto instance = makeCommand(commandId);
    if (instance == nullptr)
    {
        return nullptr;
    }

    instance->setArguments(std::move(arguments));
    return queueCommand(state, instance);
}

//==============================================================================
CommandInstancePtr CommandManagerImpl::queueCommand(LockedStateT<HistoryEnvComponentState>& state,
                                                    const char* commandId, 
                                                    const ObjectHandle& arguments)
{
    auto instance = makeCommand(commandId);
    if (instance == nullptr)
    {
        return nullptr;
    }

	instance->setArguments(arguments);
	return queueCommand(state, instance);
}

//==============================================================================
CommandInstancePtr CommandManagerImpl::queueCommand(LockedStateT<HistoryEnvComponentState>& state, CommandInstancePtr instance)
{
	std::thread::id currentThreadId = std::this_thread::get_id();
	assert((currentThreadId == workerThreadId_ || currentThreadId == ownerThreadId_) &&
	       "queueCommand can only be called in command thread and owner thread. \n");
	{
		std::unique_lock<std::mutex> lock(workerMutex_);
		// Push the command onto the queue of the relevant command frame, determined by the current thread
		
		auto commandFrame = THREAD_LOCAL_GET(state->currentFrame_);
		commandFrame->commandQueue_.push_back(instance);

		// If the command is a batch command we need to push/pop to the current command frames stack queue.
		// The stack queue is used to record pending batch command groups that have not been processed yet.
		if (isBatchCommand(instance))
		{
			auto params = instance->getArguments().getBase<BatchCommandParameters>();
			TF_ASSERT(params != nullptr);
			if (params->stage_ == BatchCommandStage::Begin)
			{
				commandFrame->stackQueue_.push_back(instance);
			}
			else
			{
				if (!commandFrame->stackQueue_.empty())
				{
					commandFrame->stackQueue_.pop_back();
				}
				else
				{
					NGT_WARNING_MSG("Command frame queue is already empty\n");
				}
			}
		}
	}

	// Try to execute the queued commands instantly.
	// This will either execute the command or notify the appropriate thread to start processing
	processCommands(state);

    return instance;
}

//==============================================================================
void CommandManagerImpl::waitForInstance(LockedStateT<HistoryEnvComponentState>& state,
                                         const CommandInstancePtr& instance)
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
	std::deque<CommandInstancePtr> stackQueue;
	std::deque<CommandInstancePtr> commandQueue;
	CommandInstancePtr batchFrame = nullptr;
	{
		std::unique_lock<std::mutex> lock(workerMutex_);
		auto commandFrame = THREAD_LOCAL_GET(state->currentFrame_);
        stackQueue = commandFrame->stackQueue_;
		commandQueue = commandFrame->commandQueue_;
	}

	auto first = commandQueue.begin();
	auto last = std::find(commandQueue.begin(), commandQueue.end(), instance);
	for (auto batch = stackQueue.rbegin(); batch != stackQueue.rend(); ++batch)
	{
		auto it = std::find(first, last, *batch);
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
			processCommands(state);

			// TODO: This is piggy backing on now defunct command status messages.
			// This needs to be revised.
			pCommandManager_->fireProgressMade(*waitFor);
		}
		CommandInstancePtr parent = nullptr;
		while (it != first)
		{
			--it;
			if (std::find((*it)->children_.begin(), (*it)->children_.end(), waitFor) != (*it)->children_.end())
			{
                parent = *it;
				break;
			}
		}
		waitFor = parent;
	}
}

//==============================================================================
void CommandManagerImpl::updateSelected(LockedStateT<HistoryEnvComponentState>& state, const int& value)
{
	pCommandManager_->signalPreCommandIndexChanged(currentIndex_);

	state->index_ = value;
	state->previousSelectedIndex_ = value;
	currentIndex_ = value;

	pCommandManager_->signalPostCommandIndexChanged(currentIndex_);
}

//==============================================================================
bool CommandManagerImpl::canUndo() const
{
	if ((history_.empty() || (previousSelectedIndex_ != nullptr) && (*previousSelectedIndex_ < 0)))
	{
		return false;
	}
	return true;
}

//==============================================================================
bool CommandManagerImpl::canRedo() const
{
	if (!history_.empty() && (previousSelectedIndex_ != nullptr) &&
	    (*previousSelectedIndex_ != ((int)history_.size() - 1)))
	{
		return true;
	}
	return false;
}

void CommandManagerImpl::removeCommands(LockedStateT<HistoryEnvComponentState>& state,
                                        const ICommandManager::TRemoveFunctor& functor)
{
	flush(state);
	int currentIndexValue = 0;
	{
		std::unique_lock<std::mutex> lock(workerMutex_);

		currentIndexValue = currentIndex_;
		int commandIndex = 0;

		int prevSelectedIndexValue = state->previousSelectedIndex_;
		int prevSelectedIndex = 0;

		auto iter = history_.begin();
		while (iter != history_.end())
		{
			if (functor((*iter).value<CommandInstancePtr>()))
			{
				iter = history_.erase(iter);
				if (commandIndex <= currentIndexValue)
				{
					currentIndexValue = std::max(currentIndexValue - 1, -1);
				}

				if (prevSelectedIndex <= prevSelectedIndexValue)
				{
					prevSelectedIndexValue = std::max(prevSelectedIndexValue - 1, -1);
				}
			}
			else
			{
				++commandIndex;
				++prevSelectedIndex;
			}
		}

		state->previousSelectedIndex_ = prevSelectedIndexValue;
		currentIndex_ = currentIndexValue;
		state->index_ = currentIndexValue;
	}
}

//==============================================================================
void CommandManagerImpl::undo(LockedStateT<HistoryEnvComponentState>& state)
{
	flush(state);
	TF_ASSERT(currentIndex_ >= 0);
	TF_ASSERT(canUndo());
	moveCommandIndex(state, currentIndex_ - 1);
}

//==============================================================================
void CommandManagerImpl::redo(LockedStateT<HistoryEnvComponentState>& state)
{
	flush(state);
	TF_ASSERT(currentIndex_ < (int)history_.size());
	TF_ASSERT(canRedo());
	moveCommandIndex(state, currentIndex_ + 1);
}

//==============================================================================
Collection& CommandManagerImpl::getHistory(LockedStateT<HistoryEnvComponentState>& state)
{
	flush(state);
	return history_;
}

//==============================================================================
Collection& CommandManagerImpl::getMacros()
{
	return macros_;
}

//==============================================================================
void CommandManagerImpl::beginBatchCommand(LockedStateT<HistoryEnvComponentState>& state)
{
	auto args = ManagedObject<BatchCommandParameters>::make_unique();
	(*args)->stage_ = BatchCommandStage::Begin;
	this->queueCommand(state, getClassIdentifier<BatchCommand>(), ManagedObjectPtr(std::move(args)));
}

//==============================================================================
void CommandManagerImpl::endBatchCommand(LockedStateT<HistoryEnvComponentState>& state, const char* description)
{
	auto args = ManagedObject<BatchCommandParameters>::make_unique();
	(*args)->stage_ = BatchCommandStage::End;
	if (description != nullptr)
	{
		(*args)->description_ = description;
	}
	this->queueCommand(state, getClassIdentifier<BatchCommand>(), ManagedObjectPtr(std::move(args)));
}

//==============================================================================
void CommandManagerImpl::abortBatchCommand(LockedStateT<HistoryEnvComponentState>& state)
{
	auto args = ManagedObject<BatchCommandParameters>::make_unique();
	(*args)->stage_ = BatchCommandStage::Abort;
	this->queueCommand(state, getClassIdentifier<BatchCommand>(), ManagedObjectPtr(std::move(args)));
}

//==============================================================================
void CommandManagerImpl::pushFrame(LockedStateT<HistoryEnvComponentState>& state, const CommandInstancePtr& instance)
{
	TF_ASSERT(instance != nullptr);
	TF_ASSERT(!state->commandFrames_.empty());
	auto currentFrame = state->commandFrames_.back().get();
	TF_ASSERT(!currentFrame->commandStack_.empty());
	auto parentInstance = currentFrame->commandStack_.back();

	instance->setStatus(Running);

	state->commandFrames_.emplace_back(new CommandFrame(instance));

	if (isBatchCommand(instance))
	{
		auto params = instance->getArguments().getBase<BatchCommandParameters>();
		TF_ASSERT(params != nullptr);
		if (params->stage_ != BatchCommandStage::Begin)
		{
			return;
		}

		// The command stack represents BatchCommand groups within a frame.
		// On BeginBatchCommand we need to add a group to the stack
		currentFrame->commandStack_.push_back(instance);
	}

	if (parentInstance != nullptr)
	{
		parentInstance->children_.push_back(instance);
	}
}

//==============================================================================
void CommandManagerImpl::popFrame(LockedStateT<HistoryEnvComponentState>& state, std::unique_lock<std::mutex>& lock)
{
	TF_ASSERT(!state->commandFrames_.empty());
	auto currentFrame = state->commandFrames_.back().get();
	TF_ASSERT(!currentFrame->commandStack_.empty());
	auto instance = currentFrame->commandStack_.back();
	TF_ASSERT(instance != nullptr);
	currentFrame->commandStack_.pop_back();
	TF_ASSERT(currentFrame->commandStack_.empty() && currentFrame->commandQueue_.empty());
	state->commandFrames_.pop_back();
	TF_ASSERT(!state->commandFrames_.empty());
	currentFrame = state->commandFrames_.back().get();
	auto parentInstance = currentFrame->commandStack_.back();

	const bool isBatch = isBatchCommand(instance);
	if (isBatch)
	{
		auto params = instance->getArguments().getBase<BatchCommandParameters>();
		TF_ASSERT(params != nullptr);

		// Set the arguments to nullptr since there is no need to serialize BatchCommand arguments
		auto stage = params->stage_;

		if (stage == BatchCommandStage::Begin)
		{
			return;
		}

		// If we are ending or aborting a BatchCommand, we need to remove its group from the stack
		if (parentInstance != nullptr)
		{
			currentFrame->commandStack_.pop_back();
			auto errorCode = instance->errorCode_;
			instance = parentInstance;
			instance->errorCode_ = errorCode;
			instance->setArguments(ManagedObject<BatchCommandParameters>::make_unique(*params));
			parentInstance = currentFrame->commandStack_.back();
		}
		else
		{
			NGT_WARNING_MSG("Removing from an empty batch command group\n");
		}
	}

	const auto errorCode = instance->getErrorCode();
	if (!wgt::isCommandSuccess(errorCode))
	{
		bool topLevelBatch = isBatch && state->commandFrames_.size() == 1 && currentFrame->commandStack_.size() == 1;

		if (topLevelBatch)
		{
			state->abortingBatchCommand_ = true;
		}

		if (instance->getCommand()->canUndo(instance->getArguments()))
		{
			instance->consolidateUndoRedoData(nullptr);
			lock.unlock(); // release lock while running commands
			instance->undo();
			lock.lock();
		}

		if (isBatch)
		{
			pCommandManager_->notifyCancelMultiCommand();

			if (topLevelBatch)
			{
				state->abortingBatchCommand_ = false;
			}
		}
	}
	else
	{
		if (instance->getCommand()->canUndo(instance->getArguments()))
		{
			instance->consolidateUndoRedoData(parentInstance.get());
			if (parentInstance == nullptr)
			{
				// If we are popping a root command frame, finalise the undo/redo stream and add the command to history
				addToHistory(state, instance);
			}
			else if (isBatchCommand(parentInstance))
			{
				parentInstance->consolidateChildren();
			}
		}
		if (parentInstance == nullptr)
		{
			if (isBatch)
			{
				instance->consolidateUndoRedoData(parentInstance.get());
				instance->consolidateChildren();
				pCommandManager_->notifyCompleteMultiCommand();
			}
		}
	}

	instance->setStatus(Complete);
}

//==============================================================================
void CommandManagerImpl::addToHistory(LockedStateT<HistoryEnvComponentState>& state, const CommandInstancePtr& instance)
{
	if (instance.get()->getCommand()->canUndo(instance.get()->getArguments()))
	{
		state->pendingHistory_.push_back(instance);
	}
}

//==============================================================================
void CommandManagerImpl::moveCommandIndex(LockedStateT<HistoryEnvComponentState>& state, int index)
{
	static const char* id = typeid(UndoRedoCommand).name();
	if (currentIndex_ == index)
	{
		return;
	}
	state->previousSelectedIndex_ = currentIndex_;
	pCommandManager_->signalPreCommandIndexChanged(state->previousSelectedIndex_);
	currentIndex_ = index;
	auto args = ManagedObject<UndoRedoCommandArgument>::make_unique(currentIndex_);
	auto instance = queueCommand(state, id, ManagedObjectPtr(std::move(args)));
	waitForInstance(state, instance);
	currentIndex_ = state->previousSelectedIndex_;
	state->index_ = currentIndex_;
	pCommandManager_->signalPostCommandIndexChanged(state->index_);
}

//==============================================================================
void CommandManagerImpl::processCommands(LockedStateT<HistoryEnvComponentState>& state)
{
	{
		std::thread::id currentThreadId = std::this_thread::get_id();

		std::unique_lock<std::mutex> lock(workerMutex_);
		for (;;)
		{
			auto commandFrame = state->commandFrames_.back().get();
			if (commandFrame->commandQueue_.empty())
			{
				break;
			}

            auto job = commandFrame->commandQueue_.front();
            auto threadAffinity = job->getCommand()->threadAffinity();
			if (threadAffinity == CommandThreadAffinity::UI_THREAD && currentThreadId != ownerThreadId_)
			{
				// The next command in the queue needs to be run on the UI thread.
				// Notify the owner thread
				ownerWakeUp_ = true;
				break;
			}
			else if (threadAffinity == CommandThreadAffinity::COMMAND_THREAD && currentThreadId != workerThreadId_)
			{
				// The next command in the queue needs to be run on the command thread.
				// Notify the worker thread
				workerWakeUp_.notify_all();
				break;
			}

			commandFrame->commandQueue_.pop_front();

			if (strcmp(job->getCommandId(), typeid(UndoRedoCommand).name()) == 0)
			{
				// execute undo/redo
				lock.unlock(); // release lock while running commands
				job->setStatus(Running);
				job->execute();
				job->setStatus(Complete);
				lock.lock();
			}
			else
			{
				// Push a command frame for this job
				auto previousFrame = THREAD_LOCAL_GET(state->currentFrame_);
				pushFrame(state, job);
				auto currentFrame = state->commandFrames_.back().get();
				THREAD_LOCAL_SET(state->currentFrame_, currentFrame);

				lock.unlock(); // release lock while running commands
				job->execute();
				lock.lock();

				// Spin and process commands until all sub commands for this frame have been executed
				while (!currentFrame->commandQueue_.empty() || state->commandFrames_.back().get() != currentFrame)
				{
					lock.unlock();
					processCommands(state);
					lock.lock();
				}

				// Pop the command frame
				THREAD_LOCAL_SET(state->currentFrame_, previousFrame);
				popFrame(state, lock);
			}
		}

		if (currentThreadId != ownerThreadId_)
		{
			if (!state->pendingHistory_.empty())
			{
				ownerWakeUp_ = true;
			}
			return;
		}

		if (state->pendingHistory_.empty())
		{
			return;
		}

		if (static_cast<int>(history_.size()) > currentIndex_ + 1)
		{
			// erase all history after the current index as we have pending
			// history that will make this invalid
			auto start = history_.find(currentIndex_ + 1);
			history_.erase(start, history_.end());
		}

		while (!state->pendingHistory_.empty())
		{
			auto entry = state->pendingHistory_.front();
			history_.insertValue(history_.size(), entry);
			state->pendingHistory_.pop_front();
		}
	}
	updateSelected(state, static_cast<int>(history_.size() - 1));
}

//==============================================================================
void CommandManagerImpl::flush(LockedStateT<HistoryEnvComponentState>& state)
{
	TF_ASSERT(std::this_thread::get_id() == ownerThreadId_);

	std::unique_lock<std::mutex> lock(workerMutex_);

	while (state->commandFrames_.size() > 1 || !state->commandFrames_.front()->commandQueue_.empty() ||
	       !state->pendingHistory_.empty())
	{
		lock.unlock();
		processCommands(state);
		lock.lock();
	}
}

//==============================================================================
/*static */ void CommandManagerImpl::threadFunc()
{
	std::unique_lock<std::mutex> lock(workerMutex_);

	while (exiting_ == false)
	{
		workerWakeUp_.wait(lock, [this] {
			auto lockedState = getActiveStateT();
			auto& commandFrame = *lockedState->commandFrames_.back();
			return !commandFrame.commandQueue_.empty() || exiting_;
		});

		// execute commands
		lock.unlock();
		auto lockedState = getActiveStateT();
		processCommands(lockedState);
		lock.lock();
	}
}

//==============================================================================
bool CommandManagerImpl::createCompoundCommand(const Collection& commandInstanceList, const char* id)
{
	// create compound command
	std::vector<size_t> commandIndices;
	size_t count = commandInstanceList.size();
	{
		std::unique_lock<std::mutex> lock(workerMutex_);
		for (size_t i = 0; i < count; i++)
		{
			const Variant& variant = commandInstanceList[i];
			auto&& findIt = std::find(history_.begin(), history_.end(), variant);
			if (findIt == history_.end())
			{
				continue;
			}

			Variant positionVariant = findIt.key();
			size_t position = positionVariant.cast<size_t>();
			commandIndices.push_back(position);
		}
	}

	if (commandIndices.empty())
	{
		NGT_ERROR_MSG("Failed to create macros: no command history. \n");
		return false;
	}
	auto macro = new CompoundCommand(id);
	pCommandManager_->registerCommand(macro);
	std::sort(commandIndices.begin(), commandIndices.end());
	auto indexIt = commandIndices.begin();
	auto indexItEnd = commandIndices.end();
	for (; indexIt != indexItEnd; ++indexIt)
	{
		const CommandInstancePtr& instance = history_[*indexIt].value<CommandInstancePtr>();
		if (instance->hasChildren())
		{
			addBatchCommandToCompoundCommand(macro, instance);
		}
		else
		{
			macro->addCommand(instance->getCommandId(), instance->getArguments());
		}
	}
	macros_.insertValue(macros_.size(), macro);
	return true;
}

//==============================================================================
void CommandManagerImpl::addBatchCommandToCompoundCommand(CompoundCommand* compoundCommand,
                                                          const CommandInstancePtr& instance)
{
	TF_ASSERT(!instance->children_.empty());
	for (auto& child : instance->children_)
	{
		if (child->hasChildren())
		{
			addBatchCommandToCompoundCommand(compoundCommand, child);
		}
		else
		{
			if (strcmp(child->getCommandId(), typeid(BatchCommand).name()) != 0)
			{
				compoundCommand->addCommand(child->getCommandId(), child->getArguments());
			}
		}
	}
}

//==============================================================================
bool CommandManagerImpl::deleteCompoundCommand(LockedStateT<HistoryEnvComponentState>& state, const char* id)
{
	flush(state); // make sure commands stack do not hold a ref to this command
	bool bSuccess = false;
	CompoundCommand* compoundCommand = static_cast<CompoundCommand*>(findCommand(id));
	if (compoundCommand != nullptr)
	{
		for (auto iter = macros_.begin(); iter != macros_.end(); ++iter)
		{
			auto v = iter.value();
			auto macro = v.cast<CompoundCommand*>();
			bool isOk = (strcmp(id, macro->getId()) == 0);
			if (isOk)
			{
				deregisterCommand(id);
				macros_.erase(iter);
				delete macro;
				macro = nullptr;
				bSuccess = true;
				break;
			}
		}
	}
	return bSuccess;
}

bool CommandManagerImpl::executingCommandGroup()
{
	std::unique_lock<std::mutex> lock(workerMutex_);
	auto state = getActiveStateT();

	if (state->abortingBatchCommand_)
	{
		return true;
	}

	auto& frames = state->commandFrames_;
	auto count = frames.size();

	if (count == 2)
	{
		auto firstFrame = frames.front().get();
		return firstFrame->commandStack_.size() > 1;
	}

	return count > 2;
}

void HistoryEnvComponentState::resetState()
{
	index_ = NO_SELECTION;
	previousSelectedIndex_ = NO_SELECTION;
	pendingHistory_.clear();
	history_.clear();
	cmdMgrImpl_.pCommandManager_->signalPreCommandIndexChanged(cmdMgrImpl_.currentIndex_);
	cmdMgrImpl_.currentIndex_ = NO_SELECTION;
	cmdMgrImpl_.previousSelectedIndex_ = nullptr;
	cmdMgrImpl_.pCommandManager_->signalHistoryPreReset(cmdMgrImpl_.history_);
	{
		std::unique_lock<std::mutex> lock(cmdMgrImpl_.workerMutex_);
		cmdMgrImpl_.history_ = Collection(history_);
	}
	cmdMgrImpl_.pCommandManager_->signalHistoryPostReset(cmdMgrImpl_.history_);
	cmdMgrImpl_.currentIndex_ = index_;
	cmdMgrImpl_.previousSelectedIndex_ = &previousSelectedIndex_;
	cmdMgrImpl_.pCommandManager_->signalPostCommandIndexChanged(cmdMgrImpl_.currentIndex_);
	currentFrame_ = nullptr;
	commandFrames_.clear();
	commandFrames_.emplace_back(new CommandFrame(nullptr));
	THREAD_LOCAL_SET(currentFrame_, commandFrames_.back().get());
}

void HistoryEnvComponentState::saveState(IDataStream& stream)
{
}

void HistoryEnvComponentState::loadState(IDataStream& stream)
{
}

}

//==============================================================================
CommandManager::CommandManager( IEnvManager & envManager)
    : pImpl_( new CommandManagerImpl( this, envManager) )
	, fileSystem_(nullptr)
{
}

//==============================================================================
CommandManager::~CommandManager()
{
}

//==============================================================================
void CommandManager::init(IApplication& application, IDefinitionManager& defManager )
{
	defManager_ = &defManager;
	pImpl_->init(application);
}

//==============================================================================
void CommandManager::fini()
{
	pImpl_->fini();
}

//==============================================================================
void CommandManager::registerCommand(Command* command)
{
	pImpl_->registerCommand(command);
}

//==============================================================================
void CommandManager::deregisterCommand(const char* commandId)
{
	pImpl_->deregisterCommand(commandId);
}

//==============================================================================
Command* CommandManager::findCommand(const char* commandId) const
{
	return pImpl_->findCommand(commandId);
}
//==============================================================================
CommandInstancePtr CommandManager::queueCommand(const char* commandId)
{
    auto lockedState = pImpl_->getActiveStateT();
    return pImpl_->queueCommand(lockedState, commandId, ObjectHandle());
}

//==============================================================================
CommandInstancePtr CommandManager::queueCommand(const char* commandId, const ObjectHandle& arguments)
{
	auto lockedState = pImpl_->getActiveStateT();
	return pImpl_->queueCommand(lockedState, commandId, arguments);
}

//==============================================================================
CommandInstancePtr CommandManager::queueCommand(const char* commandId, ManagedObjectPtr arguments)
{
    auto lockedState = pImpl_->getActiveStateT();
    return pImpl_->queueCommand(lockedState, commandId, std::move(arguments));
}

//==============================================================================
void CommandManager::waitForInstance(const CommandInstancePtr& instance)
{
	auto lockedState = pImpl_->getActiveStateT();
	pImpl_->waitForInstance(lockedState, instance);
}

//==============================================================================
void CommandManager::registerCommandStatusListener(ICommandEventListener* listener)
{
	eventListenerCollection_.push_back(listener);
}

//==============================================================================
void CommandManager::deregisterCommandStatusListener(ICommandEventListener* listener)
{
	auto it = std::find(eventListenerCollection_.begin(), eventListenerCollection_.end(), listener);
	if (it != eventListenerCollection_.end())
	{
		eventListenerCollection_.erase(it);
	}
}

//==============================================================================
void CommandManager::multiCommandStatusChanged(ICommandEventListener::MultiCommandStatus status)
{
	for (auto& it : eventListenerCollection_)
	{
		it->multiCommandStatusChanged(status);
	}
}

//==============================================================================
void CommandManager::fireCommandStatusChanged(const CommandInstance& command) const
{
	EventListenerCollection::const_iterator it = eventListenerCollection_.begin();
	EventListenerCollection::const_iterator itEnd = eventListenerCollection_.end();
	for (; it != itEnd; ++it)
	{
		(*it)->statusChanged(command);
	}
}

//==============================================================================
void CommandManager::fireProgressMade(const CommandInstance& command) const
{
	EventListenerCollection::const_iterator it = eventListenerCollection_.begin();
	EventListenerCollection::const_iterator itEnd = eventListenerCollection_.end();
	for (; it != itEnd; ++it)
	{
		(*it)->progressMade(command);
	}
}

void CommandManager::fireCommandExecuted(const CommandInstance& command, CommandOperation operation) const
{
	EventListenerCollection::const_iterator it = eventListenerCollection_.begin();
	EventListenerCollection::const_iterator itEnd = eventListenerCollection_.end();
	for (; it != itEnd; ++it)
	{
		(*it)->commandExecuted(command, operation);
	}
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

void CommandManager::removeCommands(const TRemoveFunctor& functor)
{
	auto lockedState = pImpl_->getActiveStateT();
	pImpl_->removeCommands(lockedState, functor);
}

//==============================================================================
void CommandManager::undo()
{
	auto lockedState = pImpl_->getActiveStateT();
	pImpl_->undo(lockedState);
}

//==============================================================================
void CommandManager::redo()
{
	auto lockedState = pImpl_->getActiveStateT();
	pImpl_->redo(lockedState);
}

//==============================================================================
const Collection& CommandManager::getHistory() const
{
	auto lockedState = pImpl_->getActiveStateT();
	return pImpl_->getHistory(lockedState);
}

//==============================================================================
Collection& CommandManager::getMacros() const
{
	return pImpl_->getMacros();
}

//==============================================================================
const int CommandManager::commandIndex() const
{
	return pImpl_->currentIndex_;
}

//==============================================================================
void CommandManager::moveCommandIndex(int newIndex)
{
	auto lockedState = pImpl_->getActiveStateT();
	pImpl_->moveCommandIndex(lockedState, newIndex);
}

//==============================================================================
IDefinitionManager& CommandManager::getDefManager() const
{
	return *defManager_;
}

//==============================================================================
ISelectionContext& CommandManager::selectionContext()
{
	return selectionContext_;
}

//==============================================================================
std::thread::id CommandManager::ownerThreadId() /* override */
{
	return pImpl_->ownerThreadId_;
}

bool CommandManager::executingCommandGroup()
{
	return pImpl_->executingCommandGroup();
}

//==============================================================================
void CommandManager::beginBatchCommand()
{
	auto lockedState = pImpl_->getActiveStateT();
	notifyBeginMultiCommand();
	pImpl_->beginBatchCommand(lockedState);
}

//==============================================================================
void CommandManager::endBatchCommand(const char* description)
{
	auto lockedState = pImpl_->getActiveStateT();
	pImpl_->endBatchCommand(lockedState, description);
}

//==============================================================================
void CommandManager::abortBatchCommand()
{
	auto lockedState = pImpl_->getActiveStateT();
	pImpl_->abortBatchCommand(lockedState);
}

//==============================================================================
void CommandManager::notifyBeginMultiCommand()
{
	multiCommandStatusChanged(ICommandEventListener::MultiCommandStatus_Begin);
}

//==============================================================================
void CommandManager::notifyCompleteMultiCommand()
{
	multiCommandStatusChanged(ICommandEventListener::MultiCommandStatus_Complete);
}

//==============================================================================
void CommandManager::notifyCancelMultiCommand()
{
	multiCommandStatusChanged(ICommandEventListener::MultiCommandStatus_Cancel);
}

//==============================================================================
void CommandManager::notifyHandleCommandQueued(const char* commandId)
{
	for (auto& it : eventListenerCollection_)
	{
		it->handleCommandQueued(commandId);
	}
}

//==============================================================================
void CommandManager::notifyNonBlockingProcessExecution(const char* commandId)
{
	for (auto& it : eventListenerCollection_)
	{
		it->onNonBlockingProcessExecution(commandId);
	}
}

//==============================================================================
bool CommandManager::createMacro(const Collection& commandInstanceList, const char* id)
{
	auto lockedState = pImpl_->getActiveStateT();
	static int index = 1;
	const char* defaultName = "Macro";

	std::string macroName("");
	if (id != nullptr)
	{
		macroName = id;
	}
	if (findCommand(macroName.c_str()) != nullptr)
	{
		NGT_ERROR_MSG("Failed to create macros: macro name %s already exists. \n", macroName.c_str());
		return false;
	}
	if (macroName.empty())
	{
		char buffer[260];
		do
		{
			snprintf(buffer, 260, "%d", index);
			macroName = defaultName;
			macroName += buffer;
			index++;
		} while (findCommand(macroName.c_str()) != nullptr);
	}
	return pImpl_->createCompoundCommand(commandInstanceList, macroName.c_str());
}

//==============================================================================
bool CommandManager::deleteMacroByName(const char* id)
{
	auto lockedState = pImpl_->getActiveStateT();
	return pImpl_->deleteCompoundCommand(lockedState, id);
}

//==============================================================================
void CommandManager::addToHistory(const CommandInstancePtr& instance)
{
	auto lockedState = pImpl_->getActiveStateT();
	pImpl_->addToHistory(lockedState, instance);
}

//==============================================================================
bool CommandManager::undoRedo(int& desiredIndex)
{
	auto lockedState = pImpl_->getActiveStateT();
	auto& history = pImpl_->getHistory(lockedState);
	const int& size = static_cast<int>(history.size());
	if (size == 0)
	{
		return false;
	}
	if ((lockedState->previousSelectedIndex_ == desiredIndex) || (desiredIndex >= size))
	{
		return false;
	}
	while (lockedState->previousSelectedIndex_ != pImpl_->currentIndex_)
	{
		if (lockedState->previousSelectedIndex_ > pImpl_->currentIndex_)
		{
			int i = lockedState->previousSelectedIndex_;
			CommandInstancePtr job = history[i].value<CommandInstancePtr>();
			if(!job->undo())
			{
				return false;
			}
			lockedState->previousSelectedIndex_--;
		}
		else
		{
			int i = lockedState->previousSelectedIndex_;
			CommandInstancePtr job = history[i + 1].value<CommandInstancePtr>();
			if(!job->redo())
			{
				return false;
			}
			lockedState->previousSelectedIndex_++;
		}
	}
	return true;
}
} // end namespace wgt
