#pragma once

#include "core_common/signal.hpp"
#include "core_logging_system/log_level.hpp"

namespace wgt
{
	class LogMessage;
	/*
	* This class will listen to log error messages from construction until
	* it either detects the watched resource as completing a load, or
	* it is explicitly signaled as completed using the notifyCompleted function.
	* The error messages collected are then broadcast through the
	* OnOperationCompleted signal.
	*/
	class ResourceLogListener abstract
	{
	public:
		/**
		* Connect to a signal that will fire once the operation has completed.
		* The signal will contain a list of all the error messsages that occured.
		*/
		typedef void OnOperationCompleted(const std::vector<LogMessage>& logMessages);
		typedef std::function<OnOperationCompleted> OnCompletedCallback;
		virtual Connection connectOnCompleted(OnCompletedCallback callback);

		/*
		* Explicitly notify the listener that the operation has completed.
		* The log listener will be suspended.
		*/
		virtual void notifyCompleted();

		virtual ~ResourceLogListener();

		// Creates a simple log listener (requires explicit notification of completion)
		static std::unique_ptr<ResourceLogListener> createLogListener(LogLevel logLevel);
		// Creates a log listener that will trigger on automatic notification of an ILoadingEventSource that the load has completed
		static std::unique_ptr<ResourceLogListener> createLoadingEventSourceListener(const std::string& filename, LogLevel logLevel = LOG_ERROR);
		// Creates a log listener with a default handler that manages a loading dialog (either explicit notification or automatic)
		static std::unique_ptr<ResourceLogListener> createLoadListenerWithDefaultHandler(const std::string& filename, bool isLoadingEventListener = false, LogLevel logLevel = LOG_ERROR);
		// Creates a log listener with a default handler that manages a saving dialog
		static std::unique_ptr<ResourceLogListener> createSaveListenerWithDefaultHandler(const std::string& filename, std::function<bool()> successCheck, LogLevel logLevel = LOG_ERROR);
	protected:
		struct Impl;
		ResourceLogListener(std::unique_ptr<Impl>&&, LogLevel);
		std::unique_ptr<Impl> impl_;
	};
}