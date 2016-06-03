#ifndef I_APPLICATION_LISTENER_HPP
#define I_APPLICATION_LISTENER_HPP

namespace wgt
{
class IApplicationListener
{
public:
	virtual void applicationStarted() = 0;
	virtual void applicationStopped() = 0;
};
} // end namespace wgt
#endif // I_APPLICATION_LISTENER_HPP
