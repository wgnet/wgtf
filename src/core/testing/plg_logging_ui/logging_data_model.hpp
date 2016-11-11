#pragma once
#ifndef LOGGING_DATA_MODEL_HPP
#define LOGGING_DATA_MODEL_HPP

#include "core_logging_system/interfaces/i_logging_model.hpp"
#include "core_logging_system/interfaces/i_logger.hpp"
#include "core_reflection/reflected_object.hpp"

class LogMessage;

namespace wgt
{
class LoggingDataModel : public ILoggingModel
{
public:
	LoggingDataModel();
	~LoggingDataModel();

	// ILoggingModel
	virtual const std::string& getText() const override;
	virtual void putText(const std::string& text) override;
	virtual void appendText(const std::string& text) override;
	virtual void clear() override;

private:
	LoggingDataModel(const LoggingDataModel&);
	LoggingDataModel& operator=(const LoggingDataModel&);

	struct Implementation;
	std::unique_ptr<Implementation> impl_;
};

class Logger : public ILogger
{
public:
	Logger(std::shared_ptr<ILoggingModel> loggingModel);

private:
	// ILogger
	virtual void out(LogMessage* message) override;

	std::shared_ptr<ILoggingModel> loggingModel_;
};

#endif // LOGGING_DATA_MODEL_HPP
} // end namespace wgt