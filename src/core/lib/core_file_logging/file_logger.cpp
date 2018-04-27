#include "file_logger.hpp"
#include "core_logging_system/log_message.hpp"
#include "core_logging_system/interfaces/i_logging_system.hpp"
#include "core_common/platform_dll.hpp"
#include "core_common/platform_path.hpp"
#include "core_string_utils/string_utils.hpp"
#include "core_string_utils/file_path.hpp"
#include "core_logging/logging.hpp"
#include "wg_memory/allocator.hpp"
#include <ctime>
#include <psapi.h>

namespace wgt
{
namespace utils
{
const int MaxQueue = 4096;
const int MaxFileMb = 20;
const int MaxFileBytes = MaxFileMb * 1000000;
const int MaxFileCount = 5;
const char* FileExtension = ".log";
bool HandlingException = false;
}

FileLogger::FileLogger()
{
	createFile();
	registerCallback(
		[this](ILoggingSystem& loggingSystem) { registerLogger(loggingSystem, true); },
		[this](ILoggingSystem& loggingSystem) { registerLogger(loggingSystem, false); });
}

FileLogger::~FileLogger()
{
	flushQueue();
	logQueueToFile();
	closeFile();
}

void FileLogger::createFile(int id)
{
	if(id == 0)
	{
		wchar_t wModule[MAX_PATH];
		::GetModuleFileNameW(NULL, wModule, MAX_PATH);
		const auto module = StringUtils::to_string(wModule);
		fileName_ += FilePath::getFileNoExtension(module);

		// Store in the app data path if possible, else use the exe path
		std::string appDataPath;
		if (GetWGAppDataPath(appDataPath))
		{
			fileFolder_ = appDataPath;
		}
		else
		{
			fileFolder_ = FilePath::getFolder(module);
		}

		// Create the folder if it doesn't exist
		if (!PathFileExists(StringUtils::to_wstring(fileFolder_).c_str()))
		{
			CreateDirectoryPath(fileFolder_.c_str());
		}

		time_t t = time(0);
		const tm* now = localtime(&t);
		std::ostringstream timeStream;
		timeStream << "-"
			<< (now->tm_year + 1900) << "-"
			<< (now->tm_mon + 1) << "-"
			<< now->tm_mday << "-"
			<< now->tm_hour << "-"
			<< now->tm_min << "-"
			<< now->tm_sec << "-";
		fileTime_ = timeStream.str();
	}

	const auto file(fileName_ + fileTime_ + std::to_string(fileId_) + utils::FileExtension);
	filePath_ = fileFolder_ + file;
	file_ = std::make_unique<std::fstream>(filePath_.c_str(), std::ios_base::out);

	if(!file_->is_open())
	{
		enabled_ = false;
		NGT_ERROR_MSG("Could not create log at path %s", filePath_.c_str());
		file_.reset();
	}
	else
	{
		enabled_ = true;
		removeOldFiles();
		logToFile(("(DEBUG)   Opened " + file).c_str());
	}
}

void FileLogger::closeFile()
{
	if (file_ && file_->is_open())
	{
		flushFile();
		file_->close();
		enabled_ = false;
	}
}

void FileLogger::registerLogger(ILoggingSystem& loggingSystem, bool shouldRegister)
{
	if(shouldRegister)
	{
		if(!registered_)
		{
			registered_ = loggingSystem.registerLogger(this);
		}
		loggingSystem_ = &loggingSystem;
	}
	else
	{
		if(registered_)
		{
			loggingSystem.unregisterLogger(this);
		}
		registered_ = false;
		loggingSystem_ = nullptr;
	}
}

void FileLogger::removeOldFiles()
{
	std::vector<std::pair<uint64_t, std::string>> files;

	std::string directory(fileFolder_);
	std::replace(directory.begin(), directory.end(), 
		FilePath::kDirectorySeparator, FilePath::kAltDirectorySeparator);
	directory += "*";

	WIN32_FIND_DATAA findData;
	auto handle = FindFirstFileExA(directory.c_str(), 
		FindExInfoBasic, &findData, FindExSearchNameMatch, NULL, 0);

	if (handle != INVALID_HANDLE_VALUE)
	{
		do
		{
			const std::string filename(findData.cFileName);
			if(filename.find(fileName_) != std::string::npos)
			{
				files.push_back(std::make_pair(
					uint64_t(findData.ftCreationTime.dwHighDateTime) << 32 | findData.ftCreationTime.dwLowDateTime,
					fileFolder_ + filename));
			}

		} while (FindNextFileA(handle, &findData));
		FindClose(handle);
	}

	const int amountToRemove = (int)files.size() - utils::MaxFileCount;
	if (amountToRemove > 0)
	{
		std::sort(files.begin(), files.end(), [](const auto& file1, const auto& file2)
		{
			return file1.first < file2.first;
		});

		for (int i = 0; i < amountToRemove; ++i)
		{
			if(DeleteFileA(files[i].second.c_str()) == FALSE)
			{
				NGT_ERROR_MSG("Failed to remove log file %s", files[i].second.c_str());
			}
		}
	}
}

void FileLogger::logQueueToFile()
{
	if (file_ && queueSize_ > 0)
	{
		(*file_) << queue_.rdbuf();
		queue_.str("");
		queue_.seekp(0);
		queue_.clear();
		queueSize_ = 0;
	}

	if (shutdown_)
	{
		flushFile();
	}
	else if (isFileMaxSize())
	{
		switchToNewFile();
	}
}

void FileLogger::logToFile(LogLevel level, const char* message)
{
	if (file_)
	{
		(*file_) << getLevelString(level) << message;

		if(shutdown_)
		{
			flushFile();
		}
		else if(isFileMaxSize())
		{
			switchToNewFile();
		}
	}
}

void FileLogger::logToFile(const char* message)
{
	if (file_)
	{
		(*file_) << message;

		if (shutdown_)
		{
			flushFile();
		}
		else if (isFileMaxSize())
		{
			switchToNewFile();
		}
	}
}

void FileLogger::flushFile()
{
	if (file_)
	{
		file_->flush();
	}
}

void FileLogger::switchToNewFile()
{
	closeFile();
	createFile(++fileId_);
}

bool FileLogger::isFileMaxSize() const
{
	struct stat stat_buf;
	int resultCode = stat(filePath_.c_str(), &stat_buf);
	return resultCode == 0 && stat_buf.st_size >= utils::MaxFileBytes;
}

const char* FileLogger::getLevelString(LogLevel level) const
{
	switch (level)
	{
	case LOG_FATAL:
		return "\n(FATAL)   ";
	case LOG_ERROR:
		return "\n(ERROR)   ";
	case LOG_WARNING:
		return "\n(WARNING) ";
	case LOG_INFO:
		return "\n(INFO)    ";
	default:
		return "\n(DEBUG)   ";
	}
}

void FileLogger::onApplicationShutdown()
{
	flushQueue();
	logQueueToFile();
	shutdown_ = true;
}

void FileLogger::flushQueue()
{
	if (duplicateCount_ > 0)
	{
		const auto duplicateStr = " [x" + std::to_string(duplicateCount_ + 1) + "]";
		queue_ << duplicateStr;
		queueSize_ += (int)duplicateStr.size();
		duplicateCount_ = 0;
		lastMessage_.clear();
	}
}

void FileLogger::out(ILogMessage* message)
{
	if (message)
	{
		logToQueue(message->getLevel(), message->str());
	}
}

void FileLogger::log(LogLevel level, const char* message)
{
	if(shutdown_)
	{
		logToFile(level, message);
	}
	else if(loggingSystem_)
	{
		loggingSystem_->log(level, message);
	}
	else
	{
		logToQueue(level, message);
	}
}

void FileLogger::logToQueue(LogLevel level, const std::string& message)
{
	if(enabled_ && !message.empty())
	{
		const std::string levelString(getLevelString(level));
		if (lastMessage_ == message)
		{
			if (duplicateCount_ != UINT_MAX)
			{
				++duplicateCount_;
			}
		}
		else
		{
			flushQueue();

			lastMessage_ = message;
			queue_ << levelString << message;
			queueSize_ += message.size() + levelString.size();

			if (level <= LogLevel::LOG_ALERT)
			{
				logQueueToFile();
				flushFile();
			}
			else if (queueSize_ >= utils::MaxQueue)
			{
				logQueueToFile();
			}
		}
	}
}

const char* FileLogger::exceptionCodeAsString(DWORD code) const
{
	switch (code)
	{
	case EXCEPTION_ACCESS_VIOLATION:
		return "ACCESS_VIOLATION";
	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
		return "ARRAY_BOUNDS_EXCEEDED";
	case EXCEPTION_BREAKPOINT:
		return "BREAKPOINT";
	case EXCEPTION_DATATYPE_MISALIGNMENT:
		return "DATATYPE_MISALIGNMENT";
	case EXCEPTION_FLT_DENORMAL_OPERAND:
		return "FLT_DENORMAL_OPERAND";
	case EXCEPTION_FLT_DIVIDE_BY_ZERO:
		return "FLT_DIVIDE_BY_ZERO";
	case EXCEPTION_FLT_INEXACT_RESULT:
		return "FLT_INEXACT_RESULT";
	case EXCEPTION_FLT_INVALID_OPERATION:
		return "FLT_INVALID_OPERATION";
	case EXCEPTION_FLT_OVERFLOW:
		return "FLT_OVERFLOW";
	case EXCEPTION_FLT_STACK_CHECK:
		return "FLT_STACK_CHECK";
	case EXCEPTION_FLT_UNDERFLOW:
		return "FLT_UNDERFLOW";
	case EXCEPTION_GUARD_PAGE:
		return "GUARD_PAGE";
	case EXCEPTION_ILLEGAL_INSTRUCTION:
		return "ILLEGAL_INSTRUCTION";
	case EXCEPTION_IN_PAGE_ERROR:
		return "IN_PAGE_ERROR";
	case EXCEPTION_INT_DIVIDE_BY_ZERO:
		return "INT_DIVIDE_BY_ZERO";
	case EXCEPTION_INVALID_DISPOSITION:
		return "INVALID_DISPOSITION";
	case EXCEPTION_INVALID_HANDLE:
		return "INVALID_HANDLE";
	case EXCEPTION_NONCONTINUABLE_EXCEPTION:
		return "NONCONTINUABLE_EXCEPTION";
	case EXCEPTION_PRIV_INSTRUCTION:
		return "PRIV_INSTRUCTION";
	case EXCEPTION_SINGLE_STEP:
		return "SINGLE_STEP";
	case EXCEPTION_STACK_OVERFLOW:
		return "STACK_OVERFLOW";
	default:
		return code == std::stoul("0xe06d7363", nullptr, 16) 
			? "STD_EXCEPTION" : "UNKNOWN";
	}
}

const char* FileLogger::accessViolationTypeAsString(ULONG_PTR type) const
{
	switch (type)
	{
	case 0:
		return "Read";
	case 1:
		return "Write";
	case 8:
		return "Data Execution";
	default:
		return "Unknown";
	}
}

DWORD FileLogger::exceptionHandler(DWORD exceptionCode, struct _EXCEPTION_POINTERS * ep)
{
	onApplicationShutdown();

	HMODULE module;
	::GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, 
		static_cast<LPCTSTR>(ep->ExceptionRecord->ExceptionAddress), &module);
	MODULEINFO moduleInfo;
	::GetModuleInformation(::GetCurrentProcess(), module, &moduleInfo, sizeof(moduleInfo));
	char moduleFileName[MAX_PATH];
	::GetModuleFileNameExA(::GetCurrentProcess(), module, moduleFileName, MAX_PATH);

