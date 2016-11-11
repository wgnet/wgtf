#ifndef VIEW_MODEL_HPP
#define VIEW_MODEL_HPP

#include "core_reflection/reflected_object.hpp"
#include "core_ui_framework/layout_hint.hpp"

namespace wgt
{
class ViewModel
{
	DECLARE_REFLECTED

public:
	ViewModel();
	virtual ~ViewModel();

	void setTitle(const char* title);
	void setWindowId(const char* windowId);
	void addLayoutHint(const char* hint, float strength = 1.0f);

	const std::string& getTitle() const;
	const std::string& getWindowId() const;
	const std::map<std::string, float>& getLayoutHints() const;

private:
	std::string title_;
	std::string windowId_;
	LayoutHint layoutHints_;
};

} // end namespace wgt
#endif // VIEW_MODEL_HPP
