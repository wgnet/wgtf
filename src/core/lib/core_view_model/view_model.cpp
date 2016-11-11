#include "view_model.hpp"

namespace wgt
{
ViewModel::ViewModel()
{
}

ViewModel::~ViewModel()
{
}

void ViewModel::setTitle(const char* title)
{
	if (title == nullptr)
	{
		title_.clear();
		return;
	}

	title_ = title;
}

void ViewModel::setWindowId(const char* windowId)
{
	if (windowId == nullptr)
	{
		windowId_.clear();
		return;
	}

	windowId_ = windowId;
}

void ViewModel::addLayoutHint(const char* hint, float strength)
{
	if (hint == nullptr)
	{
		return;
	}

	layoutHints_ += LayoutHint(hint, strength);
}

const std::string& ViewModel::getTitle() const
{
	return title_;
}

const std::string& ViewModel::getWindowId() const
{
	return windowId_;
}

const std::map<std::string, float>& ViewModel::getLayoutHints() const
{
	return layoutHints_.hints();
}
} // end namespace wgt
