#ifndef QML_WINDOW_HPP
#define QML_WINDOW_HPP

#include "core_ui_framework/i_window.hpp"
#include "core_ui_framework/layout_hint.hpp"
#include <memory>
#include <QObject>
#include <QQuickWindow>

class QUrl;
class QQmlContext;
class QQmlEngine;
class QQuickWidget;
class QString;
class QVariant;
class QWindow;

namespace wgt
{
class IComponentContext;
class IUIApplication;

class QmlWindow : public QObject, public IWindow
{
	Q_OBJECT
public:
	QmlWindow( IComponentContext & context, QQmlEngine & qmlEngine );
	virtual ~QmlWindow();

	const char * id() const override;
	const char * title() const override;
	void update() override;
	void close() override;

	void setIcon(const char* path) override;
	void show( bool wait = false ) override;
	void showMaximized( bool wait = false ) override;
	void showModal() override;
	void hide() override;
	void title(const char* title) override;

	const Menus & menus() const override;
	const Regions & regions() const override;
	IStatusBar* statusBar() const override;

	void setContextObject( QObject * object );
	void setContextProperty( const QString & name, const QVariant & property );

	QQuickWidget * release();
	QQuickWidget * window() const;
	bool load( QUrl & qUrl, bool async,
		std::function< void() > loadedHandler );

	bool eventFilter( QObject * object, QEvent * event ) override;

	public Q_SLOTS:
		void error( QQuickWindow::SceneGraphError error, const QString &message );

Q_SIGNALS: 
		void windowClosed();

private slots:
    void onPrePreferencesChanged();
    void onPostPreferencesChanged();
    void onPrePreferencesSaved();

private:
	void waitForWindowExposed();
    void savePreference();

	struct Impl;
	std::unique_ptr< Impl > impl_;
};
} // end namespace wgt
#endif//QML_WINDOW_HPP
