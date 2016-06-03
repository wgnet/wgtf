#ifndef QML_WINDOW_HPP
#define QML_WINDOW_HPP

#include "core_ui_framework/i_window.hpp"
#include "core_ui_framework/layout_hint.hpp"
#include <map>
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
class IQtFramework;
class IUIApplication;

class QmlWindow : public QObject, public IWindow
{
	Q_OBJECT
public:
	QmlWindow( IQtFramework & qtFramework, QQmlEngine & qmlEngine );
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

	const Menus & menus() const override;
	const Regions & regions() const override;
	IStatusBar* statusBar() const override;

	void setApplication( IUIApplication * application ) override;
	IUIApplication * getApplication() const override;

	void setContextObject( QObject * object );
	void setContextProperty( const QString & name, const QVariant & property );

	QQuickWidget * release();
	QQuickWidget * window() const;
	bool load( QUrl & qUrl );

	bool eventFilter( QObject * object, QEvent * event ) override;

	public Q_SLOTS:
		void error( QQuickWindow::SceneGraphError error, const QString &message );

private:
	void waitForWindowExposed();
    void savePreference();
    bool loadPreference();
	IQtFramework & qtFramework_;
    QQmlEngine  & qmlEngine_;
	std::unique_ptr< QQmlContext > qmlContext_;
	QQuickWidget* mainWindow_;
	std::string id_;
	std::string title_;
	Menus menus_;
	Regions regions_;
	std::unique_ptr<IStatusBar> statusBar_;
	bool released_;
	Qt::WindowModality modalityFlag_;
	IUIApplication * application_;
    bool isMaximizedInPreference_;
    bool firstTimeShow_;
};
} // end namespace wgt
#endif//QML_WINDOW_HPP
