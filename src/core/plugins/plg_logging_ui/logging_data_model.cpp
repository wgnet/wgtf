#include "logging_data_model.hpp"

#include "core_common/assert.hpp"
#include "core_logging_system/log_message.hpp"
#include "core_string_utils/string_utils.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_generic_plugin/interfaces/i_application.hpp"
#include "core_common/connection_holder.hpp"

#include <mutex>

namespace wgt
{
//------------------------------------------------------------------------------
// Implementation
//------------------------------------------------------------------------------

namespace
{
const size_t maximum_entries = 2048;
};

struct LoggingDataModel::Implementation
{
	Implementation(LoggingDataModel& self);

	void flushBuffer();

	LoggingDataModel& self_;
	std::vector<std::string> entries_;
	std::string text_;
	std::string pendingBuffer_;
	bool pendingChange_;
	std::mutex bufferMutex_;
	Connection flushConnection_;
};

LoggingDataModel::Implementation::Implementation(LoggingDataModel& self) : self_(self), pendingChange_(false)
{
	entries_.reserve(maximum_entries);
}

void LoggingDataModel::Implementation::flushBuffer()
{
	std::lock_guard<std::mutex> guard(bufferMutex_);

	if (!pendingChange_)
	{
		return;
	}

	self_.ILoggingModel::setText(pendingBuffer_);
	pendingBuffer_.clear();
	pendingChange_ = false;
}

//------------------------------------------------------------------------------
// Logger
//------------------------------------------------------------------------------

Logger::Logger(ObjectHandleT<LoggingDataModel> loggingModel) : loggingModel_(loggingModel)
{
}

void Logger::out(ILogMessage* message)
{
	if (message != nullptr && loggingModel_.get() != nullptr && message->getLevel() != LOG_DEBUG)
	{
		loggingModel_->appendText(message->getAsHtml());
	}
}

//------------------------------------------------------------------------------
// Data Model
// Data passed to the QML panel to be used by the control(s).
//------------------------------------------------------------------------------

LoggingDataModel::LoggingDataModel(IComponentContext& context) : impl_(new Implementation(*this))
{
	IApplication* application = context.queryInterface<IApplication>();
	TF_ASSERT(application);

	std::function<void()> callback = std::bind(&LoggingDataModel::Implementation::flushBuffer, impl_.get());
	impl_->flushConnection_ = application->signalUpdate.connect(callback);
}

LoggingDataModel::~LoggingDataModel()
{
	impl_.reset();
}

const std::string& LoggingDataModel::getText() const
{
	return impl_->text_;
}

void LoggingDataModel::putText(const std::string& text)
{
	impl_->text_ = text;
}

void LoggingDataModel::setText(const std::string& text)
{
	std::lock_guard<std::mutex> guard(impl_->bufferMutex_);
	impl_->pendingBuffer_ = text;
	impl_->pendingChange_ = true;
}

void LoggingDataModel::appendText(const std::string& text)
{
	if (impl_->entries_.size() == maximum_entries)
	{
		impl_->entries_.erase(impl_->entries_.begin());
	}

	impl_->entries_.emplace_back(text);
	setText(StringUtils::join(impl_->entries_, ""));
}

void LoggingDataModel::clear()
{
	impl_->entries_.clear();
	setText("");
}

} // end namespace wgt
