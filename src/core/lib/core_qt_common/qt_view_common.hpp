#pragma once

#include "i_qt_view.hpp"
#include "core_ui_framework/layout_hint.hpp"

#include <memory>
#include <QWidget>

namespace wgt
{
class IMenu;

class QtViewCommon : public IQtView
{
public:
	QtViewCommon(const char* id);
	virtual ~QtViewCommon();

	virtual const char* id() const override;
	virtual const char* title() const override;
	virtual void title(const char* title) override;
	virtual const char* windowId() const override;
	virtual const LayoutHint& hint() const override;
	virtual uintptr_t getNativeWindowHandle() override;

	QWidget* releaseWidget() override;
	void retainWidget() override;
	QWidget* widget() const override;

	virtual bool focused() override;
	virtual void setFocus(bool focus) override;
	virtual void focusChanged() override;
	virtual Connection connectFocusChanged(const BooleanCallback& callback) override;
	virtual Connection connectTitleChanged(const CharStringCallback& callback) override;

protected:
	void initialise(QObject* object);
	void setWidget(QWidget* widget);
	void setWindowId(const char* id);
	LayoutHint& hint();
	void deleteWidget();

private:
	struct Implementation;
	std::unique_ptr<Implementation> impl_;
};
}