#include "logging_data_model.hpp"

#include "core_logging_system/log_message.hpp"
#include "core_string_utils/string_utils.hpp"

#include <sstream>
#include <regex>

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

	LoggingDataModel& self_;
	std::vector<std::string> entries_;
	std::string text_;
};

LoggingDataModel::Implementation::Implementation(LoggingDataModel& self) : self_(self)
{
	entries_.reserve(maximum_entries);
}

//------------------------------------------------------------------------------
// Logger
//------------------------------------------------------------------------------

Logger::Logger(std::shared_ptr<ILoggingModel> loggingModel) : loggingModel_(loggingModel)
{
}

void Logger::out(LogMessage* message)
{
	if (message != nullptr && loggingModel_.get() != nullptr)
	{
		std::string remark = message->str();
		// Trim tailing new line each remark is in a <div>
		if (!remark.empty() && *remark.rbegin() == '\n')
		{
			remark.erase(--remark.end());
		}
		// Remove carriage returns
		remark = regex_replace(remark, std::regex("\r"), std::string(" "));
		// Replace double quotes with singles to allow assigning innerHTML in javascript
		remark = regex_replace(remark, std::regex("\""), std::string("'"));
		// Replace new lines with breaks
		remark = regex_replace(remark, std::regex("\n"), std::string("<br>"));

		// Rather than modify all lines of code that use <color> tags we replace them here with the supported <font>
		// tags
		remark = regex_replace(remark, std::regex("<color\\svalue=['\"](.*)['\"]>"), std::string("<font color='$1'>"));
		remark = regex_replace(remark, std::regex("</color>"), std::string("</font>"));

		// In order to keep formatted whitespace we wrap the line in a div
		std::stringstream ss;
		ss << "<div style='white-space: pre;'>";

		switch (message->getLevel())
		{
		case LOG_FATAL:
		case LOG_ERROR:
			ss << "<font class='error'>(" << message->getLevelString() << ") ";
			break;
		case LOG_WARNING:
			ss << "<font class='warning'>(" << message->getLevelString() << ") ";
			break;
		case LOG_ALERT:
			ss << "<font class='alert'>(" << message->getLevelString() << ") ";
			break;
		case LOG_INFO:
			ss << "<font class='info'>";
			break;
		case LOG_DEBUG:
		default:
			ss << "<font>";
		}
		ss << remark << "</font></div>";

		loggingModel_->appendText(ss.str());
	}
}

//------------------------------------------------------------------------------
// Data Model
// Data passed to the QML panel to be used by the control(s).
//------------------------------------------------------------------------------

LoggingDataModel::LoggingDataModel() : impl_(new Implementation(*this))
{
}

LoggingDataModel::~LoggingDataModel()
{
	if (impl_ != nullptr)
	{
		impl_.reset();
	}
}

const std::string& LoggingDataModel::getText() const
{
	return impl_->text_;
}

void LoggingDataModel::putText(const std::string& text)
{
	impl_->text_ = text;
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
