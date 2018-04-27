#pragma once

#include "core_logging_system/interfaces/i_logger.hpp"
#include "core_dependency_system/context_callback_helper.hpp"
#include "core_dependency_system/depends.hpp"
#include "core_dependency_system/i_interface.hpp"
#include "core_logging/log_level.hpp"
#include "core_common/signal.hpp"
#include <fstream>
#include <sstream>
#include <Windows.h>

namespace wgt
{
class ILoggingSystem;

class FileLogger 
	: public ILogger
	, ContextCallBackHelper
{
public:
	FileLogger();
	~FileLogger();

	/** Logs a new message through the logging system if possible, else directly to file */
	void log(LogLevel level, const char* message);

	/** Disables the logging queue and disallows logging to new files */
	void onApplicationShutdown();

	/** Logs a fatal exception and logs to file*/
	static DWORD onFatalException(FileLogger* logger, DWORD exceptionCode, struct _EXCEPTION_POINTERS * ep);

private:
	void createFile(int id = 0);
	void out(ILogMessage* message) override;
	void logToQueue(LogLevel level, const std::string& message);
	void logQueueToFile();
	void logToFile(const char* message);
	void logToFile(LogLevel level, const char* message);
	void flushQueue();
	void flushFile();
	void closeFile();
	bool isFileMaxSize() const;
	void switchToNewFile();
	void removeOldFiles();
	void registerLogger(ILoggingSystem& loggingSystem, bool shouldRegister);

	/** @see https://www.microsoft.com/msj/0197/exception/exception.aspx for more information */
	const char* getLevelString(LogLevel level) const;
	const char* exceptionCodeAsString(DWORD code) const;
	const char* accessViolationTypeAsString(ULONG_PTR type) const;
	DWORD exceptionHandler(DWORD exceptionCode, struct _EXCEPTION_POINTERS * ep);

	std::string filePath_;
	std::string fileFolder_;
	std::string fileName_;
	std::string fileTime_;
	std::unique_ptr<std::fstream> file_;
	int fileId_ = 0;

	std::stringstream queue_;
	std::string lastMessage_;
	size_t queueSize_ = 0;
	unsigned int duplicateCount_ = 0;
	
	bool registered_ = false;
	bool enabled_ = false;
	bool shutdown_ = false;
	ILoggingSystem* loggingSystem_ = nullptr;
};

} // end namespace wgt