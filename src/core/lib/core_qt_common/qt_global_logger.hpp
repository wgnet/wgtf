#pragma once
#include <QtQuick/QQuickItem>
#include "core_dependency_system/depends.hpp"
#include "core_dependency_system/context_callback_helper.hpp"

namespace wgt
{
	enum LogLevel;

	class QtGlobalLogger : public QQuickItem, ContextCallBackHelper, Depends<class ILoggingSystem>
	{
		Q_OBJECT

	public:
		QtGlobalLogger();

		Q_INVOKABLE void logError(const QString& error);
		Q_INVOKABLE void logWarning(const QString& warning);
		Q_INVOKABLE void logInfo(const QString& info);

		static void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

	private:
		static ILoggingSystem* loggingSystem_;
	};

} // end namespace wgt
