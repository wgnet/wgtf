#ifndef ALERT_MANAGER_HPP
#define ALERT_MANAGER_HPP

#include <map>
#include <vector>
#include <stdint.h>

namespace wgt
{
class IAlertPresenter;

class AlertManager
{
public:
	AlertManager();
	virtual ~AlertManager();

	// Registration and deregistration of alert presentations
	bool registerPresenter(IAlertPresenter* presenter);
	bool unregisterPresenter(IAlertPresenter* presenter);

	bool add(const char* text);

private:
	static const unsigned int sDefaultDisplayTime = 5000;

	typedef std::vector<IAlertPresenter*> tPresenterVector;
	tPresenterVector presenters_;
};
} // end namespace wgt
#endif // ALERT_MANAGER_HPP
