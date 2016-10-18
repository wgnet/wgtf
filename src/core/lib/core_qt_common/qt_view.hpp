#pragma once

#include "i_qt_view.hpp"
#include "qt_connection_holder.hpp"
#include "core_qt_common/qt_new_handler.hpp"
#include "core_ui_framework/layout_hint.hpp"
#include <memory>
#include <vector>
#include <QObject>
#include <QWidget>

class QUrl;
class QQmlContext;
class QQmlEngine;
class QQuickWidget;
class QString;
class QVariant;
class QIODevice;

namespace wgt
{
class IQtFramework;

class QtView : public QObject, public IQtView
{
	Q_OBJECT
public:
	QtView(const char* id, IQtFramework& qtFramework, QIODevice& source);
	virtual ~QtView();

	const char* id() const override;
	const char* title() const override;
	const char* windowId() const override;
	const LayoutHint& hint() const override;
	void update() override;

	QWidget* releaseView() override;
	void retainView() override;
	QWidget* view() const override;

	virtual void focusInEvent() override;
	virtual void focusOutEvent() override;

	virtual void registerListener(IViewEventListener* listener) override;
	virtual void deregisterListener(IViewEventListener* listener) override;

protected:
	void init();

private:
	IQtFramework& qtFramework_;
	QWidget* widgetView_;
	std::string id_;
	std::string title_;
	std::string windowId_;
	LayoutHint hint_;
	bool released_;
	typedef std::vector<IViewEventListener*> Listeners;
	Listeners listeners_;
};
}