#include "qt_global_logger.hpp"
#include "core_logging_system/interfaces/i_logging_system.hpp"
#include "core_logging/logging.hpp"

#define WINDOWS_LEAN_AND_MEAN
#include <windows.h>

namespace wgt
{
ILoggingSystem* QtGlobalLogger::loggingSystem_ = nullptr;

QtGlobalLogger::QtGlobalLogger()
{
	registerCallback(
		[this](ILoggingSystem& logger) { loggingSystem_ = &logger; },
		[this](ILoggingSystem& logger) { loggingSystem_ = nullptr; });
}

void QtGlobalLogger::logError(const QString& error)
{
	NGT_ERROR_MSG(error.toUtf8().constData());
}

void QtGlobalLogger::logWarning(const QString& warning)
{
	NGT_WARNING_MSG(warning.toUtf8().constData());
}

void QtGlobalLogger::logInfo(const QString& info)
{
	NGT_INFO_MSG(info.toUtf8().constData());
}

void QtGlobalLogger::messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
	// based off qDefaultMessageHandler
	QString msgFormat = qFormatLogMessage(type, context, msg);
	if (!msgFormat.isNull())
	{
		msgFormat.append(QLatin1Char('\n'));
		OutputDebugStringW(reinterpret_cast<const wchar_t*>(msgFormat.utf16()));
	}
	if (loggingSystem_)
	{
		loggingSystem_->log(LOG_DEBUG, msg.toUtf8().constData());
	}
}

} // end namespace wgt
