#ifndef I_ALERT_PRESENTER_HPP
#define I_ALERT_PRESENTER_HPP

#include <stdint.h>

namespace wgt
{
class IAlertPresenter
{
public:
	virtual ~IAlertPresenter()
	{
	}
	virtual void show(const char* text) = 0;
};
} // end namespace wgt
#endif // I_ALERT_PRESENTER_HPP
