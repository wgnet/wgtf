#ifndef I_LOGGER_HPP
#define I_LOGGER_HPP

namespace wgt
{
class ILogMessage;

class ILogger
{
public:
	virtual void out(ILogMessage* message) = 0;
};
} // end namespace wgt
#endif // I_LOGGER_HPP
