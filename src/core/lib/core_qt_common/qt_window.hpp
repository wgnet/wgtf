#ifndef QT_WINDOW_HPP
#define QT_WINDOW_HPP

#include "core_ui_framework/i_window.hpp"
#include "qt_connection_holder.hpp"
#include "core_qt_common/qt_new_handler.hpp"
#include <map>
#include <memory>
#include <vector>
#include <QObject>

class QAction;
class QDockWidget;
class QIODevice;
class QMainWindow;
class QMenuBar;
class QTabWidget;
class QToolBar;
class QWidget;
class QEvent;

namespace wgt
{
struct LayoutHint;
class IUIApplication;
class IQtFramework;

class QtWindow : public QObject, public IWindow
{
	Q_OBJECT
public:
	QtWindow( IQtFramework & qtFramework, QIODevice & source );
    QtWindow( IQtFramework & qtFramework, std::unique_ptr<QMainWindow> && mainWindow );
	virtual ~QtWindow();

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

	QMainWindow * window() const;
    QMainWindow * releaseWindow();
	bool isReady() const;
    bool isLoadingPreferences() const;
signals:
	void windowReady();



protected:
    void init();
	bool eventFilter( QObject * obj, QEvent * event ) Q_DECL_OVERRIDE;

private slots:
    void onPrePreferencesChanged();
     void onPostPreferencesChanged();
    void onPrePreferencesSaved();

private:
	void waitForWindowExposed();
	void savePreference();
	bool loadPreference();
	IQtFramework & qtFramework_;
	std::unique_ptr< QMainWindow > mainWindow_;

	std::string id_;
	Menus menus_;
	Regions regions_;
	std::unique_ptr<IStatusBar> statusBar_;
	Qt::WindowModality modalityFlag_;
	IUIApplication * application_;
	bool isMaximizedInPreference_;
	bool firstTimeShow_;
    bool loadingPreferences_;
    QtConnectionHolder qtConnections_;
};
} // end namespace wgt
#endif//QT_WINDOW_HPP
