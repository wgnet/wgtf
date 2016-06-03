#ifndef I_APPLICATION_HPP
#define I_APPLICATION_HPP

#include "core_common/signal.hpp"

namespace wgt
{
class IApplication
{
	typedef Signal<void(void)> SignalVoid;

public:
	virtual ~IApplication() {}

	virtual int startApplication() = 0;
	virtual void quitApplication() = 0;

	SignalVoid signalStartUp;
	SignalVoid signalUpdate;
};
} // end namespace wgt
#endif //I_APPLICATION_HPP
