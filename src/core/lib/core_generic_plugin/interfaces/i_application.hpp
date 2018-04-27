#ifndef I_APPLICATION_HPP
#define I_APPLICATION_HPP

#include "core_common/signal.hpp"

namespace wgt
{
class IApplication
{
	typedef Signal<void(void)> SignalVoid;

public:
	typedef std::function<void(void)> TimerCallback;
	typedef int TimerId;

	virtual ~IApplication()
	{
	}

	virtual int startApplication() = 0;
	virtual void quitApplication() = 0;
	virtual TimerId startTimer(int interval_ms, TimerCallback callback) = 0;
	virtual void killTimer(TimerId id) = 0;
	virtual void setAppSettingsName(const char* name) = 0;
	virtual const char* getAppSettingsName() = 0;

	SignalVoid signalStartUp;
	SignalVoid signalUpdate;
	SignalVoid signalExit;
};
} // end namespace wgt
#endif // I_APPLICATION_HPP
