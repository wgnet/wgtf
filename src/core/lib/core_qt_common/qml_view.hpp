#pragma once

#include "qt_view_common.hpp"
#include "core_qt_common/qt_new_handler.hpp"

#include <memory>
#include <QObject>
#include <QQuickWindow>

class QUrl;
class QQmlEngine;
class QString;
class QVariant;

namespace wgt
{
class IQtFramework;

class QmlView : public QObject, public QtViewCommon
{
	Q_OBJECT
	Q_PROPERTY(bool needsToLoad READ getNeedsToLoad NOTIFY needsToLoadChanged)
	DECLARE_QT_MEMORY_HANDLER

public:
	QmlView(const char* id, QQmlEngine& qmlEngine);
	virtual ~QmlView();

	virtual void update() override;
	virtual void reload() override;
	virtual void setFocus(bool focus) override;

	const std::set<QString>& componentTypes() const;

	void setContextObject(QObject* object);
	void setContextProperty(const QString& name, const QVariant& property);

	bool load(const QUrl& qUrl, std::function<void()> loadedHandler = [] {},
		std::function<void()> errorHandler = [] {}, bool async = true);

	virtual CursorId getCursor() const override;
	virtual void setCursor(CursorId cursorId) override;
	virtual void unsetCursor() override;

	void setNeedsToLoad(bool load);
	bool getNeedsToLoad() const;

signals:
	void needsToLoadChanged();

public slots:
	void error(QQuickWindow::SceneGraphError error, const QString& message);
	void reload(const QString& url);
	void onFocusChanged(bool focused);

private:
	struct Implementation;
	friend Implementation;
	std::unique_ptr<Implementation> impl_;
};
} // end namespace wgt
