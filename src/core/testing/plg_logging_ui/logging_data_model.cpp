#include "logging_data_model.hpp"

#include "core_logging_system/log_message.hpp"

#include <sstream>

namespace wgt
{

//------------------------------------------------------------------------------
// Implementation
//------------------------------------------------------------------------------

struct LoggingDataModel::Implementation
{
	Implementation( LoggingDataModel& self );

	LoggingDataModel& self_;
	std::string text_;
};

LoggingDataModel::Implementation::Implementation( LoggingDataModel& self )
	: self_( self )
{
}


//------------------------------------------------------------------------------
// Logger
//------------------------------------------------------------------------------

Logger::Logger( std::shared_ptr< ILoggingModel > loggingModel )
	: loggingModel_( loggingModel )
{
}

void Logger::out( LogMessage* message )
{
	if ( message != nullptr && loggingModel_.get() != nullptr )
	{
		std::stringstream ss;

		ss << "[" << message->getLevelString() << "] "
			<< message->str()
			<< std::endl;

		loggingModel_->appendText( ss.str() );
	}
}


//------------------------------------------------------------------------------
// Data Model
// Data passed to the QML panel to be used by the control(s).
//------------------------------------------------------------------------------

LoggingDataModel::LoggingDataModel()
	: impl_( new Implementation( *this ) )
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

void LoggingDataModel::putText( const std::string& text )
{
	impl_->text_ = text;
}

void LoggingDataModel::appendText( const std::string& text )
{
	impl_->text_ += text;
	setText(impl_->text_);
}

void LoggingDataModel::clear()
{
	setText("");
}

} // end namespace wgt