	const int recordSize = 1024;
	char record[recordSize];
	record[0] = '\0';
	PEXCEPTION_RECORD er = ep->ExceptionRecord;
	if ((exceptionCode == EXCEPTION_ACCESS_VIOLATION) &&
		(er->NumberParameters >= 2))
	{
		sprintf(record, "(%s @ 0x%p)",
			accessViolationTypeAsString(er->ExceptionInformation[0]),
			(void*)er->ExceptionInformation[1]);
	}
	record[recordSize - 1] = '\0';

	const int bufferSize = 2048;
	char buffer[bufferSize];
	sprintf(buffer, "Exception thrown at 0x%p in %s: %s (0x%08x) %s\n",
		er->ExceptionAddress,
		moduleFileName,
		exceptionCodeAsString(exceptionCode),
		exceptionCode,
		record);
	buffer[bufferSize - 1] = '\0';
	logToFile(LOG_FATAL, buffer);

	if(exceptionCode != EXCEPTION_STACK_OVERFLOW)
	{
		const size_t framesToSkip = 2;
		NGTAllocator::printCallstack(framesToSkip,
			[this](const char* line) { logToFile(line); });
	}

	return EXCEPTION_CONTINUE_SEARCH;
}

DWORD FileLogger::onFatalException(FileLogger* logger, DWORD exceptionCode, struct _EXCEPTION_POINTERS * ep)
{
	if (!utils::HandlingException && logger)
	{
		utils::HandlingException = true;
		logger->exceptionHandler(exceptionCode, ep);
	}
	return EXCEPTION_CONTINUE_SEARCH;
}

} // end namespace wgt
