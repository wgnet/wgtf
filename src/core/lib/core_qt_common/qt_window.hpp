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
class IComponentContext;

class QtWindow : public QObject, public IWindow
{
	Q_OBJECT
public:
	QtWindow( IComponentContext & context, QIODevice & source );
    QtWindow( IComponentContext & context, std::unique_ptr<QMainWindow> && mainWindow );
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
	void onPaletteChanged();

private:
	struct Impl;
	std::unique_ptr< Impl > impl_;
};
} // end namespace wgt
#endif//QT_WINDOW_HPP
