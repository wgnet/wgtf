#ifndef QML_VIEW_HPP
#define QML_VIEW_HPP

#include "core_ui_framework/layout_hint.hpp"
#include "core_common/signal.hpp"
#include "core_qt_common/qt_new_handler.hpp"

#include "i_qt_view.hpp"
#include <memory>
#include <string>
#include <set>
#include <QObject>
#include <QQuickWindow>

class QObject;
class QUrl;
class QQmlContext;
class QQmlEngine;
class QQuickWidget;
class QString;
class QVariant;

namespace wgt
{
class IQtFramework;

class QmlView : public QObject, public IQtView
{
	Q_OBJECT
	Q_PROPERTY(bool needsToLoad READ getNeedsToLoad NOTIFY needsToLoadChanged)
	DECLARE_QT_MEMORY_HANDLER
public:
	QmlView(const char* id, IQtFramework& qtFramework, QQmlEngine& qmlEngine);
	virtual ~QmlView();

	const char* id() const override;
	const char* title() const override;
	const char* windowId() const override;
	const LayoutHint& hint() const override;
	void update() override;

	QWidget* releaseView() override;
	void retainView() override;
	QWidget* view() const override;
	const std::set<QString>& componentTypes() const;

	void setContextObject(QObject* object);
	void setContextProperty(const QString& name, const QVariant& property);

	bool load(const QUrl& qUrl, std::function<void()> loadedHandler = [] {}, std::function<void()> errorHandler = [] {},
	          bool async = true);

	virtual void focusInEvent() override;
	virtual void focusOutEvent() override;

	virtual void registerListener(IViewEventListener* listener) override;
	virtual void deregisterListener(IViewEventListener* listener) override;

	void setNeedsToLoad(bool load);
	bool getNeedsToLoad() const;

signals:
	void needsToLoadChanged();

public slots:
	void error(QQuickWindow::SceneGraphError error, const QString& message);
	void reload(const QString& url);

private:
	struct Impl;
	std::unique_ptr<Impl> impl_;
	bool needLoad_;
};
} // end namespace wgt
#endif // QML_VIEW_HPP
