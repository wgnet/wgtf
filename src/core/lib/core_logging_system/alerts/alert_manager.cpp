#include "alert_manager.hpp"
#include "i_alert_presenter.hpp"
#include "basic_alert_logger.hpp"

namespace wgt
{
AlertManager::AlertManager()
{
}

AlertManager::~AlertManager()
{
}

bool AlertManager::registerPresenter(IAlertPresenter* presenter)
{
	if (std::find(presenters_.begin(), presenters_.end(), presenter) != presenters_.end())
	{
		// Presenter already registered
		return false;
	}

	presenters_.push_back(presenter);

	return true;
}

bool AlertManager::unregisterPresenter(IAlertPresenter* presenter)
{
	std::vector<IAlertPresenter*>::iterator itrPresenter = std::find(presenters_.begin(), presenters_.end(), presenter);

	if (itrPresenter != presenters_.end())
	{
		presenters_.erase(itrPresenter);
		return true;
	}

	// Presenter not found
	return false;
}

bool AlertManager::add(const char* text)
{
	// Call on any registered alert presenters to handle the alert.
	// Note that if none have been registered, the alerts will not display!
	tPresenterVector::iterator itrPresenter = presenters_.begin();
	tPresenterVector::iterator itrPresenterEnd = presenters_.end();
	for (; itrPresenter != itrPresenterEnd; ++itrPresenter)
	{
		IAlertPresenter* presenter = (*itrPresenter);
		if (presenter != nullptr)
		{
			presenter->show(text);
		}
	}

	return true;
}
} // end namespace wgt
